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
#ifndef MU_NOTATION_NOTATIONPAINTVIEW_H
#define MU_NOTATION_NOTATIONPAINTVIEW_H

#include <QObject>
#include <QQuickPaintedItem>

#include "modularity/ioc.h"
#include "interfaces/iinteractive.h"
#include "domain/notation/interfaces/inotationcreator.h"

namespace mu::notation {

class NotationPaintView : public QQuickPaintedItem
{
    Q_OBJECT

    IOCINJECT("notation", framework::IInteractive, interactive)
    IOCINJECT("notation", INotationCreator, notationCreator)

public:
    NotationPaintView();

    Q_INVOKABLE void open();




private:
    void paint(QPainter *painter) override;
    //void componentComplete() override;

    std::shared_ptr<INotation> _notation;

};

}

#endif // MU_NOTATION_NOTATIONPAINTVIEW_H
