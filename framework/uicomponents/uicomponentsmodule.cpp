#include "uicomponentsmodule.h"

using namespace muf;

std::string UiComponentsModule::moduleName() const
{
    return "uicomponents";
}

void UiComponentsModule::registerResources()
{
    Q_INIT_RESOURCE(uicomponents);
}

void UiComponentsModule::registerUiTypes()
{

}
