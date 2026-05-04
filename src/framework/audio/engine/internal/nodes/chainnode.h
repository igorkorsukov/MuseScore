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

#include <vector>

#include "global/containers.h"

namespace muse::audio {
//! NOTE Just combines several nodes into one entity
template<typename T>
class ChainNode : public AudioNode<T>
{
public:

    void add(IAudioNodePtr node);
    void remove(IAudioNodePtr node);

protected:

    void doSelfProcess(float*, samples_t) override
    {
        // noop
    }

    std::vector<IAudioNodePtr> m_nodes;
};

template<typename T>
void ChainNode<T>::add(IAudioNodePtr node)
{
    IF_ASSERT_FAILED(node) {
        return;
    }

    IAudioNodePtr prev = m_nodes.empty() ? this->shared_from_this() : m_nodes.front();

    m_nodes.push_back(node);

    node->connect(prev);
}

template<typename T>
void ChainNode<T>::remove(IAudioNodePtr node)
{
    IF_ASSERT_FAILED(node) {
        return;
    }

    size_t index = muse::indexOf(m_nodes, node);
    if (index == muse::nidx) {
        return;
    }

    IAudioNodePtr prev = index == 0 ? this->shared_from_this() : m_nodes[index - 1];
    IAudioNodePtr next = index + 1 < m_nodes.size() ? m_nodes[index + 1] : nullptr;

    node->disconnect(prev);

    if (next) {
        node->connect(next);
    }

    m_nodes.erase(m_nodes.begin() + index);
}
}
