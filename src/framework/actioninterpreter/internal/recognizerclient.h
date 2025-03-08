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

#include "global/io/path.h"
#include "global/modularity/ioc.h"
#include "network/inetworkmanagercreator.h"

namespace muse::ai {
class RecognizerClient
{
    Inject<network::INetworkManagerCreator> networkCreator;

public:
    RecognizerClient() = default;

    struct Config {
        std::string endPoint = "http://localhost:2212";
    };

    void setConfig(const Config& c);

    struct Result {
        std::string responce;
        std::string action;
    };

    Result send(const io::path_t& wavFile);

private:

    Config m_config;
    network::INetworkManagerPtr m_network;
};
}
