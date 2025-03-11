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

#include "global/modularity/ioc.h"
#include "global/iglobalconfiguration.h"
#include "../iaiquerydispatcher.h"

#include "../internal/soxrecorder.h"
#include "../internal/recognizerclient.h"

namespace muse::ai {
class VoiceControlViewModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isRecording READ isRecording NOTIFY isRecordingChanged FINAL)
    Q_PROPERTY(QStringList resultList READ resultList NOTIFY resultListChanged FINAL)

    Q_PROPERTY(QString status READ status NOTIFY statusChanged FINAL)

    Inject<IGlobalConfiguration> globalConfiguration;
    Inject<IAiQueryDispatcher> queryDispatcher;

public:
    VoiceControlViewModel(QObject* parent = nullptr);
    ~VoiceControlViewModel();

    bool isRecording() const;
    QString status() const;
    QStringList resultList() const;

    Q_INVOKABLE void toogleRecord();

signals:
    void isRecordingChanged();
    void statusChanged();
    void resultListChanged();

private:

    void setStatus(const QString& newStatus);
    void setIsRecording(bool newIsRecording);

    void processWavFile(const io::path_t& wavFile);

    bool m_isRecording = false;
    bool m_isProcessing = false;
    io::path_t m_commandWavFile;
    SoxRecorder m_recorder;
    RecognizerClient m_client;
    QString m_status;
    QStringList m_resultList;
};
}
