#include "uimodule.h"

#include <QtQml>

#include "modularity/ioc.h"

#include "uiengine.h"
#include "qmltheme.h"
#include "iconcodes.h"

using namespace msf;

std::string UiModule::moduleName() const
{
    static std::string name = "uiengine";
    return name;
}

void UiModule::registerExports()
{
    ioc()->registerExport<IUiEngine>(moduleName(), UiEngine::instance());
}

void UiModule::registerUiTypes()
{
    qmlRegisterUncreatableType<UiEngine>("MuseScore.Ui", 1, 0, "UiEngine", "Cannot create an UiEngine");
    qmlRegisterUncreatableType<QmlTheme>("MuseScore.Ui", 1, 0, "QmlTheme", "Cannot create a QmlTheme");
    qmlRegisterUncreatableType<IconCode>("MuseScore.Ui", 1, 0, "IconCode", "Cannot create an IconCode");
}
