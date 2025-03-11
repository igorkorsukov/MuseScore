/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2025 MuseScore Limited
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
#include "aiquerydispatcher.h"

#include "global/containers.h"

#include "log.h"

using namespace muse::ai;

void AiQueryDispatcher::reg(AIQueryable* client, const AiQuery& q, const QueryCallBack& call)
{
    std::string qs = q.toString();
    auto it = m_clients.find(qs);
    IF_ASSERT_FAILED(it == m_clients.end()) {
        LOGE() << "callback is already registered, there can be only one, query:" << qs;
        return;
    }

    m_clients.insert({ qs, { client, call } });
}

void AiQueryDispatcher::unReg(AIQueryable* client)
{
    muse::remove_if(m_clients, [client](const auto& p) {
        return p.second.client == client;
    });
}

void AiQueryDispatcher::dispatch(const AiQuery& q)
{
    // try full
    auto it = m_clients.find(q.toString());
    if (it != m_clients.end()) {
        it->second.callback(q);
        return;
    }

    // try just uri
    it = m_clients.find(q.uri().toString());
    if (it != m_clients.end()) {
        it->second.callback(q);
        return;
    }

    LOGW() << "not found callback for query: " << q.toString();
}
