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

namespace muse::ai {
class CasesViewItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged FINAL)
    Q_PROPERTY(QStringList actions READ actions WRITE setActions NOTIFY actionsChanged FINAL)

public:

    explicit CasesViewItem(QObject* parent = nullptr);

    QString name() const;
    void setName(const QString& newName);
    QString text() const;
    void setText(const QString& newText);
    QStringList actions() const;
    void setActions(const QStringList& newActions);

signals:
    void nameChanged();
    void textChanged();
    void actionsChanged();

private:
    QString m_name;
    QString m_text;
    QStringList m_actions;
};
}
