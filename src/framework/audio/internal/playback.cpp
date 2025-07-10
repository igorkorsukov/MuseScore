/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
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
#include "playback.h"

#include <utility>

#include "global/async/async.h"

#include "internal/audiothread.h"
#include "internal/audiosanitizer.h"

#include "internal/player.h"

#include "audioerrors.h"

#include "log.h"

using namespace muse;
using namespace muse::audio;
using namespace muse::async;

void Playback::init()
{
}

void Playback::deinit()
{
}

bool Playback::isInited() const
{
    return true;//m_trackHandlersPtr != nullptr;
}

Promise<TrackSequenceId> Playback::addSequence()
{
    return Promise<TrackSequenceId>([this](auto resolve, auto /*reject*/) {
        ONLY_AUDIO_WORKER_THREAD;
        TrackSequenceId newId = workerPlayback()->addSequence();
        m_sequenceAdded.send(newId);
        return resolve(newId);
    }, AudioThread::ID);
}

Promise<TrackSequenceIdList> Playback::sequenceIdList() const
{
    return Promise<TrackSequenceIdList>([this](auto resolve, auto /*reject*/) {
        ONLY_AUDIO_WORKER_THREAD;
        TrackSequenceIdList result = workerPlayback()->sequenceIdList();
        return resolve(std::move(result));
    }, AudioThread::ID);
}

void Playback::removeSequence(const TrackSequenceId id)
{
    Async::call(this, [this, id]() {
        ONLY_AUDIO_WORKER_THREAD;
        workerPlayback()->removeSequence(id);
        m_sequenceRemoved.send(id);
    }, AudioThread::ID);
}

Channel<TrackSequenceId> Playback::sequenceAdded() const
{
    ONLY_AUDIO_MAIN_OR_WORKER_THREAD;

    return m_sequenceAdded;
}

Channel<TrackSequenceId> Playback::sequenceRemoved() const
{
    ONLY_AUDIO_MAIN_OR_WORKER_THREAD;

    return m_sequenceRemoved;
}

IPlayerPtr Playback::player(const TrackSequenceId id) const
{
    return workerPlayback()->player(id);
}

ITracksPtr Playback::tracks() const
{
    ONLY_AUDIO_MAIN_OR_WORKER_THREAD;

    return workerPlayback()->tracks();
}

IAudioOutputPtr Playback::audioOutput() const
{
    return workerPlayback()->audioOutput();
}
