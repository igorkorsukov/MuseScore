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

using namespace muse::audio;
using namespace muse::audio::engine;

void AudioNode::setOutputSpec(const OutputSpec& spec)
{
    if (m_outputSpec == spec) {
        return;
    }
    m_outputSpec = spec;
    onOutputSpecChanged(spec);
}

const OutputSpec& AudioNode::outputSpec() const
{
    return m_outputSpec;
}

void AudioNode::onOutputSpecChanged(const OutputSpec& spec)
{
    if (m_input) {
        m_input->setOutputSpec(spec);
    }
}

void AudioNode::setMode(const ProcessMode mode)
{
    if (m_mode == mode) {
        return;
    }
    m_mode = mode;
    onModeChanged(mode);
}

ProcessMode AudioNode::mode() const
{
    return m_mode;
}

void AudioNode::onModeChanged(const ProcessMode mode)
{
    if (m_input) {
        m_input->setMode(mode);
    }
}

void AudioNode::setEnabled(bool enabled)
{
    if (m_enabled == enabled) {
        return;
    }
    m_enabled = enabled;
    onEnabledChanged(enabled);
}

bool AudioNode::enabled() const
{
    return m_enabled;
}

void AudioNode::onEnabledChanged(bool enabled)
{
    if (m_input) {
        m_input->setEnabled(enabled);
    }
}

void AudioNode::setBypassed(bool bypassed)
{
    if (m_bypassed == bypassed) {
        return;
    }
    m_bypassed = bypassed;
    onBypassedChanged(bypassed);
}

bool AudioNode::bypassed() const
{
    return m_bypassed;
}

void AudioNode::onBypassedChanged(bool /*bypassed*/)
{
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

    IF_ASSERT_FAILED(!m_input) {
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
    if (!m_enabled) {
        return;
    }

    doProcess(buffer, samplesPerChannel);
}

void AudioNode::doProcess(float* buffer, samples_t samplesPerChannel)
{
    if (m_input) {
        m_input->process(buffer, samplesPerChannel);
    }

    if (m_bypassed) {
        return;
    }

    doSelfProcess(buffer, samplesPerChannel);
}
