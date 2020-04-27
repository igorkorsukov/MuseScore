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

<<<<<<< HEAD
#include "uiengine.h"

#include <QApplication>
#include <QQmlEngine>
#include <QStringList>
#include <QDir>
#include <QQmlContext>

namespace Ms {

extern QString mscoreGlobalShare; //! FIXME Need to remove global variable

=======
<<<<<<< HEAD:framework/global/modularity/imoduleexport.h
#ifndef MSF_IMODULEEXPORT_H
#define MSF_IMODULEEXPORT_H

#include <memory>
=======
#include "uiengine.h"

#include <QCoreApplication>
#include <QQmlEngine>
#include <QStringList>
#include <QDir>
>>>>>>> ccc13fc02... added ui engine:framework/ui/uiengine.cpp

#define INTERFACE_ID(cls)               \
public:                                 \
    static const char* interfaceId() {  \
        static const char* id = #cls;   \
        return id;                      \
    }                                   \

namespace msf {

<<<<<<< HEAD:framework/global/modularity/imoduleexport.h
class IModuleExportInterface
{
public:
    virtual ~IModuleExportInterface() {}

};

struct IModuleExportCreator {
    virtual ~IModuleExportCreator() {}
    virtual std::shared_ptr<IModuleExportInterface> create() = 0;
};

=======
>>>>>>> ccc13fc02... added ui engine
}

using namespace msf;

<<<<<<< HEAD
const std::shared_ptr<UiEngine>& UiEngine::instance()
{
    struct make_shared_enabler : public UiEngine {};
    static std::shared_ptr<UiEngine> e = std::make_shared<make_shared_enabler>();
    return e;
}

QQmlEngine* UiEngine::engine()
=======
UiEngine::UiEngine()
{

}

UiEngine::~UiEngine()
{
    delete _engine;
}

QQmlEngine* UiEngine::engine() const
>>>>>>> ccc13fc02... added ui engine
{
    if (_engine) {
        return _engine;
    }

<<<<<<< HEAD
    _engine = new QQmlEngine(this);

    setup();

    return _engine;
}

void UiEngine::setup()
{
    _engine->rootContext()->setContextProperty("ui", this);
    _theme = new QmlTheme(QApplication::palette(), this);

#ifdef Q_OS_WIN
    QStringList importPaths;
    QDir dir(QCoreApplication::applicationDirPath() + QString("/../qml"));
    importPaths.append(dir.absolutePath());
    engine->setImportPathList(importPaths);
#endif
#ifdef Q_OS_MAC
    QStringList importPaths;
    QDir dir(mscoreGlobalShare + QString("/qml"));
    importPaths.append(dir.absolutePath());
    engine->setImportPathList(importPaths);
#endif

    _engine->addImportPath(":/qml");
}

void UiEngine::updateTheme()
{
    if (!_engine) {
        return;
    }

    theme()->update(QApplication::palette());
}

QmlTheme* UiEngine::theme() const
{
    return _theme;
=======
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
>>>>>>> ccc13fc02... added ui engine
}

QQmlEngine* UiEngine::qmlEngine() const
{
<<<<<<< HEAD
    return const_cast<UiEngine*>(this)->engine();
=======
    return engine();
>>>>>>> ccc13fc02... added ui engine
}

void UiEngine::clearComponentCache()
{
    engine()->clearComponentCache();;
<<<<<<< HEAD
}
=======
>>>>>>> ccc13fc02... added ui engine:framework/ui/uiengine.cpp
}

#define MODULE_EXPORT_INTERFACE public msf::IModuleExportInterface
#define MODULE_EXPORT_CREATOR public msf::IModuleExportCreator

#endif // MSF_IMODULEEXPORT_H
>>>>>>> ccc13fc02... added ui engine
