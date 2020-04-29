#include "uimodule.h"

#include <QtQml>

#include "modularity/ioc.h"

#include "uiengine.h"
#include "qmltheme.h"
#include "iconfont.h"

using namespace mu::framework;

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
    qmlRegisterUncreatableType<QmlTheme>("MuseScore.Ui", 1, 0, "IconFont", "Cannot create an IconFont");
}
