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
#include "aiactioncontroller.h"

using namespace muse::ai;
using namespace muse::actions;

void AiActionController::init()
{
    dispatcher()->reg(this, ActionQuery("action://diagnostics/ai/voicecontrol"), [this](const ActionQuery&) {
        interactive()->open("muse://diagnostics/ai/voicecontrol?floating=true&modal=false");
    });

    dispatcher()->reg(this, ActionQuery("action://diagnostics/ai/cases"), [this](const ActionQuery&) {
        interactive()->open("muse://diagnostics/ai/cases?floating=true&modal=false");
    });
}
