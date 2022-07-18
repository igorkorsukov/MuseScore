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
#include "autobotperftestsmodel.h"

using namespace mu::autobot;

AutobotPerfTestsModel::AutobotPerfTestsModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

AutobotPerfTestsModel::~AutobotPerfTestsModel()
{
}

QVariant AutobotPerfTestsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount()) {
        return QVariant();
    }

    switch (role) {
    case rTitle: return QString::number(index.row());
    }
    return QVariant();
}

int AutobotPerfTestsModel::rowCount(const QModelIndex&) const
{
    return static_cast<int>(6);
}

QHash<int, QByteArray> AutobotPerfTestsModel::roleNames() const
{
    static const QHash<int, QByteArray> roles = {
        { rTitle, "titleRole" },
    };
    return roles;
}

void AutobotPerfTestsModel::load()
{
    beginResetModel();

    endResetModel();
}
