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
#ifndef MU_NOTATIONSCENE_NOTATIONVIEWINPUTCONTROLLER_H
#define MU_NOTATIONSCENE_NOTATIONVIEWINPUTCONTROLLER_H

#include <QObject>
#include <QWheelEvent>

#include "domain/notation/inotationinputvontroller.h"

namespace mu::scene::notation {

class NotationViewInputController : public QObject
{
    Q_OBJECT

public:

    struct IView
    {
        virtual ~IView() = default;
        virtual int viewWidth() const = 0;
        virtual int viewHeight() const = 0;
        virtual QPoint toLogical(const QPoint&p ) const = 0;
        virtual float hitWidth() const = 0;
        virtual domain::notation::INotationInputController* notationInputController() const = 0;
    };

    NotationViewInputController(IView* view);

    void wheelEvent(QWheelEvent* ev);
    void mousePressEvent(QMouseEvent* ev);
    void mouseMoveEvent(QMouseEvent* ev);
    void mouseReleaseEvent(QMouseEvent* ev);

signals:
    void requiredMoveScene(int dx, int dy);
    void requiredZoomStep(qreal steps, const QPoint& logicPos);
    void requiredScrollHorizontal(int dx);
    void requiredScrollVertical(int dy);

private:

    struct InteractData {
        QPoint beginPoint;
        domain::notation::INotationInputController::HitElement element;
    };

    domain::notation::INotationInputController* notationInputController() const;

    IView* m_view = nullptr;
    InteractData m_interactData;
};

}

#endif // MU_NOTATIONSCENE_NOTATIONVIEWINPUTCONTROLLER_H
