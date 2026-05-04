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

#pragma once

#include "audionode.h"

#include "../../iplayhead.h"

namespace muse::audio {
struct PlayheadTag
{
    static constexpr const char* name = "Playhead";
};
}

namespace muse::audio::engine {
class PlayheadNode : public AudioNode<PlayheadTag>
{
public:
    explicit PlayheadNode(PlayheadPtr playhead);

protected:

    void doSelfProcess(float* buffer, samples_t samplesPerChannel) override;

    PlayheadPtr m_playhead;
};

using PlayheadNodePtr = std::shared_ptr<PlayheadNode>;
}
