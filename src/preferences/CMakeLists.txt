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

declare_module(preferences)

set(MODULE_QRC preferences.qrc)
set(MODULE_QML_IMPORT ${CMAKE_CURRENT_LIST_DIR}/qml)

set(MODULE_SRC
    ${CMAKE_CURRENT_LIST_DIR}/preferencesmodule.cpp
    ${CMAKE_CURRENT_LIST_DIR}/preferencesmodule.h

    ${CMAKE_CURRENT_LIST_DIR}/view/preferencesmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/preferencesmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/preferencepageitem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/preferencepageitem.h
    ${CMAKE_CURRENT_LIST_DIR}/view/folderspreferencesmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/folderspreferencesmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/canvaspreferencesmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/canvaspreferencesmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/saveandpublishpreferencesmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/saveandpublishpreferencesmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/generalpreferencesmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/generalpreferencesmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/updatepreferencesmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/updatepreferencesmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/appearancepreferencesmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/appearancepreferencesmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/noteinputpreferencesmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/noteinputpreferencesmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/advancedpreferencesmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/advancedpreferencesmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/scorepreferencesmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/scorepreferencesmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/importpreferencesmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/importpreferencesmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/audiomidipreferencesmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/audiomidipreferencesmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/percussionpreferencesmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/percussionpreferencesmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/commonaudioapiconfigurationmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/commonaudioapiconfigurationmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/view/braillepreferencesmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/braillepreferencesmodel.h
    )

setup_module()




