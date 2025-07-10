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
#include "generalrpcchannel.h"

#include <thread>

using namespace muse::audio::rpc;

void GeneralRpcChannel::setupWorkerThread()
{
    m_workerThreadID = std::this_thread::get_id();
}

bool GeneralRpcChannel::isWorkerThread() const
{
    return std::this_thread::get_id() == m_workerThreadID;
}

void GeneralRpcChannel::process()
{
    if (isWorkerThread()) {
        doProcessRPC(m_mainTh, m_workerTh);
    } else {
        doProcessRPC(m_workerTh, m_mainTh);
    }
}

void GeneralRpcChannel::doProcessRPC(RpcData& from, RpcData& to) const
{
    MQ fromMQ;
    {
        std::scoped_lock<std::mutex> lock(from.mutex);
        fromMQ.swap(from.queue);
    }

    while (!fromMQ.empty()) {
        const Msg& m = fromMQ.front();

        if (to.listener) {
            to.listener(m.method, m.args);
        }

        fromMQ.pop();
    }
}

void GeneralRpcChannel::send(CallId method, const Args& args)
{
    if (isWorkerThread()) {
        std::scoped_lock<std::mutex> lock(m_workerTh.mutex);
        m_workerTh.queue.push(Msg { method, args });
    } else {
        std::scoped_lock<std::mutex> lock(m_mainTh.mutex);
        m_mainTh.queue.push(Msg { method, args });
    }
}

void GeneralRpcChannel::listen(Handler h)
{
    if (isWorkerThread()) {
        m_workerTh.listener = h;
    } else {
        m_mainTh.listener = h;
    }
}
