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
#include "recognizerclient.h"

#include "global/io/file.h"
#include "global/serialization/json.h"

#include "log.h"

using namespace muse::ai;
using namespace muse::network;

static int s_lastReqId = 0;

RecognizerClient::Result RecognizerClient::send(const io::path_t& wavFile)
{
    if (!m_network) {
        m_network = networkCreator()->makeNetworkManager();
    }

    std::string fileBase64;

    // read file
    {
        ByteArray fileData;
        Ret ret = io::File::readFile(wavFile, fileData);
        if (!ret) {
            LOGE() << "failed read err: " << ret.toString() << ", file: " << wavFile;
            return Result();
        }

        LOGDA() << "wav size: " << fileData.size();

        QByteArray base64 = fileData.toQByteArrayNoCopy().toBase64();
        fileBase64 = std::string(base64.constData(), base64.size());
    }

    // pack request
    ByteArray request;
    {
        JsonObject obj;
        obj["reqID"] = ++s_lastReqId;
        obj["wavFile"] = fileBase64;

        request = JsonDocument(obj).toJson();
    }

    // send to asr
    QByteArray responce;
    {
        QUrl url = QUrl(QString::fromStdString(m_config.endPoint) + "/asr");
        QByteArray qreq = request.toQByteArrayNoCopy();
        QBuffer reqBuf(&qreq);
        OutgoingDevice out(&reqBuf);

        QBuffer resBuf(&responce);

        RequestHeaders headers;
        headers.rawHeaders["Content-Type"] = "application/json";

        m_network->post(url, &out, &resBuf, headers);

        LOGDA() << responce.constData();
    }

    // result
    Result result;
    {
        result.responce = std::string(responce.constData(), responce.size());

        ByteArray json = ByteArray::fromQByteArrayNoCopy(responce);
        JsonObject obj = JsonDocument::fromJson(json).rootObject();
        result.action = obj.value("action").toStdString();
    }

    return result;
}
