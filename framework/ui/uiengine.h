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

#ifndef MSF_UIENGINE_H
#define MSF_UIENGINE_H

#include <memory>

#include "interfaces/iuiengine.h"

class QQmlEngine;

namespace msf {

class UiEngine : public IUiEngine
{
public:
    ~UiEngine();

    static UiEngine* instance() {
        static UiEngine e;
        return &e;
    }

    QQmlEngine* qmlEngine() const override;

    void clearComponentCache() override;

private:

    UiEngine();

    QQmlEngine* engine() const;

    mutable QQmlEngine* _engine = nullptr;
};

}

#endif // MSF_UIENGINE_H
