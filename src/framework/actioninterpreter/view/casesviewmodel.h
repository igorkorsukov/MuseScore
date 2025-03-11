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

#include <QObject>
#include <QList>

#include "global/async/asyncable.h"

#include "global/modularity/ioc.h"
#include "../internal/iaicasesservice.h"

#include "casesviewitem.h"

namespace muse::ai {
class CasesViewModel : public QObject, public async::Asyncable
{
    Q_OBJECT
    Q_PROPERTY(QList<CasesViewItem*> cases READ cases NOTIFY casesChanged FINAL)
    Q_PROPERTY(CasesViewItem * currentCase READ currentCase NOTIFY currentCaseChanged FINAL)

    Inject<IAiCasesService> casesService;

public:
    CasesViewModel(QObject* parent = nullptr);

    QList<CasesViewItem*> cases() const;
    CasesViewItem* currentCase() const;

    Q_INVOKABLE void init();
    Q_INVOKABLE void selectCase(int index);
    Q_INVOKABLE void addNewCase();

    Q_INVOKABLE void changeCurrentText(const QString& text);
    Q_INVOKABLE void changeCurrentActions(const QString& actions);

    Q_INVOKABLE void runCase();

signals:
    void casesChanged();
    void currentCaseChanged();

private:

    CasesViewItem* makeItem(const AiCase& c) const;
    CasesViewItem* findItem(const std::string& uuid) const;

    QList<CasesViewItem*> m_items;
    CasesViewItem* m_currentCase = nullptr;
};
}
