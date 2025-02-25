# SPDX-License-Identifier: GPL-3.0-only
# MuseScore-Studio-CLA-applies
#
# MuseScore Studio
# Music Composition & Notation
#
# Copyright (C) 2025 MuseScore Limited
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# This is module for convert mscz files to various formats (image, pdf, audio and etc).
# Main goal - determine what need to do and call necessary functions.
# Usually use from command line.

declare_module(actioninterpreter)
set(MODULE_ALIAS muse::actioninterpreter)

set(MODULE_SRC
    ${CMAKE_CURRENT_LIST_DIR}/actioninterpretermodule.cpp
    ${CMAKE_CURRENT_LIST_DIR}/actioninterpretermodule.h
    ${CMAKE_CURRENT_LIST_DIR}/aitypes.h
    ${CMAKE_CURRENT_LIST_DIR}/aiqueryable.h
    ${CMAKE_CURRENT_LIST_DIR}/iaiquerydispatcher.h

    ${CMAKE_CURRENT_LIST_DIR}/internal/aiquerydispatcher.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/aiquerydispatcher.h

    ${CMAKE_CURRENT_LIST_DIR}/dev/devnetlistener.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dev/devnetlistener.h
    )

setup_module()

