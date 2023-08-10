# SPDX-License-Identifier: GPL-3.0-only
# MuseScore-CLA-applies
#
# MuseScore
# Music Composition & Notation
#
# Copyright (C) 2023 MuseScore BVBA and others
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

if (MUE_COMPILE_USE_SYSTEM_FREETYPE)
    find_package(Freetype)
    if (FREETYPE_FOUND)
        message(STATUS "Found freetype: ${FREETYPE_VERSION_STRING}")
    else()
        message(WARNING "Set MUE_COMPILE_USE_SYSTEM_FREETYPE=ON, but system freetype not found, built-in will be used")
    endif()
endif()

if (NOT FREETYPE_FOUND)
    add_subdirectory(${THIRDPARTY_DIR}/freetype freetype)
    set(FREETYPE_LIBRARIES freetype)
    set(FREETYPE_INCLUDE_DIRS ${THIRDPARTY_DIR}/freetype/include)
endif()
