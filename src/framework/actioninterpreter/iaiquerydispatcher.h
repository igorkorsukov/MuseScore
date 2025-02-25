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

#include "global/modularity/imoduleinterface.h"

#include "aitypes.h"

namespace muse::ai {
class AIQueryable;
class IAIQueryDispatcher : MODULE_EXPORT_INTERFACE
{
    INTERFACE_ID(IAIQueryDispatcher)
public:

    virtual ~IAIQueryDispatcher() = default;

    using QueryCallBack = std::function<void (const AIQuery&)>;

    virtual void reg(AIQueryable* client, const AIQuery& q, const QueryCallBack& call) = 0;
    virtual void unReg(AIQueryable* client) = 0;

    virtual void dispatch(const AIQuery& q) = 0;
};
}
