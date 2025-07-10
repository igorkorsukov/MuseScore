/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2025 MuseScore BVBA and others
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
#include "workerplayback.h"

#include "internal/audiosanitizer.h"

#include "trackshandler.h"
#include "audiooutputhandler.h"
#include "tracksequence.h"

#include "internal/player.h"

using namespace muse::audio;
using namespace muse::audio::worker;

void WorkerPlayback::init()
{
    ONLY_AUDIO_WORKER_THREAD;

    m_trackHandlersPtr = std::make_shared<TracksHandler>(this, iocContext());
    m_audioOutputPtr = std::make_shared<AudioOutputHandler>(this, iocContext());
}

void WorkerPlayback::deinit()
{
    ONLY_AUDIO_WORKER_THREAD;

    m_sequences.clear();

    m_trackHandlersPtr = nullptr;
    m_audioOutputPtr = nullptr;

    disconnectAll();
}

TrackSequenceId WorkerPlayback::addSequence()
{
    ONLY_AUDIO_WORKER_THREAD;

    TrackSequenceId newId = static_cast<TrackSequenceId>(m_sequences.size());

    m_sequences.emplace(newId, std::make_shared<TrackSequence>(newId, iocContext()));

    return newId;
}

void WorkerPlayback::removeSequence(const TrackSequenceId id)
{
    ONLY_AUDIO_WORKER_THREAD;

    auto it = m_sequences.find(id);
    if (it != m_sequences.end()) {
        m_sequences.erase(it);
    }
}

TrackSequenceIdList WorkerPlayback::sequenceIdList() const
{
    ONLY_AUDIO_WORKER_THREAD;

    TrackSequenceIdList result;
    result.reserve(m_sequences.size());

    for (const auto& pair : m_sequences) {
        result.push_back(pair.first);
    }

    return result;
}

ITrackSequencePtr WorkerPlayback::sequence(const TrackSequenceId id) const
{
    ONLY_AUDIO_WORKER_THREAD;

    auto search = m_sequences.find(id);

    if (search != m_sequences.end()) {
        return search->second;
    }

    return nullptr;
}

IPlayerPtr WorkerPlayback::player(const TrackSequenceId id) const
{
    std::shared_ptr<Player> p = std::make_shared<Player>(this, id);
    p->init();
    return p;
}

ITracksPtr WorkerPlayback::tracks() const
{
    ONLY_AUDIO_MAIN_OR_WORKER_THREAD;

    return m_trackHandlersPtr;
}

IAudioOutputPtr WorkerPlayback::audioOutput() const
{
    ONLY_AUDIO_MAIN_OR_WORKER_THREAD;

    return m_audioOutputPtr;
}
