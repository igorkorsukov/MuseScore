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
#include "projectaiquerycontroller.h"

using namespace muse;
using namespace muse::ai;
using namespace muse::actions;
using namespace mu::project;

void ProjectAIQueryController::init()
{
    using Controller = ProjectAIQueryController;

    reg("ai://open_project", &Controller::openProject);
}

void ProjectAIQueryController::reg(const std::string& q, void (ProjectAIQueryController::* handler)(const muse::ai::AIQuery& q))
{
    queryDispatcher()->reg(this, AIQuery(q), this, handler);
}

void ProjectAIQueryController::reg(const std::string& q, void (ProjectAIQueryController::* handler)())
{
    queryDispatcher()->reg(this, AIQuery(q), this, handler);
}

void ProjectAIQueryController::openProject(const muse::ai::AIQuery& q)
{
    const std::string name = q.param("name", Val("last")).toString();

    const RecentFilesList& recentScores = recentFilesController()->recentFilesList();

    if (recentScores.empty()) {
        LOGW() << "no recent files";
        return;
    }

    if (name == "last") {
        const RecentFile& last = recentScores.at(0);
        ActionData args =  ActionData::make_arg2<QUrl, QString>(QUrl::fromLocalFile(last.path.toQString()), last.displayNameOverride);
        actionsDispatcher()->dispatch("file-open", args);
    }
}
