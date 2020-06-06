//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2020 MuseScore BVBA and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================
#include "notationactioncontroller.h"

using namespace mu::domain::notation;

NotationActionController::NotationActionController()
{
    dispatcher()->reg("note-input", [this](const actions::ActionName&) { toggleNoteInput(); });
    dispatcher()->reg("pad-note-8", [this](const actions::ActionName& name) { padNote(name); });
    dispatcher()->reg("pad-note-4", [this](const actions::ActionName& name) { padNote(name); });
    dispatcher()->reg("pad-note-16", [this](const actions::ActionName& name) { padNote(name); });
}

std::shared_ptr<INotation> NotationActionController::currentNotation() const
{
    return globalContext()->currentNotation();
}

void NotationActionController::toggleNoteInput()
{
    auto notation = currentNotation();
    if (!notation) {
        return;
    }

    if (notation->inputState()->noteEntryMode()) {
        notation->endNoteEntry();
    } else {
        notation->startNoteEntry();
        notation->padNote("pad-note-8");
    }
}

void NotationActionController::padNote(const actions::ActionName& name)
{
    auto notation = currentNotation();
    if (!notation) {
        return;
    }

    notation->padNote(name);
}
