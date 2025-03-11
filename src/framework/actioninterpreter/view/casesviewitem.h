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
#include "../internal/iaicasesservice.h"

namespace muse::ai {
class CasesViewItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text NOTIFY caseChanged FINAL)
    Q_PROPERTY(QString actions READ actions NOTIFY caseChanged FINAL)

public:

    explicit CasesViewItem(QObject* parent = nullptr);

    void setCase(const AiCase& c);
    const AiCase& aicase() const;

    QString text() const;
    QString actions() const;

signals:
    void caseChanged();

private:
    AiCase m_case;
};
}
