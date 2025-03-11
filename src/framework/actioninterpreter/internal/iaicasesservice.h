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

#include <string>
#include <vector>

#include "modularity/imoduleinterface.h"

#include "global/async/channel.h"

#include "aitypes.h"

namespace muse::ai {
struct AiCase {
    std::string uuid;
    std::string text;
    std::vector<AiQuery> actions;
};

using AiCases = std::vector<AiCase>;

class IAiCasesService : MODULE_EXPORT_INTERFACE
{
    INTERFACE_ID(IAiCasesService)
public:

    virtual ~IAiCasesService() = default;

    virtual AiCases cases() const = 0;

    virtual void addNewCase() = 0;
    virtual muse::async::Channel<AiCase> caseAdded() const = 0;

    virtual void updateCase(const AiCase& c) = 0;
    virtual muse::async::Channel<AiCase> caseChanged() const = 0;

    virtual void runCase(const AiCase& c) = 0;
};
}
