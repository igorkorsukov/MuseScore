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
#include "notationinputcontroller.h"

#include <QRectF>

#include "libmscore/score.h"
#include "libmscore/page.h"
#include "libmscore/element.h"

#include "scorehelpful.h"

using namespace mu::domain::notation;
using namespace Ms;

NotationInputController::NotationInputController(IGetScore* getScore)
    : m_getScore(getScore)
{

}

INotationInputController::HitElement NotationInputController::hitElement(const QPointF& pos, float width) const
{
    QList<Element*> ll = hitElements(pos, width);
    if (ll.isEmpty()) {
        return HitElement();
    }

    HitElement e;
    e.ptr = ll.at(0);
    return e;
}

Ms::Page* NotationInputController::point2page(const QPointF& p) const
{
    if (score()->layoutMode() == LayoutMode::LINE) {
        return score()->pages().isEmpty() ? 0 : score()->pages().front();
    }

    for (Page* page : score()->pages()) {
        if (page->bbox().translated(page->pos()).contains(p)) {
            return page;
        }
    }

    return nullptr;
}

QList<Element*> NotationInputController::hitElements(const QPointF &p_in, float w) const
{
    Page* page = point2page(p_in);
    if (!page) {
        return QList<Element*>();
    }

    QList<Element*> ll;

    QPointF p = p_in - page->pos();

    QRectF r(p.x() - w, p.y() - w, 3.0 * w, 3.0 * w);

    QList<Element*> el = page->items(r);
    //! TODO
    //    for (int i = 0; i < MAX_HEADERS; i++)
    //        if (score()->headerText(i) != nullptr)      // gives the ability to select the header
    //            el.push_back(score()->headerText(i));
    //    for (int i = 0; i < MAX_FOOTERS; i++)
    //        if (score()->footerText(i) != nullptr)      // gives the ability to select the footer
    //            el.push_back(score()->footerText(i));
    //! -------

    for (Element* e : el) {
        e->itemDiscovered = 0;
        if (!e->selectable() || e->isPage())
            continue;
        if (e->contains(p))
            ll.append(e);
    }

    int n = ll.size();
    if ((n == 0) || ((n == 1) && (ll[0]->isMeasure()))) {
        //
        // if no relevant element hit, look nearby
        //
        for (Element* e : el) {
            if (e->isPage() || !e->selectable())
                continue;
            if (e->intersects(r))
                ll.append(e);
        }
    }

    if (!ll.empty()) {
        std::sort(ll.begin(), ll.end(), ScoreHelpful::elementLower);
    }

    return ll;
}

Ms::MasterScore* NotationInputController::score() const
{
    return m_getScore->masterScore();
}
