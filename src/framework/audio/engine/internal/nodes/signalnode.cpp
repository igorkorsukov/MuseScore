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

#include "signalnode.h"

#include "global/types/number.h"

#include "../audiosignalnotifier.h"

using namespace muse::audio;
using namespace muse::audio::engine;

SignalNode::SignalNode()
{
    m_audioSignalNotifier = std::make_shared<AudioSignalsNotifier>();
}

bool SignalNode::isSilent() const
{
    return m_isSilent;
}

void SignalNode::setNoAudioSignal()
{
    unsigned int channelsCount = m_outputSpec.audioChannelCount;

    for (audioch_t audioChNum = 0; audioChNum < channelsCount; ++audioChNum) {
        m_audioSignalNotifier->updateSignalValue(audioChNum, 0.f);
    }

    m_isSilent = true;
}

void SignalNode::notifyAboutAudioSignalChanges()
{
    m_audioSignalNotifier->notifyAboutChanges();
}

AudioSignalChanges SignalNode::audioSignalChanges() const
{
    return m_audioSignalNotifier->audioSignalChanges;
}

void SignalNode::doSelfProcess(float* buffer, samples_t samplesPerChannel)
{
    const unsigned int channelsCount = m_outputSpec.audioChannelCount;
    float globalPeak = 0.f;

    for (audioch_t audioChNum = 0; audioChNum < channelsCount; ++audioChNum) {
        float peak = 0.f;

        for (unsigned int s = 0; s < samplesPerChannel; ++s) {
            const unsigned int idx = s * channelsCount + audioChNum;
            const float absSample = std::fabs(buffer[idx]);

            if (absSample > peak) {
                peak = absSample;
            }
        }

        m_audioSignalNotifier->updateSignalValue(audioChNum, peak);

        if (peak > globalPeak) {
            globalPeak = peak;
        }
    }

    m_isSilent = muse::is_zero(globalPeak);
}
