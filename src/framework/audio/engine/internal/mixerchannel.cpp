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
#include "mixerchannel.h"

#include <algorithm>

#include "audio/common/audiosanitizer.h"

using namespace muse;
using namespace muse::async;
using namespace muse::audio;
using namespace muse::audio::engine;

MixerChannel::MixerChannel(const TrackId trackId, AudioSourceNodePtr source, PlayheadPositionPtr playheadPosition)
    : m_trackId(trackId),
    m_playheadPosition(playheadPosition),
    m_sourceNode(source)
{
    ONLY_AUDIO_ENGINE_THREAD;
}

MixerChannel::MixerChannel(const TrackId trackId,  PlayheadPositionPtr playheadPosition)
    : MixerChannel(trackId, nullptr, playheadPosition)
{
    ONLY_AUDIO_ENGINE_THREAD;
}

void MixerChannel::init()
{
    ONLY_AUDIO_ENGINE_THREAD;

    // Make the chain: mixer <- signalnode <- controlnode <- mixerchannel(fxnodes <- audio source)
    m_signalNode = std::make_shared<SignalNode>();
    m_controlNode = std::make_shared<ControlNode>();

    m_controlNode->connect(m_signalNode);

    this->connect(m_controlNode);
}

void MixerChannel::setPlayheadPosition(PlayheadPositionPtr playheadPosition)
{
    ONLY_AUDIO_ENGINE_THREAD;
    m_playheadPosition = playheadPosition;

    for (FxNodePtr& fx : m_fxNodes) {
        fx->setPlayheadPosition(m_playheadPosition);
    }
}

TrackId MixerChannel::trackId() const
{
    return m_trackId;
}

AudioNodePtr MixerChannel::source() const
{
    return m_sourceNode;
}

bool MixerChannel::muted() const
{
    return m_params.muted;
}

async::Notification MixerChannel::mutedChanged() const
{
    return m_mutedChanged;
}

AudioOutputParams MixerChannel::onOutputParamsChanged(const AudioOutputParams& requiredParams)
{
    ONLY_AUDIO_ENGINE_THREAD;

    m_fxNodes.clear();
    m_fxNodes = audioFactory()->makeTrackFxList(m_trackId, requiredParams.fxChain);

    for (FxNodePtr& fx : m_fxNodes) {
        fx->setOutputSpec(m_outputSpec);
        fx->setMode(mode());
        fx->setPlayheadPosition(m_playheadPosition);

        fx->paramsChanged().onReceive(this, [this](const AudioFxParams& fxParams) {
            m_params.fxChain.insert_or_assign(fxParams.chainOrder, fxParams);
            m_paramsChanges.send(m_params);
            updateShouldProcessDuringSilence();
        }, async::Asyncable::Mode::SetReplace);
    }

    AudioOutputParams resultParams = requiredParams;

    auto findFxNode = [this](const std::pair<AudioFxChainOrder, AudioFxParams>& params) -> FxNodePtr {
        for (FxNodePtr& fx : m_fxNodes) {
            if (fx->params().chainOrder != params.first) {
                continue;
            }

            if (fx->params().resourceMeta == params.second.resourceMeta) {
                return fx;
            }
        }

        return nullptr;
    };

    for (auto it = resultParams.fxChain.begin(); it != resultParams.fxChain.end();) {
        if (FxNodePtr fx = findFxNode(*it)) {
            fx->setBypassed(!it->second.active);
            ++it;
        } else {
            it = resultParams.fxChain.erase(it);
        }
    }

    bool mutedChanged = m_params.muted != resultParams.muted;
    if (mutedChanged) {
        m_params = resultParams; //! NOTE Temporary hack
        m_mutedChanged.notify();
    }

    if (mutedChanged && !resultParams.muted && m_sourceNode && m_playheadPosition) {
        m_sourceNode->seek(m_playheadPosition->currentPosition());
    }

    updateShouldProcessDuringSilence();

    m_controlNode->setVolume(muse::db_to_linear(resultParams.volume));
    m_controlNode->setPan(resultParams.balance);
    m_controlNode->setMute(resultParams.muted);

    return resultParams;
}

AudioSignalChanges MixerChannel::audioSignalChanges() const
{
    return m_signalNode->audioSignalChanges();
}

void MixerChannel::setNoAudioSignal()
{
    m_signalNode->setNoAudioSignal();
}

void MixerChannel::notifyAboutAudioSignalChanges()
{
    m_signalNode->notifyAboutAudioSignalChanges();
}

void MixerChannel::onModeChanged(const ProcessMode mode)
{
    ONLY_AUDIO_ENGINE_THREAD;

    if (m_sourceNode) {
        m_sourceNode->setMode(mode);
    }

    for (FxNodePtr& fx : m_fxNodes) {
        fx->setMode(mode);
    }
}

void MixerChannel::onOutputSpecChanged(const OutputSpec& spec)
{
    ONLY_AUDIO_ENGINE_THREAD;

    m_signalNode->setOutputSpec(spec);
    m_controlNode->setOutputSpec(spec);

    if (m_sourceNode) {
        m_sourceNode->setOutputSpec(spec);
    }

    for (FxNodePtr& fx : m_fxNodes) {
        fx->setOutputSpec(spec);
    }
}

void MixerChannel::doProcess(float* buffer, samples_t samplesPerChannel)
{
    ONLY_AUDIO_PROC_THREAD;

    //! NOTE Temporary hack
    // mixer -> mixerchannel (process->doProcess)
    // -> m_signalNode
    // -> m_controlNode
    // -> mixerchannel (process->doProcess->doSelfProcess)

    if (!m_chainProcessing) {
        m_chainProcessing = true;
        m_signalNode->process(buffer, samplesPerChannel);
        m_chainProcessing = false;
    } else {
        doSelfProcess(buffer, samplesPerChannel);
    }
}

void MixerChannel::doSelfProcess(float* buffer, samples_t samplesPerChannel)
{
    ONLY_AUDIO_PROC_THREAD;

    const unsigned int audioChannelCount = m_outputSpec.audioChannelCount;

    if (m_sourceNode) {
        if (!m_params.muted || !m_signalNode->isSilent()) {
            m_sourceNode->process(buffer, samplesPerChannel);
        }
    }

    if (m_params.muted && m_signalNode->isSilent()) {
        std::fill(buffer, buffer + samplesPerChannel * audioChannelCount, 0.f);
        setNoAudioSignal();
        return;
    }

    for (FxNodePtr& fx : m_fxNodes) {
        fx->process(buffer, samplesPerChannel);
    }
}

void MixerChannel::updateShouldProcessDuringSilence()
{
    bool shouldProcessDuringSilence = false;
    for (const FxNodePtr& fx : m_fxNodes) {
        if (fx->shouldProcessDuringSilence()) {
            shouldProcessDuringSilence = true;
            break;
        }
    }

    if (shouldProcessDuringSilence != m_shouldProcessDuringSilence) {
        m_shouldProcessDuringSilence = shouldProcessDuringSilence;
        m_shouldProcessDuringSilenceChanged.send(shouldProcessDuringSilence);
    }
}

bool MixerChannel::isSilent() const
{
    return m_signalNode ? m_signalNode->isSilent() : true;
}

bool MixerChannel::shouldProcessDuringSilence() const
{
    return m_shouldProcessDuringSilence;
}

async::Channel<bool> MixerChannel::shouldProcessDuringSilenceChanged() const
{
    return m_shouldProcessDuringSilenceChanged;
}
