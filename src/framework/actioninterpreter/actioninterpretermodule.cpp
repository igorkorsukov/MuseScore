/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore Limited
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "actioninterpretermodule.h"

#include "modularity/ioc.h"
#include "ui/iinteractiveuriregister.h"
#include "ui/iuiactionsregister.h"

#include "internal/aiquerydispatcher.h"
#include "internal/aiuiactions.h"
#include "internal/aiactioncontroller.h"

#include "view/aidiagnosticviewmodel.h"

#include "dev/devnetlistener.h"

using namespace muse::modularity;
using namespace muse::ai;

static void ai_init_qrc()
{
    Q_INIT_RESOURCE(ai);
}

std::string ActionInterpreterModule::moduleName() const
{
    return "actioninterpreter";
}

void ActionInterpreterModule::registerExports()
{
    m_actionController = std::make_shared<AiActionController>();
    m_devNetListener = std::make_shared<DevNetListener>();

    ioc()->registerExport<IAIQueryDispatcher>(moduleName(), new AIQueryDispatcher());
}

void ActionInterpreterModule::resolveImports()
{
    auto ir = ioc()->resolve<muse::ui::IInteractiveUriRegister>(moduleName());
    if (ir) {
        ir->registerQmlUri(Uri("muse://actioninterpreter/diagnostic"), "Muse/Ai/AiDiagnosticDialog.qml");
    }

    auto ar = ioc()->resolve<muse::ui::IUiActionsRegister>(moduleName());
    if (ar) {
        ar->reg(std::make_shared<AiUiActions>());
    }
}

void ActionInterpreterModule::registerResources()
{
    ai_init_qrc();
}

void ActionInterpreterModule::registerUiTypes()
{
    qmlRegisterType<AiDiagnosticViewModel>("Muse.Ai", 1, 0, "AiDiagnosticViewModel");
}

void ActionInterpreterModule::onInit(const IApplication::RunMode&)
{
    m_actionController->init();
    m_devNetListener->listen();
}
