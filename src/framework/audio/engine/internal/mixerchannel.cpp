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

#include "dsp/audiomathutils.h"

using namespace muse;
using namespace muse::async;
using namespace muse::audio;
using namespace muse::audio::engine;

MixerChannel::MixerChannel(const TrackId trackId, const OutputSpec& outputSpec, AudioNodePtr source,
                           PlayheadPositionPtr playheadPosition)
    : m_trackId(trackId),
    m_outputSpec(outputSpec),
    m_audioSource(std::move(source)),
    m_playheadPosition(playheadPosition)
{
    ONLY_AUDIO_ENGINE_THREAD;
}

MixerChannel::MixerChannel(const TrackId trackId, const OutputSpec& outputSpec,
                           PlayheadPositionPtr playheadPosition)
    : MixerChannel(trackId, outputSpec, nullptr, playheadPosition)
{
    ONLY_AUDIO_ENGINE_THREAD;
}

void MixerChannel::setPlayheadPosition(PlayheadPositionPtr playheadPosition)
{
    ONLY_AUDIO_ENGINE_THREAD;
    m_playheadPosition = playheadPosition;
}

TrackId MixerChannel::trackId() const
{
    return m_trackId;
}

AudioNodePtr MixerChannel::source() const
{
    return m_audioSource;
}

bool MixerChannel::muted() const
{
    return m_params.muted;
}

async::Notification MixerChannel::mutedChanged() const
{
    return m_mutedChanged;
}

const AudioOutputParams& MixerChannel::outputParams() const
{
    return m_params;
}

void MixerChannel::applyOutputParams(const AudioOutputParams& requiredParams)
{
    ONLY_AUDIO_ENGINE_THREAD;

    if (m_params == requiredParams) {
        return;
    }

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

    m_params = resultParams;
    m_paramsChanges.send(std::move(resultParams));

    if (mutedChanged) {
        m_mutedChanged.notify();
    }

    updateShouldProcessDuringSilence();
}

async::Channel<AudioOutputParams> MixerChannel::outputParamsChanged() const
{
    return m_paramsChanges;
}

AudioSignalChanges MixerChannel::audioSignalChanges() const
{
    return m_audioSignalNotifier.audioSignalChanges;
}

ProcessMode MixerChannel::mode() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_mode;
}

void MixerChannel::setMode(const ProcessMode mode)
{
    ONLY_AUDIO_ENGINE_THREAD;

    m_mode = mode;

    if (m_audioSource) {
        m_audioSource->setMode(mode);
    }

    for (FxNodePtr& fx : m_fxNodes) {
        fx->setMode(mode);
    }
}

void MixerChannel::setOutputSpec(const OutputSpec& spec)
{
    ONLY_AUDIO_ENGINE_THREAD;

    m_outputSpec = spec;

    if (m_audioSource) {
        m_audioSource->setOutputSpec(spec);
    }

    for (FxNodePtr& fx : m_fxNodes) {
        fx->setOutputSpec(spec);
    }
}

unsigned int MixerChannel::audioChannelsCount() const
{
    ONLY_AUDIO_ENGINE_THREAD;

    return m_outputSpec.audioChannelCount;
}

async::Channel<unsigned int> MixerChannel::audioChannelsCountChanged() const
{
    ONLY_AUDIO_ENGINE_THREAD;

    static async::Channel<unsigned int> channel;
    return channel;
}

samples_t MixerChannel::process(float* buffer, samples_t samplesPerChannel)
{
    ONLY_AUDIO_ENGINE_THREAD;

    samples_t processedSamplesCount = samplesPerChannel;

    if (m_audioSource) {
        if (!m_params.muted || !m_isSilent) {
            m_audioSource->process(buffer, samplesPerChannel);
        }
    }

    if (processedSamplesCount == 0 || (m_params.muted && m_isSilent)) {
        std::fill(buffer, buffer + samplesPerChannel * audioChannelsCount(), 0.f);
        setNoAudioSignal();

        return processedSamplesCount;
    }

    for (FxNodePtr& fx : m_fxNodes) {
        fx->process(buffer, samplesPerChannel);
    }

    completeOutput(buffer, samplesPerChannel);

    return processedSamplesCount;
}

void MixerChannel::completeOutput(float* buffer, unsigned int samplesCount)
{
    const unsigned int channelsCount = audioChannelsCount();
    const float volume = muse::db_to_linear(m_params.volume);
    float globalPeak = 0.f;

    for (audioch_t audioChNum = 0; audioChNum < channelsCount; ++audioChNum) {
        const gain_t totalGain = dsp::balanceGain(m_params.balance, audioChNum) * volume;
        float peak = 0.f;

        for (unsigned int s = 0; s < samplesCount; ++s) {
            const unsigned int idx = s * channelsCount + audioChNum;
            const float resultSample = buffer[idx] * totalGain;
            const float absSample = std::fabs(resultSample);

            buffer[idx] = resultSample;

            if (absSample > peak) {
                peak = absSample;
            }
        }

        m_audioSignalNotifier.updateSignalValue(audioChNum, peak);

        if (peak > globalPeak) {
            globalPeak = peak;
        }
    }

    m_isSilent = RealIsNull(globalPeak);
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
    return m_isSilent;
}

bool MixerChannel::shouldProcessDuringSilence() const
{
    return m_shouldProcessDuringSilence;
}

async::Channel<bool> MixerChannel::shouldProcessDuringSilenceChanged() const
{
    return m_shouldProcessDuringSilenceChanged;
}

AudioSignalsNotifier& MixerChannel::signalNotifier() const
{
    return m_audioSignalNotifier;
}

void MixerChannel::setNoAudioSignal()
{
    unsigned int channelsCount = audioChannelsCount();

    for (audioch_t audioChNum = 0; audioChNum < channelsCount; ++audioChNum) {
        m_audioSignalNotifier.updateSignalValue(audioChNum, 0.f);
    }
}
