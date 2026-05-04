/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2026 MuseScore Limited and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "audiocontext.h"

#include "audio/common/audiosanitizer.h"
#include "audio/common/audioerrors.h"
#include "audio/common/audioutils.h"

#include "nodes/trackchain.h"

#include "contextplayer.h"

#include "muse_framework_config.h"
#ifdef MUSE_MODULE_AUDIO_EXPORT
#include "export/soundtrackwriter.h"
#endif

using namespace muse;
using namespace muse::audio;
using namespace muse::audio::engine;

AudioContext::AudioContext(const AudioCtxId& ctxId)
    : m_ctxId(ctxId)
{
    m_player = std::make_shared<ContextPlayer>(this);

    m_playheadNode = std::make_shared<PlayheadNode>(std::static_pointer_cast<IPlayhead>(m_player));
    m_mixer = std::make_shared<Mixer>();
}

AudioCtxId AudioContext::id() const
{
    return m_ctxId;
}

Ret AudioContext::init()
{
    m_mixer->init();
    m_mixer->setPlayhead(std::static_pointer_cast<IPlayhead>(m_player));

    // Make the chain: audiocontext <- playheadnode <- mixer
    m_mixer->connect(m_playheadNode);

    OutputSpec outputSpec = audioEngine()->outputSpec();
    setOutputSpec(outputSpec);
    setMode(ProcessMode::Idle);

    m_player->isActiveChanged().onReceive(this, [this](bool isActive) {
        setMode(isActive ? ProcessMode::Playing : ProcessMode::Idle);
    });

    return make_ret(Ret::Code::Ok);
}

void AudioContext::deinit()
{
    ONLY_AUDIO_ENGINE_THREAD;

    m_mixer->setPlayhead(nullptr);

    removeAllTracks();

    m_player.reset();

    // Explicitly disconnect and clear all channel members before
    // async_disconnectAll() and before the destructor runs. This ensures
    // subscribers are disconnected while they're still alive, not during IoC
    // teardown when some may already be destroyed.
    m_trackRemoved = async::Channel<TrackId>();
    m_trackAdded = async::Channel<TrackId>();

    m_saveSoundTracksProgress = SaveSoundTrackProgressData();
    m_outputParamsChanged = async::Channel<TrackId, AudioOutputParams>();
    m_inputParamsChanged = async::Channel<TrackId, AudioInputParams>();

    async_disconnectAll();
}

// Config
void AudioContext::onModeChanged(const ProcessMode mode)
{
    ONLY_AUDIO_ENGINE_THREAD;
    if (mode == ProcessMode::Idle) {
        m_mixer->setTracksToProcessWhenIdle(m_tracksToProcessWhenIdle);
    }
    m_mixer->setIsIdle(mode == ProcessMode::Idle);
    m_playheadNode->setMode(mode);
    // mixer will be notified about the mode change via the playhead node
}

void AudioContext::onOutputSpecChanged(const OutputSpec& outputSpec)
{
    ONLY_AUDIO_ENGINE_THREAD;
    m_playheadNode->setOutputSpec(outputSpec);
    // mixer will be notified about the output spec change via the playhead node

    TimePosition currentPosition = m_player->currentPosition();
    if (currentPosition.isValid()) {
        m_player->seek(TimePosition::fromTime(currentPosition.time(), outputSpec.sampleRate));
    } else {
        m_player->seek(TimePosition::zero(outputSpec.sampleRate));
    }
}

// Setup tracks
TrackId AudioContext::newTrackId() const
{
    static TrackId lastId = 0;
    ++lastId;
    return lastId;
}

RetVal2<TrackId, AudioParams> AudioContext::addTrack(const std::string& trackName,
                                                     io::IODevice* playbackData,
                                                     const AudioParams&)
{
    ONLY_AUDIO_ENGINE_THREAD;

    using RetType = RetVal2<TrackId, AudioParams>;

    if (!playbackData) {
        return RetType::make_ret(Err::InvalidAudioFilePath);
    }

    TrackId trackId = newTrackId();
    Track track;
    track.type = TrackType::Sound_track;
    track.id = trackId;
    track.name = trackName;
//! NOT IMPLEMENTED YET
// track.source = source.val;
// track.output = output.val;

    doAddTrack(track);

    return RetType::make_ok(trackId, { });
}

