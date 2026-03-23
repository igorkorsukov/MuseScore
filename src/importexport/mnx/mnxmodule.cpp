/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2026 MuseScore Limited
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
#include "mnxmodule.h"

#include "modularity/ioc.h"

#include "internal/mnxconfiguration.h"
#include "internal/notationmnxreader.h"
#include "internal/notationmnxwriter.h"
#include "project/inotationreadersregister.h"
#include "project/inotationwritersregister.h"

using namespace muse::modularity;
using namespace mu::iex::mnxio;
using namespace mu::project;

static const std::string mname("iex_mnx");

std::string MnxModule::moduleName() const
{
    return mname;
}

void MnxModule::registerExports()
{
    m_configuration = std::make_shared<MnxConfiguration>();
    globalIoc()->registerExport<IMnxConfiguration>(mname, m_configuration);
}

void MnxModule::resolveImports()
{
    auto readers = globalIoc()->resolve<INotationReadersRegister>(mname);
    if (readers) {
        readers->reg({ "mnx", "json" }, std::make_shared<NotationMnxReader>());
    }
}

void MnxModule::onInit(const muse::IApplication::RunMode&)
{
    m_configuration->init();
}

IContextSetup* MnxModule::newContext(const muse::modularity::ContextPtr& ctx) const
{
    return new MnxModuleContext(ctx);
}

void MnxModuleContext::resolveImports()
{
    auto writers = ioc()->resolve<INotationWritersRegister>(mname);
    if (writers) {
        writers->reg({ "mnx" }, std::make_shared<NotationMnxWriter>());
    }
}
