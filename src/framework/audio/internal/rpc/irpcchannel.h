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

#include <vector>
#include <functional>

#include "global/modularity/imoduleinterface.h"
#include "global/types/val.h"

namespace muse::audio::rpc {
enum class CallId {
    Undefined = 0,

    Play
};

using Arg = muse::Val;
using Args = std::vector<Arg>;

using Handler = std::function<void (CallId method, const Args& args)>;

class IRpcChannel : MODULE_EXPORT_INTERFACE
{
    INTERFACE_ID(IRpcChannel)
public:
    virtual ~IRpcChannel() = default;

    virtual void send(CallId method, const Args& args) = 0;
    virtual void listen(Handler h) = 0;
};
}
