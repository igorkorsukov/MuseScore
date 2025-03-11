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
        c.name = obj.value("name").toStdString();
        c.text = obj.value("text").toStdString();

        JsonArray acts = obj.value("actions").toArray();
        for (size_t j = 0; j < acts.size(); ++j) {
            c.actions.push_back(acts.at(j).toStdString());
        }

        m_cases.push_back(std::move(c));
    }
}

void AiCasesService::save()
{
    JsonArray arr;
    for (const AiCase& c : m_cases) {
        JsonObject obj;
        obj["name"] = c.name;
        obj["text"] = c.text;

        JsonArray acts;
        for (const auto& a : c.actions) {
            acts.append(a);
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
    c.name = muse::trc("ai", "New case");

    m_cases.push_back(c);

    save();

    m_caseAdded.send(c);
}

void AiCasesService::updateCase(const AiCase& c)
{
    const std::string& name = c.name;
    auto it = std::find_if(m_cases.begin(), m_cases.end(), [name](const AiCase& c) {
        return c.name == name;
    });

    if (it == m_cases.end()) {
        m_cases.push_back(c);
        save();
        m_caseAdded.send(c);
        return;
    }

    *it = c;
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
