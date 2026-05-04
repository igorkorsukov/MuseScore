/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore Limited and others
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
#include "mixer.h"

#include "audio/common/audiosanitizer.h"
#include "audio/common/audioerrors.h"

#include "muse_framework_config.h"

#ifdef MUSE_THREADS_SUPPORT
#include "concurrency/taskscheduler.h"
#endif

#include "log.h"

using namespace muse;
using namespace muse::async;
using namespace muse::audio;
using namespace muse::audio::engine;

constexpr size_t MIN_TRACK_COUNT_FOR_MULTITHREADING = 2;

Mixer::~Mixer()
{
    ONLY_AUDIO_MAIN_OR_ENGINE_THREAD;
    delete m_taskScheduler;
}

void Mixer::init()
{
    ONLY_AUDIO_ENGINE_THREAD;

#ifdef MUSE_THREADS_SUPPORT
    m_taskScheduler = new TaskScheduler();

    if (!m_taskScheduler->setThreadsPriority(ThreadPriority::High)) {
        LOGE() << "Unable to change audio threads priority";
    }

    AudioSanitizer::setMixerThreads(m_taskScheduler->threadIdSet());
#endif

    //! Make the chain: audiocontext <- signalnode <- controlnode <- mixerchannel[n]
    m_signalNode = std::make_shared<SignalNode>();
    m_controlNode = std::make_shared<ControlNode>();

    m_controlNode->connect(m_signalNode);
    this->connect(m_controlNode);
}

Ret Mixer::addChannel(TrackChainPtr trackChain, const AuxSendsParams& auxSends)
{
    const size_t outBufferSize = m_outputSpec.samplesPerChannel * m_outputSpec.audioChannelCount;

    TrackData trackData;
    trackData.trackId = trackChain->trackId();
    trackData.chain = trackChain;
    trackData.buffer.resize(outBufferSize);

    m_tracks.emplace_back(std::move(trackData));

    m_auxSends[trackChain->trackId()] = auxSends;

    return make_ok();
}

Ret Mixer::addAuxChannel(TrackChainPtr trackChain)
{
    ONLY_AUDIO_ENGINE_THREAD;
    const size_t outBufferSize = m_outputSpec.samplesPerChannel * m_outputSpec.audioChannelCount;

    TrackData trackData;
    trackData.trackId = trackChain->trackId();
    trackData.chain = trackChain;
    trackData.buffer.resize(outBufferSize);

    m_auxTracks.emplace_back(std::move(trackData));

    return make_ok();
}

Ret Mixer::removeChannel(const TrackId trackId)
{
    ONLY_AUDIO_ENGINE_THREAD;

    bool removed = muse::remove_if(m_tracks, [trackId](const TrackData& track) {
        return track.trackId == trackId;
    });

    if (removed) {
        m_auxSends.erase(trackId);
    }

    if (!removed) {
        removed = muse::remove_if(m_auxTracks, [trackId](const TrackData& track) {
            return track.trackId == trackId;
        });
    }

    return removed ? make_ret(Ret::Code::Ok) : make_ret(Err::InvalidTrackId);
}

void Mixer::onOutputSpecChanged(const OutputSpec& spec)
{
    ONLY_AUDIO_ENGINE_THREAD;

    m_signalNode->setOutputSpec(spec);
    m_controlNode->setOutputSpec(spec);

    for (auto& t : m_tracks) {
        t.chain->setOutputSpec(spec);
    }

    for (auto& t : m_auxTracks) {
        t.chain->setOutputSpec(spec);
    }

    if (m_masterFxChain) {
        m_masterFxChain->setOutputSpec(spec);
    }
}

void Mixer::process(float* buffer, samples_t samplesPerChannel)
{
    ONLY_AUDIO_PROC_THREAD;

    //! NOTE Temporary hack
    // audiocontext -> mixer(process->doProcess)
    // -> m_signalNode
    // -> m_controlNode
    // -> mixer (process->doProcess->doSelfProcess)

    if (!m_chainProcessing) {
        m_chainProcessing = true;
        m_signalNode->process(buffer, samplesPerChannel);
        m_chainProcessing = false;
    } else {
        doSelfProcess(buffer, samplesPerChannel);
    }
}

