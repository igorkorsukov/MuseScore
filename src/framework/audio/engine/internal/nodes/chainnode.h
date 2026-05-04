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
    void clear();

protected:

    void doSelfProcess(float*, samples_t) override {}

    void doAdd(IAudioNodePtr node);
    virtual void rebuild();

    std::vector<IAudioNodePtr> m_nodes;
};

template<typename T>
void ChainNode<T>::rebuild()
{
    IAudioNodePtr next = this->shared_from_this();
    for (auto& node : m_nodes) {
        node->disconnectAll();
        node->connect(next);
        next = node;
    }
}

template<typename T>
void ChainNode<T>::add(IAudioNodePtr node)
{
    IF_ASSERT_FAILED(node) {
        return;
    }

    doAdd(node);
    rebuild();
}

template<typename T>
void ChainNode<T>::doAdd(IAudioNodePtr node)
{
    IF_ASSERT_FAILED(node) {
        return;
    }

    node->setOutputSpec(AudioNode<T>::outputSpec());
    node->setMode(AudioNode<T>::mode());
    m_nodes.push_back(node);
}

template<typename T>
void ChainNode<T>::remove(IAudioNodePtr node)
{
    IF_ASSERT_FAILED(node) {
        return;
    }

    bool removed = muse::remove(m_nodes, node);
    if (removed) {
        node->disconnectAll();
        rebuild();
    }
}

template<typename T>
void ChainNode<T>::clear()
{
    for (auto& node : m_nodes) {
        node->disconnectAll();
    }
    m_nodes.clear();
}
}
