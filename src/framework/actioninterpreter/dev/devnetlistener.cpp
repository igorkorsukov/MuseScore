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
#include "devnetlistener.h"

#include <QTcpServer>
#include <QTcpSocket>

#include "log.h"

using namespace muse::ai;

static const int TIMEOUT_MSEC(500);

DevNetListener::DevNetListener()
{
    m_server = new QTcpServer();
}

DevNetListener::~DevNetListener()
{
    delete m_server;
}

void DevNetListener::listen(int port)
{
    bool ok = m_server->listen(QHostAddress::LocalHost, port);
    if (!ok) {
        LOGE() << "failed listen port: " << port;
        return;
    }

    QObject::connect(m_server, &QTcpServer::newConnection, [this]() {
        QTcpSocket* socket = m_server->nextPendingConnection();
        if (!socket) {
            return;
        }

        socket->waitForReadyRead(TIMEOUT_MSEC);

        QByteArray data = socket->readAll();

        LOGD() << "data: " << data;
        if (data.isEmpty()) {
            return;
        }

        std::string str = String::fromUtf8(data.constData()).toStdString();

        AiQuery q(str);
        if (!q.isValid() || q.uri().scheme() != "ai") {
            LOGE() << "bad query: " << str;
            return;
        }

        dispatcher()->dispatch(q);
    });
}
