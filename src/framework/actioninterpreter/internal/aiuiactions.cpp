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
#include "aiuiactions.h"

#include "ui/uiaction.h"
#include "shortcuts/shortcutcontext.h"
#include "types/translatablestring.h"

using namespace muse;
using namespace muse::ui;
using namespace muse::actions;
using namespace muse::ai;

const UiActionList AiUiActions::m_actions = {
    UiAction("action://diagnostic/ai/voicecontrol",
             muse::ui::UiCtxAny,
             muse::shortcuts::CTX_ANY,
             TranslatableString("ai", "Show ai voice control")
             ),
    UiAction("action://diagnostic/ai/cases",
             muse::ui::UiCtxAny,
             muse::shortcuts::CTX_ANY,
             TranslatableString("ai", "Show ai cases")
             )
};

const UiActionList& AiUiActions::actionsList() const
{
    return m_actions;
}

bool AiUiActions::actionEnabled(const UiAction&) const
{
    return true;
}

muse::async::Channel<ActionCodeList> AiUiActions::actionEnabledChanged() const
{
    static muse::async::Channel<ActionCodeList> ch;
    return ch;
}

bool AiUiActions::actionChecked(const UiAction&) const
{
    return false;
}

muse::async::Channel<ActionCodeList> AiUiActions::actionCheckedChanged() const
{
    static muse::async::Channel<ActionCodeList> ch;
    return ch;
}
