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

#include <queue>
#include <mutex>

#include "../../irpcchannel.h"

namespace muse::audio::rpc {
class GeneralRpcChannel : public IRpcChannel
{
public:
    GeneralRpcChannel() = default;

    void setupWorkerThread();
    void process();

    // IRpcAudioChannel
    void send(CallId method, const Args& args) override;
    void listen(Handler h) override;

private:

    struct Msg {
        CallId method;
        Args args;
    };

    using MQ = std::queue<Msg>;

    struct RpcData {
        std::mutex mutex;
        MQ queue;
        Handler listener;
    };

    bool isWorkerThread() const;
    void doProcessRPC(RpcData& from, RpcData& to) const;

    std::thread::id m_workerThreadID;
    RpcData m_workerTh;
    RpcData m_mainTh;
};
}
