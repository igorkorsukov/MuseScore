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

#include "fxnode.h"

using namespace muse;
using namespace muse::audio;
using namespace muse::audio::engine;

FxNode::FxNode(IFxProcessorPtr fxProcessor, PlayheadPositionPtr playheadPosition)
    : m_fxProcessor(fxProcessor), m_playheadPosition(playheadPosition)
{
    assert(m_fxProcessor && "FxNode requires a non-null IFxProcessor");
}

void FxNode::setPlayheadPosition(PlayheadPositionPtr playheadPosition)
{
    m_playheadPosition = playheadPosition;
}

void FxNode::onModeChanged(const ProcessMode mode)
{
    m_fxProcessor->setMode(mode);
}

const AudioFxParams& FxNode::params() const
{
    return m_fxProcessor->params();
}

async::Channel<audio::AudioFxParams> FxNode::paramsChanged() const
{
    return m_fxProcessor->paramsChanged();
}

bool FxNode::shouldProcessDuringSilence() const
{
    return m_fxProcessor->shouldProcessDuringSilence();
}

void FxNode::onOutputSpecChanged(const OutputSpec& spec)
{
    m_fxProcessor->setOutputSpec(spec);
}

void FxNode::doSelfProcess(float* buffer, samples_t samplesPerChannel)
{
    samples_t pos = m_playheadPosition ? m_playheadPosition->currentPosition().samples() : 0;
    m_fxProcessor->process(buffer, samplesPerChannel, pos);
}
