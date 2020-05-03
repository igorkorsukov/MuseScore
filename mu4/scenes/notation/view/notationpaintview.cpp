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
#include "notationpaintview.h"

#include <QPainter>

#include "log.h"

using namespace mu::notation;

NotationPaintView::NotationPaintView()
{

}

void NotationPaintView::open()
{
    QString filePath = interactive()->selectOpeningFile("Score", "", "");
    LOGI() << "filePath: " << filePath;

    _notation = notationCreator()->newNotation();
    IF_ASSERT_FAILED(_notation) {
        return;
    }

    INotation::Params params;
    params.pageSize.width = width();
    params.pageSize.height = height();
    bool ok = _notation->load(filePath.toStdString(), params);
    if (!ok) {
        LOGE() << "failed load: " << filePath;
    }

    update();
}

void NotationPaintView::paint(QPainter *p)
{
    if (_notation) {
        _notation->paint(p);
    } else {
        p->drawText(10, 10, "no notation");
    }
}