RetVal2<TrackId, AudioParams> AudioContext::addTrack(const std::string& trackName,
                                                     const mpe::PlaybackData& playbackData, const AudioParams& params)
{
    ONLY_AUDIO_ENGINE_THREAD;

    using RetType = RetVal2<TrackId, AudioParams>;

    if (!playbackData.setupData.isValid()) {
        return RetType::make_ret(Err::InvalidSetupData);
    }

    TrackId trackId = newTrackId();

    auto onOffStreamReceived = [this, trackId]() {
        std::unordered_set<TrackId> tracksToProcess = m_tracksToProcessWhenIdle;

        if (m_prevActiveTrackId == INVALID_TRACK_ID) {
            tracksToProcess.insert(trackId);
        } else {
            tracksToProcess.insert({ m_prevActiveTrackId, trackId });
        }

        m_mixer->setTracksToProcessWhenIdle(tracksToProcess);
        m_prevActiveTrackId = trackId;
    };

    // Make source
    RetVal<AudioSourceNodePtr> source = audioFactory()->makeEventSource(trackId, playbackData, params.in, onOffStreamReceived);
    if (!source.ret) {
        return RetType::make_ret(source.ret);
    }

    // Make fx chain
    FxChainPtr fxChain = audioFactory()->makeTrackFxChain(trackId, params.out.fxChain);
    fxChain->setPlayheadPosition(std::static_pointer_cast<IPlayheadPosition>(m_player));
    fxChain->fxChainSpecChanged().onReceive(this, [this, trackId](const AudioFxChain& fxChainSpec) {
        if (Track* t = track(trackId)) {
            t->params.fxChain = fxChainSpec;
            m_outputParamsChanged.send(trackId, t->params);
        }
    });

    fxChain->shouldProcessDuringSilenceChanged().onReceive(this, [this, trackId](bool shouldProcess) {
        onShouldProcessDuringSilenceChanged(trackId, shouldProcess);
    });

    TrackChainPtr trackChain = std::make_shared<TrackChain>(trackId);
    trackChain->setOutputSpec(outputSpec());
    trackChain->setMode(mode());
    trackChain->setSource(source.val);
    trackChain->setFxChain(fxChain);
    trackChain->setControl(std::make_shared<ControlNode>());
    trackChain->setSignal(std::make_shared<SignalNode>());
    trackChain->rebuild();

    Ret ret = m_mixer->addChannel(trackChain, params.out.auxSends);
    if (!ret) {
        return RetType::make_ret(ret);
    }

    // Make track info
    Track track;
    track.type = TrackType::Event_track;
    track.id = trackId;
    track.name = trackName;
    track.params = params.out;
    track.chain = trackChain;

    track.params.fxChain = fxChain->fxChainSpec();

    doAddTrack(track);

    return RetType::make_ok(trackId, { source.val->inputParams(), track.params });
}

