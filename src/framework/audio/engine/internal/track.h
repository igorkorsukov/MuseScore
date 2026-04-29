/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2025 MuseScore Limited and others
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

#include "global/async/channel.h"
#include "global/async/notification.h"

#include "audio/common/audiotypes.h"
#include "audio/common/timeposition.h"

#include "nodes/audionode.h"
#include "../iaudiosource.h"

namespace muse::audio::engine {
enum TrackType {
    Undefined = -1,
    Event_track,
    Sound_track
};

class ITrackAudioOutput : public IAudioSource
{
public:
    virtual ~ITrackAudioOutput() = default;

    virtual const AudioOutputParams& outputParams() const = 0;
    virtual void applyOutputParams(const AudioOutputParams& requiredParams) = 0;
    virtual async::Channel<AudioOutputParams> outputParamsChanged() const = 0;

    virtual AudioSignalChanges audioSignalChanges() const = 0;
};

using ITrackAudioOutputPtr = std::shared_ptr<ITrackAudioOutput>;
}
