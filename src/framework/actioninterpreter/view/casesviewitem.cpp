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
#include "casesviewitem.h"

using namespace muse::ai;

CasesViewItem::CasesViewItem(QObject* parent)
    : QObject(parent)
{
}

void CasesViewItem::setCase(const AiCase& c)
{
    m_case = c;
    emit caseChanged();
}

const AiCase& CasesViewItem::aicase() const
{
    return m_case;
}

QString CasesViewItem::text() const
{
    return QString::fromStdString(m_case.text);
}

QString CasesViewItem::actions() const
{
    QString str;
    for (const AiQuery& q : m_case.actions) {
        str += QString::fromStdString(q.toString()) + "\n";
    }
    return str;
}
