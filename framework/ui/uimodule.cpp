#include "uimodule.h"

#include "modularity/ioc.h"

#include "uiengine.h"

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

}
