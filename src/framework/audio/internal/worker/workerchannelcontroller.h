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

#include "global/modularity/ioc.h"
#include "../rpc/irpcchannel.h"

#include "iworkerplayback.h"

namespace muse::audio::worker {
class WorkerChannelController
{
    Inject<rpc::IRpcChannel> channel;

public:
    WorkerChannelController() = default;

    void initOnWorker(std::shared_ptr<IWorkerPlayback> playback);
    void deinitOnWorker();

private:

    std::shared_ptr<IWorkerPlayback> m_playback;
};
}
