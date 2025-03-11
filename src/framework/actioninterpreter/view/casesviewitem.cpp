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

QString CasesViewItem::name() const
{
    return m_name;
}

void CasesViewItem::setName(const QString& newName)
{
    if (m_name == newName) {
        return;
    }
    m_name = newName;
    emit nameChanged();
}

QString CasesViewItem::text() const
{
    return m_text;
}

void CasesViewItem::setText(const QString& newText)
{
    if (m_text == newText) {
        return;
    }
    m_text = newText;
    emit textChanged();
}

QStringList CasesViewItem::actions() const
{
    return m_actions;
}

void CasesViewItem::setActions(const QStringList& newActions)
{
    if (m_actions == newActions) {
        return;
    }
    m_actions = newActions;
    emit actionsChanged();
}
