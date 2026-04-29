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
#include "audionode.h"

#include "log.h"

using namespace muse::audio::engine;

void AudioNode::setOutputSpec(const OutputSpec& spec)
{
    m_outputSpec = spec;
    onOutputSpecChanged(spec);
}

void AudioNode::onOutputSpecChanged(const OutputSpec& spec)
{
    if (m_input) {
        m_input->setOutputSpec(spec);
    }
}

AudioNode* AudioNode::connect(std::shared_ptr<AudioNode> other)
{
    IF_ASSERT_FAILED(other) {
        return this;
    }
    other->doAddNode(shared_from_this());
    return this;
}

AudioNode* AudioNode::disconnect(std::shared_ptr<AudioNode> other)
{
    IF_ASSERT_FAILED(other) {
        return this;
    }
    other->doRemoveNode(shared_from_this());
    return this;
}

void AudioNode::doAddNode(std::shared_ptr<AudioNode> other)
{
    IF_ASSERT_FAILED(other) {
        return;
    }

    IF_ASSERT_FAILED(m_input) {
        LOGE() << "already connected to another node";
        return;
    }

    m_input = other;
}

void AudioNode::doRemoveNode(std::shared_ptr<AudioNode> other)
{
    IF_ASSERT_FAILED(other) {
        return;
    }

    IF_ASSERT_FAILED(m_input == other) {
        LOGE() << "not connected to this node";
        return;
    }

    m_input = nullptr;
}

void AudioNode::process(float* buffer, samples_t samplesPerChannel)
{
    doProcess(buffer, samplesPerChannel);
}

void AudioNode::doProcess(float* buffer, samples_t samplesPerChannel)
{
    if (m_input) {
        m_input->process(buffer, samplesPerChannel);
    }

    doSelfProcess(buffer, samplesPerChannel);
}
