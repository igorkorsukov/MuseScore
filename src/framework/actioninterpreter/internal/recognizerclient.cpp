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

    ByteArray fileData;
    Ret ret = io::File::readFile(wavFile, fileData);
    if (!ret) {
        LOGE() << "failed read err: " << ret.toString() << ", file: " << wavFile;
        return Result();
    }

    LOGDA() << "wav size: " << fileData.size();

    QByteArray fileBase64 = fileData.toQByteArrayNoCopy().toBase64();
    std::string fileStr(fileBase64.constData(), fileBase64.size());
    LOGDA() << "fileStr: " << fileStr.size();

    io::File::writeFile(wavFile + ".txt", ByteArray::fromQByteArrayNoCopy(fileBase64));

    JsonObject obj;
    obj["reqID"] = ++s_lastReqId;
    obj["wavFile"] = fileStr;

    ByteArray json = JsonDocument(obj).toJson();
    QByteArray ba = json.toQByteArrayNoCopy();

    QUrl url = QUrl(QString::fromStdString(m_config.endPoint) + "/asr");
    QBuffer buf(&ba);
    OutgoingDevice device(&buf);

    QByteArray resData;
    QBuffer resBuf(&resData);

    m_network->post(url, &device, &resBuf);

    LOGDA() << resData.constData();

    JsonObject resObj = JsonDocument::fromJson(ByteArray::fromQByteArrayNoCopy(resData)).rootObject();

    Result result;
    result.transcribe = resObj.value("transcribe").toStdString();

    return result;
}
