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

namespace muse::audio::engine {
class ControlNode : public AudioNode
{
public:

    void setVolume(float value);
    float volume() const;
    void setPan(float pan);
    float pan() const;
    void setMute(bool mute);
    bool mute() const;

protected:

    void updateChannelGains();

    void onOutputSpecChanged(const OutputSpec& spec) override;
    void doSelfProcess(float* buffer, samples_t samplesPerChannel) override;

    std::vector<float> m_channelGains;

    float m_volume = 1.0f;
    float m_pan = 0.0f;
    bool m_mute = false;
};

using ControlNodePtr = std::shared_ptr<ControlNode>;
}
