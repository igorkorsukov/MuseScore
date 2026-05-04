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

 #include "trackchain.h"

using namespace muse;
using namespace muse::audio;
using namespace muse::audio::engine;

TrackChain::TrackChain(TrackId trackId)
    : m_trackId(trackId)
{
}

void TrackChain::build()
{
    IAudioNodePtr next = this->shared_from_this();
    if (m_signal) {
        m_signal->connect(next);
        next = m_signal;
    }
    if (m_control) {
        m_control->connect(next);
        next = m_control;
    }
    if (m_fxChain) {
        m_fxChain->connect(next);
        next = m_fxChain;
    }
    if (m_source) {
        m_source->connect(next);
        next = m_source;
    }
}

void TrackChain::setSource(AudioSourceNodePtr source)
{
    m_source = source;
}

AudioSourceNodePtr TrackChain::source() const
{
    return m_source;
}

void TrackChain::setFxChain(FxChainPtr fxChain)
{
    m_fxChain = fxChain;
}

FxChainPtr TrackChain::fxChain() const
{
    return m_fxChain;
}

void TrackChain::setControl(ControlNodePtr controlNode)
{
    m_control = controlNode;
}

ControlNodePtr TrackChain::control() const
{
    return m_control;
}

void TrackChain::setSignal(SignalNodePtr signalNode)
{
    m_signal = signalNode;
}

SignalNodePtr TrackChain::signal() const
{
    return m_signal;
}