RetVal2<TrackId, AudioOutputParams> AudioContext::addAuxTrack(const std::string& trackName,
                                                              const AudioOutputParams& params)
{
    ONLY_AUDIO_ENGINE_THREAD;

    using RetType = RetVal2<TrackId, AudioOutputParams>;

    TrackId trackId = newTrackId();

    // Make fx chain
    FxChainPtr fxChain = audioFactory()->makeTrackFxChain(trackId, params.fxChain);
    fxChain->setPlayheadPosition(std::static_pointer_cast<IPlayheadPosition>(m_player));
    fxChain->fxChainSpecChanged().onReceive(this, [this, trackId](const AudioFxChain& fxChainSpec) {
        if (Track* t = track(trackId)) {
            t->params.fxChain = fxChainSpec;
            m_outputParamsChanged.send(trackId, t->params);
        }
    });

    fxChain->shouldProcessDuringSilenceChanged().onReceive(this, [this, trackId](bool shouldProcess) {
        onShouldProcessDuringSilenceChanged(trackId, shouldProcess);
    });

    TrackChainPtr trackChain = std::make_shared<TrackChain>(trackId);
    trackChain->setOutputSpec(outputSpec());
    trackChain->setMode(mode());
    trackChain->setFxChain(fxChain);
    trackChain->setControl(std::make_shared<ControlNode>());
    trackChain->setSignal(std::make_shared<SignalNode>());
    trackChain->rebuild();

    Ret ret = m_mixer->addAuxChannel(trackChain);
    if (!ret) {
        return RetType::make_ret(ret);
    }

    // Make track info
    Track track;
    track.type = TrackType::Event_track;
    track.id = trackId;
    track.name = trackName;
    track.params = params;
    track.chain = trackChain;

    track.params.fxChain = fxChain->fxChainSpec();

    doAddTrack(track);

    return RetType::make_ok(trackId, track.params);
}

void AudioContext::doAddTrack(const Track& track)
{
    ONLY_AUDIO_ENGINE_THREAD;
    const TrackId trackId = track.id;

    if (track.chain) {
        if (auto source = track.chain->source()) {
            source->seek(m_player->currentPosition());
            source->inputParamsChanged().onReceive(this, [this, trackId](const AudioInputParams& params) {
                m_inputParamsChanged.send(trackId, params);
            });
        }
    }

    m_tracks.push_back(track);
    m_trackAdded.send(trackId);

    updateNonMutedTrackCount();
}

void AudioContext::onOutputParamsChanged(Track& track, const AudioOutputParams& requiredParams)
{
    ONLY_AUDIO_ENGINE_THREAD;

    const TrackId trackId = track.id;
    std::shared_ptr<IPlayheadPosition> playheadPosition = std::static_pointer_cast<IPlayheadPosition>(m_player);

    // Make fx chain
    FxChainPtr fxChain = audioFactory()->makeTrackFxChain(trackId, requiredParams.fxChain);
    fxChain->setPlayheadPosition(playheadPosition);
    fxChain->fxChainSpecChanged().onReceive(this, [this, trackId](const AudioFxChain& fxChainSpec) {
        if (Track* t = this->track(trackId)) {
            t->params.fxChain = fxChainSpec;
            m_outputParamsChanged.send(trackId, t->params);
        }
    });

    fxChain->shouldProcessDuringSilenceChanged().onReceive(this, [this, trackId](bool shouldProcess) {
        onShouldProcessDuringSilenceChanged(trackId, shouldProcess);
    });

    track.chain->setFxChain(fxChain);
    track.chain->rebuild();

    const bool mutedChanged = track.params.muted != requiredParams.muted;

    track.params = requiredParams;
    track.params.fxChain = fxChain->fxChainSpec();

    if (mutedChanged && !requiredParams.muted && track.chain && track.chain->source()) {
        track.chain->source()->seek(playheadPosition->currentPosition());
    }

    if (auto control = track.chain->control()) {
        control->setVolume(muse::db_to_linear(track.params.volume));
        control->setPan(track.params.balance);
        control->setMute(track.params.muted);
    }

    updateNonMutedTrackCount();
}

void AudioContext::updateNonMutedTrackCount()
{
    ONLY_AUDIO_ENGINE_THREAD;
    size_t count = 0;
    for (const Track& t : m_tracks) {
        if (!t.params.muted) {
            count++;
        }
    }
    m_mixer->setNonMutedTrackCount(count);
}

void AudioContext::removeTrack(const TrackId trackId)
{
    ONLY_AUDIO_ENGINE_THREAD;

    auto it = std::find_if(m_tracks.begin(), m_tracks.end(), [trackId](const Track& t) {
        return t.id == trackId;
    });

    if (it == m_tracks.end()) {
        return;
    }

    Track track = *it;
    if (track.chain) {
        if (auto source = track.chain->source()) {
            source->inputParamsChanged().disconnect(this);
        }
    }

    m_mixer->removeChannel(trackId);
    m_tracks.erase(it);
    muse::remove(m_tracksToProcessWhenIdle, trackId);

    if (m_prevActiveTrackId == trackId) {
        m_prevActiveTrackId = INVALID_TRACK_ID;
    }

    m_trackRemoved.send(trackId);
}

