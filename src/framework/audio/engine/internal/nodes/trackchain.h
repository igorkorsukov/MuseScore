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

 #include "audiosourcenode.h"
 #include "fxchain.h"
 #include "controlnode.h"
 #include "signalnode.h"

namespace muse::audio {
struct TrackChainTag
{
    static constexpr const char* name = "TrackChain";
};
}

namespace muse::audio::engine {
//! NOTE A typical node chain for processing a track:
//! signalnode <- controlnode <- fxnode[] <- audiosource
class TrackChain : public AudioNode<TrackChainTag>
{
public:

    TrackChain(TrackId trackId);

    void setSource(AudioSourceNodePtr source);
    AudioSourceNodePtr source() const;

    void setFxChain(FxChainPtr fxChain);
    FxChainPtr fxChain() const;

    void setControl(ControlNodePtr controlNode);
    ControlNodePtr control() const;

    void setSignal(SignalNodePtr signalNode);
    SignalNodePtr signal() const;

    void build();

protected:

    TrackId m_trackId;
    AudioSourceNodePtr m_source;
    FxChainPtr m_fxChain;
    ControlNodePtr m_control;
    SignalNodePtr m_signal;
};

using TrackChainPtr = std::shared_ptr<TrackChain>;
}
