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

#include "uiengine.h"

#include <QCoreApplication>
#include <QQmlEngine>
#include <QStringList>
#include <QDir>

namespace Ms {

extern QString mscoreGlobalShare;

}

using namespace msf;

UiEngine::UiEngine()
{

}

UiEngine::~UiEngine()
{
    delete _engine;
}

QQmlEngine* UiEngine::engine() const
{
    if (_engine) {
        return _engine;
    }

     _engine = new QQmlEngine();

#ifdef Q_OS_WIN
      QStringList importPaths;
      QDir dir(QCoreApplication::applicationDirPath() + QString("/../qml"));
      importPaths.append(dir.absolutePath());
      _engine->setImportPathList(importPaths);
#endif
#ifdef Q_OS_MAC
      QStringList importPaths;
      QDir dir(mscoreGlobalShare + QString("/qml"));
      importPaths.append(dir.absolutePath());
      _engine->setImportPathList(importPaths);
#endif

      _engine->addImportPath(":/qml");

      return _engine;
}

QQmlEngine* UiEngine::qmlEngine() const
{
    return engine();
}

void UiEngine::clearComponentCache()
{
    engine()->clearComponentCache();;
}