void Mixer::doSelfProcess(float* outBuffer, samples_t samplesPerChannel)
{
    ONLY_AUDIO_PROC_THREAD;

    size_t outBufferSize = samplesPerChannel * m_outputSpec.audioChannelCount;
    std::fill(outBuffer, outBuffer + outBufferSize, 0.f);

    bool procFxOnSilence = false;
    if (m_masterFxChain) {
        procFxOnSilence = m_masterFxChain->shouldProcessDuringSilence();
    }

    if (m_isIdle && m_tracksToProcessWhenIdle.empty() && (m_signalNode->isSilent() && !procFxOnSilence)) {
        notifyNoAudioSignal();
        return;
    }

    processTrackChannels(outBufferSize, samplesPerChannel);

    prepareAuxBuffers(outBufferSize);
    for (auto& t : m_tracks) {
        if (!t.processed) {
            continue;
        }

        if (auto signal = t.chain->signal()) {
            if (signal->isSilent()) {
                continue;
            }
        }

        mixOutputFromChannel(outBuffer, t.buffer.data(), samplesPerChannel);
        writeTrackToAuxBuffers(t.buffer.data(), outBufferSize, m_auxSends[t.trackId]);
    }

    if (m_masterParams.muted || samplesPerChannel == 0 || (m_signalNode->isSilent() && !procFxOnSilence)) {
        notifyNoAudioSignal();
        return;
    }

    processAuxChannels(outBuffer, samplesPerChannel);
    processMasterFx(outBuffer, samplesPerChannel);

    notifyAboutAudioSignalChanges();
}

void Mixer::processTrackChannels(size_t outBufferSize, size_t samplesPerChannel)
{
    auto processChannel = [outBufferSize, samplesPerChannel](TrackData& trackData) {
        IF_ASSERT_FAILED(trackData.chain) {
            return;
        }

        if (trackData.buffer.size() < outBufferSize) {
            trackData.buffer.resize(outBufferSize);
        }

        std::fill(trackData.buffer.begin(), trackData.buffer.begin() + outBufferSize, 0.f);
        trackData.chain->process(trackData.buffer.data(), samplesPerChannel);
        trackData.processed = true;
    };

    bool filterTracks = m_isIdle && !m_tracksToProcessWhenIdle.empty();

#ifdef MUSE_THREADS_SUPPORT
    if (useMultithreading()) {
        std::vector<std::future<void> > futures;

        for (auto& t : m_tracks) {
            t.processed = false;

            if (filterTracks && !muse::contains(m_tracksToProcessWhenIdle, t.trackId)) {
                continue;
            }

            if (auto signal = t.chain->signal()) {
                bool muted = t.chain->control()->mute();
                if (muted && signal->isSilent()) {
                    signal->setNoAudioSignal();
                    continue;
                }
            }

            std::future<void> future = m_taskScheduler->submit(processChannel, std::ref(t));
            futures.emplace_back(std::move(future));
        }

        for (auto& f : futures) {
            f.wait();
        }
    } else
#endif
    {
        for (auto& t : m_tracks) {
            t.processed = false;

            if (filterTracks && !muse::contains(m_tracksToProcessWhenIdle, t.trackId)) {
                continue;
            }

            if (auto signal = t.chain->signal()) {
                bool muted = t.chain->control()->mute();
                if (muted && signal->isSilent()) {
                    signal->setNoAudioSignal();
                    continue;
                }
            }

            processChannel(t);
        }
    }
}

void Mixer::setNonMutedTrackCount(size_t count)
{
    m_nonMutedTrackCount = count;
}

bool Mixer::useMultithreading() const
{
#ifdef MUSE_THREADS_SUPPORT
    if (m_nonMutedTrackCount < MIN_TRACK_COUNT_FOR_MULTITHREADING) {
        return false;
    }

    if (m_isIdle) {
        if (m_tracksToProcessWhenIdle.size() < MIN_TRACK_COUNT_FOR_MULTITHREADING) {
            return false;
        }
    }

    return true;
#else
    return false;
#endif
}

void Mixer::onModeChanged(const ProcessMode mode)
{
    ONLY_AUDIO_ENGINE_THREAD;

    for (auto& t : m_tracks) {
        t.chain->setMode(mode);
    }

    for (auto& t : m_auxTracks) {
        t.chain->setMode(mode);
    }

    if (m_masterFxChain) {
        m_masterFxChain->setMode(mode);
    }
}

void Mixer::setPlayhead(PlayheadPtr playhead)
{
    ONLY_AUDIO_ENGINE_THREAD;
    m_playhead = playhead;

    for (auto& t : m_tracks) {
        t.chain->fxChain()->setPlayheadPosition(playhead);
    }

    for (auto& t : m_auxTracks) {
        t.chain->fxChain()->setPlayheadPosition(playhead);
    }
}

AudioOutputParams Mixer::masterOutputParams() const
{
    ONLY_AUDIO_ENGINE_THREAD;

    return m_masterParams;
}