void AudioContext::removeAllTracks()
{
    ONLY_AUDIO_ENGINE_THREAD;

    for (const TrackId& id : trackIdList().val) {
        removeTrack(id);
    }
}

async::Channel<TrackId> AudioContext::trackAdded() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_trackAdded;
}

async::Channel<TrackId> AudioContext::trackRemoved() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_trackRemoved;
}

RetVal<TrackIdList> AudioContext::trackIdList() const
{
    ONLY_AUDIO_ENGINE_THREAD;

    TrackIdList result;
    result.reserve(m_tracks.size());

    for (const auto& t : m_tracks) {
        result.push_back(t.id);
    }

    return RetVal<TrackIdList>::make_ok(result);
}

const AudioContext::Track* AudioContext::track(const TrackId id) const
{
    ONLY_AUDIO_ENGINE_THREAD;
    for (const Track& t : m_tracks) {
        if (t.id == id) {
            return &t;
        }
    }
    return nullptr;
}

AudioContext::Track* AudioContext::track(const TrackId id)
{
    ONLY_AUDIO_ENGINE_THREAD;
    for (Track& t : m_tracks) {
        if (t.id == id) {
            return &t;
        }
    }
    return nullptr;
}

RetVal<TrackName> AudioContext::trackName(const TrackId trackId) const
{
    ONLY_AUDIO_ENGINE_THREAD;

    if (const Track* t = track(trackId)) {
        return RetVal<TrackName>::make_ok(t->name);
    }

    return RetVal<TrackName>::make_ret((int)Err::InvalidTrackId, "no track");
}

AudioSourceNodePtr AudioContext::trackSource(const TrackId trackId) const
{
    ONLY_AUDIO_ENGINE_THREAD;
    if (const Track* t = track(trackId)) {
        return t->chain->source();
    }
    return nullptr;
}

std::vector<AudioSourceNodePtr> AudioContext::allTracksSources() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    std::vector<AudioSourceNodePtr> result;
    result.reserve(m_tracks.size());
    for (const Track& t : m_tracks) {
        if (t.chain) {
            if (auto source = t.chain->source()) {
                result.push_back(source);
            }
        }
    }
    return result;
}

void AudioContext::onShouldProcessDuringSilenceChanged(const TrackId trackId, bool shouldProcess)
{
    ONLY_AUDIO_ENGINE_THREAD;
    if (shouldProcess) {
        m_tracksToProcessWhenIdle.insert(trackId);
    } else {
        muse::remove(m_tracksToProcessWhenIdle, trackId);
    }

    m_mixer->setTracksToProcessWhenIdle(m_tracksToProcessWhenIdle);
}

// Sources
AudioResourceMetaList AudioContext::availableInputResources() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return audioFactory()->availableInputResources();
}

SoundPresetList AudioContext::availableSoundPresets(const AudioResourceMeta& resourceMeta) const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return audioFactory()->availableSoundPresets(resourceMeta);
}

RetVal<AudioInputParams> AudioContext::inputParams(const TrackId trackId) const
{
    ONLY_AUDIO_ENGINE_THREAD;
    if (const Track* t = track(trackId)) {
        if (t->chain) {
            if (auto source = t->chain->source()) {
                return RetVal<AudioInputParams>::make_ok(source->inputParams());
            }
        }
    }

    return RetVal<AudioInputParams>::make_ret(Err::InvalidTrackId);
}

void AudioContext::setInputParams(const TrackId trackId, const AudioInputParams& params)
{
    ONLY_AUDIO_ENGINE_THREAD;
    if (const Track* t = track(trackId)) {
        if (t->chain) {
            if (auto source = t->chain->source()) {
                source->applyInputParams(params);
            }
        }
    }
}

