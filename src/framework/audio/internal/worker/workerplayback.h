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
#pragma once

#include "iworkerplayback.h"

#include "global/async/asyncable.h"

#include "global/modularity/ioc.h"

#include "iplayer.h"
#include "itracks.h"
#include "iaudiooutput.h"
#include "igettracksequence.h"

namespace muse::audio::worker {
class WorkerPlayback : public IWorkerPlayback, public IGetTrackSequence, public Injectable, public async::Asyncable
{
public:
    WorkerPlayback(const muse::modularity::ContextPtr& iocCtx)
        : Injectable(iocCtx) {}

    void init();
    void deinit();

    // IWorkerPlayback
    TrackSequenceId addSequence() override;
    void removeSequence(const TrackSequenceId id) override;
    TrackSequenceIdList sequenceIdList() const override;

    IPlayerPtr player(const TrackSequenceId id) const override;
    ITracksPtr tracks() const override;
    IAudioOutputPtr audioOutput() const override;

    // IGetTrackSequence
    ITrackSequencePtr sequence(const TrackSequenceId id) const override;

private:

    ITracksPtr m_trackHandlersPtr = nullptr;
    IAudioOutputPtr m_audioOutputPtr = nullptr;

    std::map<TrackSequenceId, ITrackSequencePtr> m_sequences;
};
}
