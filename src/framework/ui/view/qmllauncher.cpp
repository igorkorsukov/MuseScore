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
#include "qmllauncher.h"

using namespace muse::ui;

QmlLauncher::QmlLauncher(QObject* parent, const modularity::ContextPtr& iocCtx)
    : QObject(parent), Injectable(iocCtx)
{
}

bool QmlLauncher::open(const QString& uri)
{
    interactive()->open(UriQuery(uri.toStdString()));
    return true;
}

bool QmlLauncher::openApp(const QString& uri)
{
    interactive()->openApp(Uri(uri));
    return true;
}

bool QmlLauncher::openUrl(const QString& url)
{
    return interactive()->openUrl(QUrl(url));
}