void Mixer::setMasterOutputParams(const AudioOutputParams& params)
{
    ONLY_AUDIO_ENGINE_THREAD;

    if (m_masterParams == params) {
        return;
    }

    m_masterFxChain = audioFactory()->makeMasterFxChain(params.fxChain);
    m_masterFxChain->setOutputSpec(m_outputSpec);
    m_masterFxChain->setMode(m_mode);
    m_masterFxChain->setPlayheadPosition(m_playhead);

    m_masterFxChain->fxChainSpecChanged().onReceive(this, [this](const AudioFxChain& fxChainSpec) {
        m_masterParams.fxChain = fxChainSpec;
        m_masterOutputParamsChanged.send(m_masterParams);
    });

    m_masterParams = params;
    m_masterParams.fxChain = m_masterFxChain->fxChainSpec();
    m_masterOutputParamsChanged.send(m_masterParams);

    m_controlNode->setVolume(muse::db_to_linear(m_masterParams.volume));
    m_controlNode->setPan(m_masterParams.balance);
    m_controlNode->setMute(m_masterParams.muted);
}

void Mixer::clearMasterOutputParams()
{
    setMasterOutputParams(AudioOutputParams());
}

Channel<AudioOutputParams> Mixer::masterOutputParamsChanged() const
{
    return m_masterOutputParamsChanged;
}

AudioSignalChanges Mixer::masterAudioSignalChanges() const
{
    return m_signalNode->audioSignalChanges();
}

void Mixer::setIsIdle(bool idle)
{
    ONLY_AUDIO_ENGINE_THREAD;

    m_isIdle = idle;
}

void Mixer::setTracksToProcessWhenIdle(const std::unordered_set<TrackId>& trackIds)
{
    ONLY_AUDIO_ENGINE_THREAD;

    m_tracksToProcessWhenIdle = trackIds;
}

void Mixer::mixOutputFromChannel(float* outBuffer, const float* inBuffer, unsigned int samplesCount) const
{
    IF_ASSERT_FAILED(outBuffer && inBuffer) {
        return;
    }

    if (m_masterParams.muted) {
        return;
    }

    for (samples_t s = 0; s < samplesCount; ++s) {
        size_t samplePos = s * m_outputSpec.audioChannelCount;

        for (audioch_t audioChNum = 0; audioChNum < m_outputSpec.audioChannelCount; ++audioChNum) {
            size_t idx = samplePos + audioChNum;
            float sample = inBuffer[idx];
            outBuffer[idx] += sample;
        }
    }
}

void Mixer::prepareAuxBuffers(size_t outBufferSize)
{
    for (auto& aux : m_auxTracks) {
        aux.processed = false;
        if (!aux.chain->fxChain()) {
            continue;
        }
        aux.buffer.resize(outBufferSize);
        std::fill(aux.buffer.begin(), aux.buffer.begin() + outBufferSize, 0.f);
    }
}

void Mixer::writeTrackToAuxBuffers(const float* trackBuffer, size_t outBufferSize, const AuxSendsParams& auxSends)
{
    for (aux_channel_idx_t auxIdx = 0; auxIdx < auxSends.size(); ++auxIdx) {
        if (auxIdx >= m_auxTracks.size()) {
            break;
        }

        TrackData& aux = m_auxTracks.at(auxIdx);
        if (!aux.chain->fxChain()) {
            continue;
        }

        const AuxSendParams& auxSend = auxSends.at(auxIdx);
        if (!auxSend.active || muse::is_zero(auxSend.signalAmount)) {
            continue;
        }

        float* auxBuffer = aux.buffer.data();
        float signalAmount = auxSend.signalAmount;

        for (size_t i = 0; i < outBufferSize; ++i) {
            auxBuffer[i] += trackBuffer[i] * signalAmount;
        }

        aux.processed = true;
    }
}

void Mixer::processAuxChannels(float* buffer, samples_t samplesPerChannel)
{
    for (TrackData& aux : m_auxTracks) {
        if (!aux.processed) {
            continue;
        }

        float* auxBuffer = aux.buffer.data();
        aux.chain->process(auxBuffer, samplesPerChannel);

        if (auto signal = aux.chain->signal()) {
            if (!signal->isSilent()) {
                mixOutputFromChannel(buffer, auxBuffer, samplesPerChannel);
            }
        }
    }
}

void Mixer::processMasterFx(float* buffer, samples_t samplesPerChannel)
{
    if (m_masterFxChain) {
        m_masterFxChain->process(buffer, samplesPerChannel);
    }
}

void Mixer::notifyAboutAudioSignalChanges()
{
    for (const auto& t : m_tracks) {
        if (auto signal = t.chain->signal()) {
            signal->notifyAboutAudioSignalChanges();
        }
    }

    for (const auto& aux : m_auxTracks) {
        if (auto signal = aux.chain->signal()) {
            signal->notifyAboutAudioSignalChanges();
        }
    }

    m_signalNode->notifyAboutAudioSignalChanges();
}

void Mixer::notifyNoAudioSignal()
{
    m_signalNode->setNoAudioSignal();
    notifyAboutAudioSignalChanges();
}
