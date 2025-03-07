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
#include "notationaiquerycontroller.h"

#include "log.h"

using namespace muse::ai;
using namespace mu::notation;

void NotationAIQueryController::init()
{
    using Controller = NotationAIQueryController;

    reg("ai://move_cursor", &Controller::moveCursor);
    reg("ai://change_pitch", &Controller::changePitch);
    reg("ai://add_note", &Controller::addNote);
}

void NotationAIQueryController::reg(const std::string& q, void (NotationAIQueryController::* handler)(const muse::ai::AIQuery& q))
{
    queryDispatcher()->reg(this, AIQuery(q), this, handler);
}

void NotationAIQueryController::reg(const std::string& q, void (NotationAIQueryController::* handler)())
{
    queryDispatcher()->reg(this, AIQuery(q), this, handler);
}

void NotationAIQueryController::moveCursor(const muse::ai::AIQuery& q)
{
    const static std::map<std::string, std::vector<std::string> > map = {
        { "right", { "next-element" } },
        { "left", { "prev-element" } },

        { "first_element", { "first-element" } },
        { "next_element", { "next-element" } },
        { "prev_element", { "prev-element" } },

        { "first_measure", { "first-element", "notation-move-right" } },
        { "next_measure", { "notation-move-right-quickly" } },
        { "prev_measure", { "notation-move-left-quickly" } },
    };

    std::string direction = q.param("direction").toString();
    auto it = map.find(direction);
    if (it == map.end()) {
        LOGW() << "unknown direction: " << q.toString();
        return;
    }

    const std::vector<std::string>& acts = it->second;
    for (const std::string& act : acts) {
        actionsDispatcher()->dispatch(act);
    }
}

void NotationAIQueryController::changePitch(const muse::ai::AIQuery& q)
{
    const static std::map<std::string, std::string> map = {
        { "up", "pitch-up" },
        { "down", "pitch-down" },
    };

    std::string direction = q.param("direction").toString();
    auto it = map.find(direction);
    if (it == map.end()) {
        LOGW() << "unknown direction: " << q.toString();
        return;
    }

    actionsDispatcher()->dispatch(it->second);
}

void NotationAIQueryController::addNote(const muse::ai::AIQuery& q)
{
    LOGDA() << q.toString();

    std::string note = q.param("note").toString();

    actionsDispatcher()->dispatch("note-input-by-note-name");
    actionsDispatcher()->dispatch("note-" + note);
}
