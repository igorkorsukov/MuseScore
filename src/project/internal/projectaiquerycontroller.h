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

#include "actioninterpreter/aiqueryable.h"
#include "global/modularity/ioc.h"
#include "actioninterpreter/iaiquerydispatcher.h"
#include "actions/iactionsdispatcher.h"
#include "../irecentfilescontroller.h"

namespace mu::project {
class ProjectAIQueryController : public muse::ai::AIQueryable
{
    muse::Inject<muse::ai::IAIQueryDispatcher> queryDispatcher;
    muse::Inject<muse::actions::IActionsDispatcher> actionsDispatcher;
    muse::Inject<IRecentFilesController> recentFilesController;

public:
    ProjectAIQueryController() = default;

    void init();

private:

    void reg(const std::string& q, void (ProjectAIQueryController::*)(const muse::ai::AIQuery& q));
    void reg(const std::string& q, void (ProjectAIQueryController::*)());

    void openProject(const muse::ai::AIQuery& q);
};
}
