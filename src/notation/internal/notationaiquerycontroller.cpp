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

    reg("ai://notation/add_note", &Controller::addNote);
}

void NotationAIQueryController::reg(const std::string& q, void (NotationAIQueryController::* handler)(const muse::ai::AIQuery& q))
{
    dispatcher()->reg(this, AIQuery(q), this, handler);
}

void NotationAIQueryController::addNote(const muse::ai::AIQuery& q)
{
    LOGDA() << q.toString();
}
