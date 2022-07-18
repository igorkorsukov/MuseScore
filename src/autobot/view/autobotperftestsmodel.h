/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
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
#ifndef MU_AUTOBOT_AUTOBOTPERFTESTSMODEL_H
#define MU_AUTOBOT_AUTOBOTPERFTESTSMODEL_H

#include <QAbstractListModel>

namespace mu::autobot {
class AutobotPerfTestsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AutobotPerfTestsModel(QObject* parent = nullptr);
    ~AutobotPerfTestsModel();

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void load();

private:
    enum Roles {
        rTitle = Qt::UserRole + 1,
    };
};
}

#endif // MU_AUTOBOT_AUTOBOTPERFTESTSMODEL_H
