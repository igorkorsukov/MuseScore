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
#include "soxrecorder.h"

#include <QProcess>

using namespace muse::ai;

SoxRecorder::SoxRecorder()
{
    m_process = std::make_shared<QProcess>();
}

void SoxRecorder::record(const io::path_t& filePath)
{
    QStringList args = {
        "-d",
        "-r", "16000",
        "-b", "16",
        "--endian", "little",
        "-c 1",
        filePath.toQString()
    };
    m_process->start("sox", args);
}

void SoxRecorder::stop()
{
    m_process->terminate();
    m_process->waitForFinished();
}
