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

    if (m_fxChain) {
        m_fxChain->setPlayheadPosition(m_playheadPosition);
    }
}

TrackId MixerChannel::trackId() const
{
    return m_trackId;
}

IAudioNodePtr MixerChannel::source() const
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

    m_fxChain = audioFactory()->makeTrackFxChain(m_trackId, requiredParams.fxChain);
    m_fxChain->setOutputSpec(m_outputSpec);
    m_fxChain->setMode(mode());
    m_fxChain->setPlayheadPosition(m_playheadPosition);

    m_fxChain->fxChainSpecChanged().onReceive(this, [this](const AudioFxChain& fxChainSpec) {
        m_params.fxChain = fxChainSpec;
        m_paramsChanges.send(m_params);
    });

    m_fxChain->shouldProcessDuringSilenceChanged().onReceive(this, [this](bool shouldProcessDuringSilence) {
        m_shouldProcessDuringSilenceChanged.send(shouldProcessDuringSilence);
    });

    bool mutedChanged = m_params.muted != requiredParams.muted;

    m_params = requiredParams;
    m_params.fxChain = m_fxChain->fxChainSpec();

    if (mutedChanged) {
        m_mutedChanged.notify();
    }

    if (mutedChanged && !requiredParams.muted && m_sourceNode && m_playheadPosition) {
        m_sourceNode->seek(m_playheadPosition->currentPosition());
    }

    m_controlNode->setVolume(muse::db_to_linear(m_params.volume));
    m_controlNode->setPan(m_params.balance);
    m_controlNode->setMute(m_params.muted);

    return m_params;
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

    if (m_fxChain) {
        m_fxChain->setMode(mode);
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

    if (m_fxChain) {
        m_fxChain->setOutputSpec(spec);
    }
}

void MixerChannel::process(float* buffer, samples_t samplesPerChannel)
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

    if (m_fxChain) {
        m_fxChain->process(buffer, samplesPerChannel);
    }
}

bool MixerChannel::isSilent() const
{
    return m_signalNode ? m_signalNode->isSilent() : true;
}

bool MixerChannel::shouldProcessDuringSilence() const
{
    return m_fxChain ? m_fxChain->shouldProcessDuringSilence() : false;
}

async::Channel<bool> MixerChannel::shouldProcessDuringSilenceChanged() const
{
    return m_shouldProcessDuringSilenceChanged;
}
