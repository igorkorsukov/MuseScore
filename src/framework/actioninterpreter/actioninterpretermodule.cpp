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

#include "internal/aiquerydispatcher.h"

#include "dev/devnetlistener.h"

using namespace muse::modularity;
using namespace muse::ai;

std::string ActionInterpreterModule::moduleName() const
{
    return "actioninterpreter";
}

void ActionInterpreterModule::registerExports()
{
    m_devNetListener = std::make_shared<DevNetListener>();

    ioc()->registerExport<IAIQueryDispatcher>(moduleName(), new AIQueryDispatcher());
}

void ActionInterpreterModule::onInit(const IApplication::RunMode&)
{
    m_devNetListener->listen();
}
