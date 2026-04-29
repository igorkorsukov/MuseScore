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

#include <memory>

#include "audio/common/audiotypes.h"

namespace muse::audio::engine {
//! NOTE Abstract Base Audio Node
class AudioNode : public std::enable_shared_from_this<AudioNode>
{
public:
    virtual ~AudioNode() = default;

    void setOutputSpec(const OutputSpec& spec);

    AudioNode* connect(std::shared_ptr<AudioNode> other);
    AudioNode* disconnect(std::shared_ptr<AudioNode> other);

    void process(float* buffer, samples_t samplesPerChannel);

protected:

    virtual void onOutputSpecChanged(const OutputSpec& spec);
    virtual void doAddNode(std::shared_ptr<AudioNode> other);
    virtual void doRemoveNode(std::shared_ptr<AudioNode> other);
    virtual void doProcess(float* buffer, samples_t samplesPerChannel);
    virtual void doSelfProcess(float* buffer, samples_t samplesPerChannel) = 0;

    OutputSpec m_outputSpec;
    std::shared_ptr<AudioNode> m_input = nullptr;
};
}
