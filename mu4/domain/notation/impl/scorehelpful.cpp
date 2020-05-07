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
#include "scorehelpful.h"

#include "libmscore/element.h"
#include "libmscore/note.h"
#include "libmscore/notedot.h"

using namespace mu::domain::notation;
using namespace Ms;

bool ScoreHelpful::elementLower(const Element* e1, const Element* e2)
{
    if (!e1->selectable())
        return false;
    if (!e2->selectable())
        return true;
    if (e1->isNote() && e2->isStem())
        return true;
    if (e2->isNote() && e1->isStem())
        return false;
    if (e1->z() == e2->z()) {
        // same stacking order, prefer non-hidden elements
        if (e1->type() == e2->type()) {
            if (e1->type() == ElementType::NOTEDOT) {
                const NoteDot* n1 = static_cast<const NoteDot*>(e1);
                const NoteDot* n2 = static_cast<const NoteDot*>(e2);
                if (n1->note() && n1->note()->hidden())
                    return false;
                else if (n2->note() && n2->note()->hidden())
                    return true;
            }
            else if (e1->type() == ElementType::NOTE) {
                const Note* n1 = static_cast<const Note*>(e1);
                const Note* n2 = static_cast<const Note*>(e2);
                if (n1->hidden())
                    return false;
                else if (n2->hidden())
                    return true;
            }
        }
        // different types, or same type but nothing hidden - use track
        return e1->track() <= e2->track();
    }

    // default case, use stacking order
    return e1->z() <= e2->z();
}
