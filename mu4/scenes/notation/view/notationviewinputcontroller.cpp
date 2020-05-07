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
#include "notationviewinputcontroller.h"

#include "log.h"

using namespace mu::scene::notation;
using namespace mu::domain::notation;

static constexpr int PIXELSSTEPSFACTOR = 5;

NotationViewInputController::NotationViewInputController(IView *view)
    : m_view(view)
{

}

void NotationViewInputController::wheelEvent(QWheelEvent* ev)
{
    QPoint pixelsScrolled = ev->pixelDelta();
    QPoint stepsScrolled  = ev->angleDelta();

    int dx = 0;
    int dy = 0;
    qreal steps = 0.0;

    if (!pixelsScrolled.isNull()) {

        dx = pixelsScrolled.x();
        dy = pixelsScrolled.y();
        steps = static_cast<qreal>(dy) / static_cast<qreal>(PIXELSSTEPSFACTOR);

    } else if (!stepsScrolled.isNull()) {

        dx = (stepsScrolled.x() * qMax(2.0, m_view->viewWidth() / 10.0)) / 120;
        dy = (stepsScrolled.y() * qMax(2.0, m_view->viewHeight() / 10.0)) / 120;
        steps = static_cast<qreal>(stepsScrolled.y()) / 120.0;
    }

    // Windows touch pad pinches also execute this
    if (ev->modifiers() & Qt::ControlModifier) {

        emit requiredZoomStep(steps, m_view->toLogical(ev->pos()));

    } else if (ev->modifiers() & Qt::ShiftModifier && dx == 0) {

        dx = dy;
        emit requiredScrollHorizontal(dx);

    } else {
        emit requiredScrollVertical(dy);
    }
}

void NotationViewInputController::mousePressEvent(QMouseEvent* ev)
{
    QPoint logicPos = m_view->toLogical(ev->pos());

    m_interactData.beginPoint = logicPos;
    m_interactData.element = notationInputController()->hitElement(logicPos, m_view->hitWidth());
    LOGI() << "hitElement valid: " << m_interactData.element.isValid();
}

void NotationViewInputController::mouseMoveEvent(QMouseEvent* ev)
{
    QPoint pos = m_view->toLogical(ev->pos());
    QPoint d = pos - m_interactData.beginPoint;
    int dx = d.x();
    int dy = d.y();

    if (dx == 0 && dy == 0) {
        return;
    }

    if (!m_interactData.element.isValid()) {
        emit requiredMoveScene(dx, dy);
        return;
    }
}

void NotationViewInputController::mouseReleaseEvent(QMouseEvent* /*ev*/)
{

}

mu::domain::notation::INotationInputController* NotationViewInputController::notationInputController() const
{
    return m_view->notationInputController();
}
