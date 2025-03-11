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
#include "casesviewmodel.h"

#include "log.h"

using namespace muse::ai;

CasesViewModel::CasesViewModel(QObject* parent)
    : QObject(parent)
{
}

void CasesViewModel::init()
{
    const AiCases cases = casesService()->cases();

    qDeleteAll(m_items);
    m_items.clear();

    for (auto it = cases.crbegin(); it != cases.crend(); ++it) {
        m_items << makeItem(*it);
    }

    if (!m_items.isEmpty()) {
        m_currentCase = m_items.at(0);
    }

    casesService()->caseAdded().onReceive(this, [this](const AiCase& c) {
        m_items.prepend(makeItem(c));
        m_currentCase = m_items.last();
        emit casesChanged();
        emit currentCaseChanged();
    });

    emit casesChanged();
    emit currentCaseChanged();
}

CasesViewItem* CasesViewModel::makeItem(const AiCase& c) const
{
    CasesViewItem* item = new CasesViewItem();
    item->setName(QString::fromStdString(c.name));
    item->setText(QString::fromStdString(c.text));
    QStringList l;
    l.reserve(c.actions.size());
    std::transform(c.actions.cbegin(), c.actions.cend(), std::back_inserter(l), &QString::fromStdString);
    item->setActions(l);

    return item;
}

void CasesViewModel::selectCase(int index)
{
    IF_ASSERT_FAILED(index >= 0 && index < m_items.size()) {
        return;
    }

    m_currentCase = m_items.at(index);
    emit currentCaseChanged();
}

void CasesViewModel::addNewCase()
{
    casesService()->addNewCase();
}

void CasesViewModel::saveCurrent()
{
}

QList<CasesViewItem*> CasesViewModel::cases() const
{
    return m_items;
}

CasesViewItem* CasesViewModel::currentCase() const
{
    return m_currentCase;
}