async::Channel<TrackId, AudioInputParams> AudioContext::inputParamsChanged() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_inputParamsChanged;
}

void AudioContext::processInput(const TrackId trackId) const
{
    ONLY_AUDIO_ENGINE_THREAD;
    if (const Track* t = track(trackId)) {
        if (t->chain) {
            if (auto source = t->chain->source()) {
                source->processInput();
            }
        }
    }
}

RetVal<InputProcessingProgress> AudioContext::inputProcessingProgress(const TrackId trackId) const
{
    ONLY_AUDIO_ENGINE_THREAD;
    if (const Track* t = track(trackId)) {
        if (auto source = t->chain->source()) {
            return RetVal<InputProcessingProgress>::make_ok(source->inputProcessingProgress());
        }
    }

    return make_ret(Err::InvalidTrackId);
}

void AudioContext::clearCache(const TrackId trackId) const
{
    ONLY_AUDIO_ENGINE_THREAD;
    if (const Track* t = track(trackId)) {
        if (auto source = t->chain->source()) {
            source->clearCache();
        }
    }
}

void AudioContext::clearSources()
{
    ONLY_AUDIO_ENGINE_THREAD;
    audioFactory()->clearSynthSources();
}

// Outputs
AudioResourceMetaList AudioContext::availableOutputResources() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return audioFactory()->availableOutputResources();
}

RetVal<AudioOutputParams> AudioContext::outputParams(const TrackId trackId) const
{
    ONLY_AUDIO_ENGINE_THREAD;
    if (const Track* t = track(trackId)) {
        return RetVal<AudioOutputParams>::make_ok(t->params);
    }

    return make_ret(Err::InvalidTrackId);
}

void AudioContext::setOutputParams(const TrackId trackId, const AudioOutputParams& params)
{
    ONLY_AUDIO_ENGINE_THREAD;
    if (Track* t = track(trackId)) {
        onOutputParamsChanged(*t, params);
        m_outputParamsChanged.send(trackId, t->params);
    }
}

async::Channel<TrackId, AudioOutputParams> AudioContext::outputParamsChanged() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_outputParamsChanged;
}

RetVal<AudioSignalChanges> AudioContext::signalChanges(const TrackId trackId) const
{
    ONLY_AUDIO_ENGINE_THREAD;
    if (const Track* t = track(trackId)) {
        if (auto signal = t->chain->signal()) {
            return RetVal<AudioSignalChanges>::make_ok(signal->audioSignalChanges());
        }
    }

    return RetVal<AudioSignalChanges>::make_ret(Err::InvalidTrackId);
}

RetVal<AudioOutputParams> AudioContext::masterOutputParams() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return RetVal<AudioOutputParams>::make_ok(m_mixer->masterOutputParams());
}

void AudioContext::setMasterOutputParams(const AudioOutputParams& params)
{
    ONLY_AUDIO_ENGINE_THREAD;
    m_mixer->setMasterOutputParams(params);
}

void AudioContext::clearMasterOutputParams()
{
    ONLY_AUDIO_ENGINE_THREAD;
    m_mixer->clearMasterOutputParams();
}

async::Channel<AudioOutputParams> AudioContext::masterOutputParamsChanged() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_mixer->masterOutputParamsChanged();
}

RetVal<AudioSignalChanges> AudioContext::masterSignalChanges() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return RetVal<AudioSignalChanges>::make_ok(m_mixer->masterAudioSignalChanges());
}

void AudioContext::clearAllFx()
{
    ONLY_AUDIO_ENGINE_THREAD;
    audioFactory()->clearAllFx();
}

// Play
async::Promise<Ret> AudioContext::prepareToPlay()
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_player->prepareToPlay();
}

void AudioContext::play(const secs_t delay)
{
    LOGD() << m_playheadNode->dump();

    ONLY_AUDIO_ENGINE_THREAD;
    m_player->play(delay);
}

