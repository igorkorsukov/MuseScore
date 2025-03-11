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
#include "voicecontrolviewmodel.h"

using namespace muse::ai;

VoiceControlViewModel::VoiceControlViewModel(QObject* parent)
    : QObject(parent)
{
    m_commandWavFile = globalConfiguration()->tempPath() + "/aicommand.wav";
}

VoiceControlViewModel::~VoiceControlViewModel()
{
}

void VoiceControlViewModel::toogleRecord()
{
    if (m_isProcessing) {
        return;
    }

    if (m_isRecording) {
        m_recorder.stop();
        setStatus("stop recording.");
        m_isProcessing = true;
        processWavFile(m_commandWavFile);
        m_isProcessing = false;
    } else {
        setStatus("start recording...");
        m_recorder.record(m_commandWavFile);
    }

    setIsRecording(!m_isRecording);
}

void VoiceControlViewModel::processWavFile(const io::path_t& wavFile)
{
    setStatus("start processing...");
    RecognizerClient::Result res = m_client.send(wavFile);
    setStatus("finish processing.");

    if (!res.responce.empty()) {
        m_resultList.append(QString::fromStdString(res.responce));
        emit resultListChanged();

        queryDispatcher()->dispatch(AiQuery(res.action));
    }
}

bool VoiceControlViewModel::isRecording() const
{
    return m_isRecording;
}

void VoiceControlViewModel::setIsRecording(bool newIsRecording)
{
    if (m_isRecording == newIsRecording) {
        return;
    }
    m_isRecording = newIsRecording;
    emit isRecordingChanged();
}

QString VoiceControlViewModel::status() const
{
    return m_status;
}

void VoiceControlViewModel::setStatus(const QString& newStatus)
{
    if (m_status == newStatus) {
        return;
    }
    m_status = newStatus;
    emit statusChanged();
}

QStringList VoiceControlViewModel::resultList() const
{
    return m_resultList;
}
