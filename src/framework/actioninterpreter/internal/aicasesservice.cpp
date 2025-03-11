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
#include "aicasesservice.h"

#include "global/io/file.h"
#include "global/serialization/json.h"
#include "global/translation.h"
#include "global/uuid.h"
#include "global/async/async.h"
#include "global/containers.h"

#include "log.h"

using namespace muse::ai;

void AiCasesService::load()
{
    io::path_t filePath = configuration()->casesFilePath();
    ByteArray data;
    Ret ret = io::File::readFile(filePath, data);
    if (!ret) {
        LOGE() << "failed read err: " << ret << ", file: " << filePath;
        return;
    }

    JsonArray arr = JsonDocument::fromJson(data).rootArray();
    m_cases.reserve(arr.size());
    for (size_t i = 0; i < arr.size(); ++i) {
        JsonObject obj = arr.at(i).toObject();
        AiCase c;
        c.uuid = obj.value("uuid").toStdString();
        c.text = obj.value("text").toStdString();

        JsonArray acts = obj.value("actions").toArray();
        for (size_t j = 0; j < acts.size(); ++j) {
            c.actions.push_back(AiQuery(acts.at(j).toStdString()));
        }

        m_cases.push_back(std::move(c));
    }
}

void AiCasesService::save()
{
    JsonArray arr;
    for (const AiCase& c : m_cases) {
        JsonObject obj;
        obj["uuid"] = c.uuid;
        obj["text"] = c.text;

        JsonArray acts;
        for (const auto& a : c.actions) {
            acts.append(a.toString());
        }
        obj["actions"] = acts;

        arr.append(obj);
    }

    ByteArray data = JsonDocument(arr).toJson();
    io::path_t filePath = configuration()->casesFilePath();
    Ret ret = io::File::writeFile(filePath, data);
    if (!ret) {
        LOGE() << "failed write err: " << ret << ", file: " << filePath;
    }
}

void AiCasesService::addNewCase()
{
    AiCase c;
    c.uuid = Uuid::gen();
    c.text = muse::trc("ai", "New case");

    m_cases.push_back(c);

    save();

    m_caseAdded.send(c);
}

void AiCasesService::updateCase(const AiCase& c)
{
    const std::string& uuid = c.uuid;
    auto it = std::find_if(m_cases.begin(), m_cases.end(), [uuid](const AiCase& c) {
        return c.uuid == uuid;
    });

    // add new
    if (it == m_cases.end()) {
        m_cases.push_back(c);
        save();
        m_caseAdded.send(c);
        return;
    }

    // update
    *it = c;
    save();
    m_caseChanged.send(c);
}

AiCases AiCasesService::cases() const
{
    if (m_cases.empty()) {
        const_cast<AiCasesService*>(this)->load();
    }
    return m_cases;
}

muse::async::Channel<AiCase> AiCasesService::caseAdded() const
{
    return m_caseAdded;
}

muse::async::Channel<AiCase> AiCasesService::caseChanged() const
{
    return m_caseChanged;
}

void AiCasesService::runCase(const AiCase& c)
{
    if (m_currentCase.uuid != "") {
        LOGE() << "Busy, another case is running, text: " << m_currentCase.text;
        return;
    }

    m_currentCase = c;

    nextAction();
}

void AiCasesService::nextAction()
{
    if (m_currentCase.actions.empty()) {
        LOGD() << "finished case: " << m_currentCase.text;
        m_currentCase = AiCase();
        return;
    }

    AiQuery q = muse::takeFirst(m_currentCase.actions);
    dispatcher()->dispatch(q);

    async::Async::call(this, [this]() {
        nextAction();
    });
}
