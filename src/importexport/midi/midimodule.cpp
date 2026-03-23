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
#include "midimodule.h"

#include "modularity/ioc.h"

#include "project/inotationreadersregister.h"
#include "internal/notationmidireader.h"
#include "project/inotationwritersregister.h"
#include "internal/notationmidiwriter.h"

#include "internal/midiconfiguration.h"

#include "log.h"

using namespace muse::modularity;
using namespace mu::iex::midi;
using namespace mu::project;

static const std::string mname("iex_midi");

std::string MidiModule::moduleName() const
{
    return mname;
}

void MidiModule::registerExports()
{
    m_configuration = std::make_shared<MidiConfiguration>();

    globalIoc()->registerExport<IMidiImportExportConfiguration>(mname, m_configuration);
}

void MidiModule::resolveImports()
{
    auto readers = globalIoc()->resolve<INotationReadersRegister>(mname);
    if (readers) {
        readers->reg({ "mid", "midi", "kar" }, std::make_shared<NotationMidiReader>());
    }
}

void MidiModule::onInit(const muse::IApplication::RunMode&)
{
    m_configuration->init();
}

IContextSetup* MidiModule::newContext(const muse::modularity::ContextPtr& ctx) const
{
    return new MidiContext(ctx);
}

void MidiContext::resolveImports()
{
    auto writers = ioc()->resolve<INotationWritersRegister>(mname);
    if (writers) {
        writers->reg({ "mid", "midi", "kar" }, std::make_shared<NotationMidiWriter>());
    }
}
