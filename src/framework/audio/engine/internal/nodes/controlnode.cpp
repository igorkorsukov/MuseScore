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

#include "controlnode.h"

#include "../dsp/audiomathutils.h"

using namespace muse;
using namespace muse::audio;
using namespace muse::audio::engine;

void ControlNode::setVolume(float value)
{
    m_volume = value;
}

float ControlNode::volume() const
{
    return m_volume;
}

void ControlNode::setPan(float pan)
{
    m_pan = pan;
}

float ControlNode::pan() const
{
    return m_pan;
}

void ControlNode::setMute(bool mute)
{
    setEnabled(!mute);
}

bool ControlNode::mute() const
{
    return !enabled();
}

void ControlNode::doSelfProcess(float* buffer, samples_t samplesPerChannel)
{
    const audioch_t channelsCount = m_outputSpec.audioChannelCount;

    for (audioch_t audioChNum = 0; audioChNum < channelsCount; ++audioChNum) {
        const gain_t totalChGain = dsp::balanceGain(m_pan, audioChNum) * m_volume;
        for (unsigned int s = 0; s < samplesPerChannel; ++s) {
            const unsigned int idx = s * channelsCount + audioChNum;
            buffer[idx] = buffer[idx] * totalChGain;
        }
    }
}