void AudioContext::seek(const secs_t newPosition, const bool flushSound)
{
    ONLY_AUDIO_ENGINE_THREAD;
    m_player->seek(TimePosition::fromTime(newPosition, m_outputSpec.sampleRate), flushSound);
}

void AudioContext::stop()
{
    ONLY_AUDIO_ENGINE_THREAD;
    m_player->stop();
}

void AudioContext::pause()
{
    ONLY_AUDIO_ENGINE_THREAD;
    m_player->pause();
}

void AudioContext::resume(const secs_t delay)
{
    ONLY_AUDIO_ENGINE_THREAD;
    m_player->resume(delay);
}

void AudioContext::setDuration(const secs_t duration)
{
    ONLY_AUDIO_ENGINE_THREAD;
    m_player->setDuration(duration);
}

Ret AudioContext::setLoop(const secs_t from, const secs_t to)
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_player->setLoop(from, to);
}

void AudioContext::resetLoop()
{
    ONLY_AUDIO_ENGINE_THREAD;
    m_player->resetLoop();
}

PlaybackStatus AudioContext::playbackStatus() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_player->playbackStatus();
}

async::Channel<PlaybackStatus> AudioContext::playbackStatusChanged() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_player->playbackStatusChanged();
}

secs_t AudioContext::playbackPosition() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_player->playbackPosition();
}

async::Channel<secs_t> AudioContext::playbackPositionChanged() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_player->playbackPositionChanged();
}

// Export
async::Promise<Ret> AudioContext::saveSoundTrack(io::IODevice& dstDevice, const SoundTrackFormat& format)
{
    return async::make_promise<Ret>([this, &dstDevice, format](auto resolve, auto) {
        ONLY_AUDIO_ENGINE_THREAD;

#ifdef MUSE_MODULE_AUDIO_EXPORT
        m_player->stop();
        m_player->seek(TimePosition::zero(m_outputSpec.sampleRate));

        const bool lazyProcessingWasEnabled = configuration()->isLazyProcessingOfOnlineSoundsEnabled();
        configuration()->setIsLazyProcessingOfOnlineSoundsEnabled(false);

        listenInputProcessing([this, &dstDevice, format, lazyProcessingWasEnabled, resolve](Ret ret) {
            if (ret) {
                ret = doSaveSoundTrack(dstDevice, format);
            }

            configuration()->setIsLazyProcessingOfOnlineSoundsEnabled(lazyProcessingWasEnabled);
            (void)resolve(ret);
        });

        return async::Promise<Ret>::dummy_result();
#else
        return resolve(make_ret(Err::DisabledAudioExport, "audio export is disabled"));
#endif
    });
}

bool AudioContext::hasPendingChunks(const TrackId trackId) const
{
    ONLY_AUDIO_ENGINE_THREAD;

    if (const Track* t = track(trackId)) {
        if (auto source = t->chain->source()) {
            return source->hasPendingChunks();
        }
    }

    return false;
}

size_t AudioContext::tracksBeingProcessedCount() const
{
    size_t count = 0;

    for (TrackId trackId : trackIdList().val) {
        if (inputProcessingProgress(trackId).val.isStarted || hasPendingChunks(trackId)) {
            count++;
        }
    }

    return count;
}

