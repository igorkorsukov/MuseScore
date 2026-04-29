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
#include "mixernode.h"

using namespace muse::audio::engine;

void MixerNode::onOutputSpecChanged(const OutputSpec& spec)
{
    const size_t bufferSize = m_outputSpec.samplesPerChannel * m_outputSpec.audioChannelCount;
    m_buffer.resize(bufferSize);

    for (auto& input : m_inputs) {
        input->setOutputSpec(spec);
    }
}

void MixerNode::onModeChanged(const ProcessMode mode)
{
    for (auto& input : m_inputs) {
        input->setMode(mode);
    }
}

void MixerNode::doAddNode(std::shared_ptr<AudioNode> other)
{
    m_inputs.emplace_back(other);
}

void MixerNode::doRemoveNode(std::shared_ptr<AudioNode> other)
{
    muse::remove(m_inputs, other);
}

void MixerNode::doProcess(float* buffer, samples_t samplesPerChannel)
{
    const audioch_t audioChannelCount = m_outputSpec.audioChannelCount;
    const size_t outBufferSize = samplesPerChannel * audioChannelCount;

    for (auto& input : m_inputs) {
        // prepare input buffer
        if (m_buffer.size() < outBufferSize) {
            m_buffer.resize(outBufferSize);
        }

        // clear previous data
        std::fill(m_buffer.begin(), m_buffer.begin() + outBufferSize, 0.0f);

        // process input
        input->process(m_buffer.data(), samplesPerChannel);

        // mix input into output
        for (samples_t s = 0; s < samplesPerChannel; ++s) {
            size_t samplePos = s * audioChannelCount;
            for (audioch_t audioChNum = 0; audioChNum < audioChannelCount; ++audioChNum) {
                size_t idx = samplePos + audioChNum;
                buffer[idx] += m_buffer[idx];
            }
        }
    }
}

void MixerNode::doSelfProcess(float*, samples_t)
{
    // noop
}
