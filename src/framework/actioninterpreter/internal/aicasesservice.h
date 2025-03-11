/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2025 MuseScore Limited
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
#pragma once

#include "iaicasesservice.h"
#include "global/async/asyncable.h"

#include "global/modularity/ioc.h"
#include "../iaiconfiguration.h"
#include "../iaiquerydispatcher.h"

namespace muse::ai {
class AiCasesService : public IAiCasesService, public async::Asyncable
{
    Inject<IAiConfiguration> configuration;
    Inject<IAiQueryDispatcher> dispatcher;

public:
    AiCasesService() = default;

    AiCases cases() const override;
    muse::async::Channel<AiCase> caseAdded() const override;
    muse::async::Channel<AiCase> caseChanged() const override;

    void addNewCase() override;
    void updateCase(const AiCase& c) override;

    void runCase(const AiCase& c) override;

private:

    void load();
    void save();

    void nextAction();

    mutable AiCases m_cases;
    muse::async::Channel<AiCase> m_caseAdded;
    muse::async::Channel<AiCase> m_caseChanged;

    AiCase m_currentCase;
};
}
