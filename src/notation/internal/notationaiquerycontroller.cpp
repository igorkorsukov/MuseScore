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
    reg("ai://next_element", &Controller::nextElement);
    reg("ai://prev_element", &Controller::prevElement);


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
    std::string direction = q.param("direction").toString();
    if (direction == "right") {
        nextElement();
    } else if (direction == "left") {
        prevElement();
    } else {
        LOGW() << "unknown: " << q.toString();
    }
}

void NotationAIQueryController::nextElement()
{
    actionsDispatcher()->dispatch("next-element");
}

void NotationAIQueryController::prevElement()
{
    actionsDispatcher()->dispatch("prev-element");
}

void NotationAIQueryController::addNote(const muse::ai::AIQuery& q)
{
    LOGDA() << q.toString();

    std::string note = q.param("note").toString();

    actionsDispatcher()->dispatch("note-input-by-note-name");
    actionsDispatcher()->dispatch("note-" + note);
}
