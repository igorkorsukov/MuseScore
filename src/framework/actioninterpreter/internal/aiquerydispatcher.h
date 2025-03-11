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
#pragma once

#include <unordered_map>

#include "../iaiquerydispatcher.h"

namespace muse::ai {
class AiQueryDispatcher : public IAiQueryDispatcher
{
public:
    AiQueryDispatcher() = default;

    void reg(AIQueryable* client, const AiQuery& q, const QueryCallBack& call) override;
    void unReg(AIQueryable* client) override;

    void dispatch(const AiQuery& q) override;

private:

    struct Client {
        AIQueryable* client = nullptr;
        QueryCallBack callback;
    };

    std::unordered_map<std::string, Client> m_clients;
};
}