void AudioContext::listenInputProcessing(std::function<void(const Ret&)> completed)
{
#ifdef MUSE_MODULE_AUDIO_EXPORT

    auto soundsInProgress = std::make_shared<size_t>(0);

    for (TrackId trackId : trackIdList().val) {
        if (!isOnlineAudioResource(inputParams(trackId).val.resourceMeta)) {
            continue;
        }

        InputProcessingProgress inputProgress = inputProcessingProgress(trackId).val;
        if (!inputProgress.isStarted && !hasPendingChunks(trackId)) {
            continue;
        }

        (*soundsInProgress)++;

        if (inputProgress.isStarted) {
            m_saveSoundTracksProgress.progress.send(0, 100, SaveSoundTrackStage::ProcessingOnlineSounds);
        }

        using StatusInfo = InputProcessingProgress::StatusInfo;
        using ChunkInfoList = InputProcessingProgress::ChunkInfoList;
        using ProgressInfo = InputProcessingProgress::ProgressInfo;

        auto ch = inputProgress.processedChannel;
        ch.onReceive(this, [this, trackId, soundsInProgress, completed](const StatusInfo& status,
                                                                        const ChunkInfoList&,
                                                                        const ProgressInfo& info) {
            const size_t tracksBeingProcessedCount = this->tracksBeingProcessedCount();

            if (status.status == InputProcessingProgress::Status::Finished) {
                inputProcessingProgress(trackId).val.processedChannel.disconnect(this);
            }

            if (tracksBeingProcessedCount == 0) {
                m_saveSoundTracksProgress.aborted.disconnect(this);
                completed(make_ok());
            } else if (tracksBeingProcessedCount == 1) {
                m_saveSoundTracksProgress.progress.send(info.current, info.total, SaveSoundTrackStage::ProcessingOnlineSounds);
            } else {
                const int64_t percentage = 100 - (100 / *soundsInProgress) * tracksBeingProcessedCount;
                m_saveSoundTracksProgress.progress.send(percentage, 100, SaveSoundTrackStage::ProcessingOnlineSounds);
            }
        });
    }

    if ((*soundsInProgress) == 0) {
        completed(make_ok());
        return;
    }

    m_saveSoundTracksProgress.aborted.onNotify(this, [this, completed]() {
        m_saveSoundTracksProgress.aborted.disconnect(this);

        for (TrackId trackId : trackIdList().val) {
            if (isOnlineAudioResource(inputParams(trackId).val.resourceMeta)) {
                inputProcessingProgress(trackId).val.processedChannel.disconnect(this);
            }
        }

        completed(make_ret(Ret::Code::Cancel));
    });
#else
    UNUSED(completed);
#endif
}

Ret AudioContext::doSaveSoundTrack(io::IODevice& dstDevice, const SoundTrackFormat& format)
{
#ifdef MUSE_MODULE_AUDIO_EXPORT
    using namespace muse::audio::soundtrack;

    const secs_t totalDuration = m_player->duration();
    auto writer = std::make_shared<SoundTrackWriter>(dstDevice, format, totalDuration, m_mixer);

    writer->progress().progressChanged().onReceive(this, [this](int64_t current, int64_t total, std::string /*title*/) {
        m_saveSoundTracksProgress.progress.send(current, total, SaveSoundTrackStage::WritingSoundTrack);
    });

    std::weak_ptr<SoundTrackWriter> weakPtr = writer;
    m_saveSoundTracksProgress.aborted.onNotify(this, [weakPtr]() {
        if (auto writer = weakPtr.lock()) {
            writer->abort();
        }
    });

    setMode(ProcessMode::PlayingOffline);
    Ret ret = writer->write();

    //! NOTE Restore source (mixer) state
    // Changes to the source and audio engine state
    // must be performed via execOperation - so that synchronization with the audio driver process works
    IAudioEngine::Operation func = [this]() {
        m_mixer->setOutputSpec(audioEngine()->outputSpec());
        setMode(ProcessMode::Idle);
    };
    audioEngine()->execOperation(OperationType::LongOperation, func);

    m_player->seek(TimePosition::zero(m_outputSpec.sampleRate));

    m_saveSoundTracksProgress.aborted.disconnect(this);

    return ret;
#else
    return make_ret(Err::DisabledAudioExport, "audio export is disabled");
#endif
}

void AudioContext::abortSavingAllSoundTracks()
{
#ifdef MUSE_MODULE_AUDIO_EXPORT
    m_saveSoundTracksProgress.aborted.notify();
#endif
}

SaveSoundTrackProgress AudioContext::saveSoundTrackProgressChanged() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_saveSoundTracksProgress.progress;
}

// Processing
void AudioContext::doSelfProcess(float* buffer, samples_t samplesPerChannel)
{
    ONLY_AUDIO_PROC_THREAD;
    m_playheadNode->process(buffer, samplesPerChannel);
}
