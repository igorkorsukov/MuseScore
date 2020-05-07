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
#ifndef MU_DOMAIN_NOTATIONINPUTCONTROLLER_H
#define MU_DOMAIN_NOTATIONINPUTCONTROLLER_H

#include "../inotationinputvontroller.h"

#include "igetscore.h"

namespace Ms {
class MScore;
class Element;
class Page;
}

namespace mu::domain::notation {

class NotationInputController : public INotationInputController
{
public:
    NotationInputController(IGetScore* getScore);

    // INotationInputController
    HitElement hitElement(const QPointF& pos, float width) const override;
    // ---

    Ms::Page* point2page(const QPointF &p) const;
    QList<Ms::Element*> hitElements(const QPointF& p, float width) const;

private:

    Ms::MasterScore* score() const;

    IGetScore* m_getScore = nullptr;

};

}

#endif // MU_DOMAIN_NOTATIONINPUTCONTROLLER_H
