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

#include "chainnode.h"
#include "fxnode.h"
#include "async/asyncable.h"
#include "async/channel.h"

namespace muse::audio {
struct FxChainTag
{
    static constexpr const char* name = "FxChain";
};
}

namespace muse::audio::engine {
class FxChain : public ChainNode<FxChainTag>, public async::Asyncable
{
public:

    void add(FxNodePtr fx);
    void remove(FxNodePtr fx);

    //! NOTE Must be setted after adding all nodes to the chain
    void setFxChainSpec(const AudioFxChain& fxChainSpec);
    const AudioFxChain& fxChainSpec() const;
    async::Channel<AudioFxChain> fxChainSpecChanged() const;

    void setPlayheadPosition(PlayheadPositionPtr playheadPosition);

    bool shouldProcessDuringSilence() const;
    async::Channel<bool> shouldProcessDuringSilenceChanged() const;

private:

    void updateShouldProcessDuringSilence();

    AudioFxChain m_fxChainSpec;
    async::Channel<AudioFxChain> m_fxChainSpecChanged;

    bool m_shouldProcessDuringSilence = false;
    async::Channel<bool> m_shouldProcessDuringSilenceChanged;
};

using FxChainPtr = std::shared_ptr<FxChain>;
}
