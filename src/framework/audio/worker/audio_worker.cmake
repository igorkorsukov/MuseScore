# SPDX-License-Identifier: GPL-3.0-only
# MuseScore-CLA-applies
#
# MuseScore
# Music Composition & Notation
#
# Copyright (C) 2025 MuseScore BVBA and others
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

set(AUDIO_WORKER_SRC
    ${CMAKE_CURRENT_LIST_DIR}/audioworkermodule.cpp
    ${CMAKE_CURRENT_LIST_DIR}/audioworkermodule.h
    ${CMAKE_CURRENT_LIST_DIR}/iworkerplayback.h
    ${CMAKE_CURRENT_LIST_DIR}/iaudioengine.h
    ${CMAKE_CURRENT_LIST_DIR}/iaudiosource.h
    ${CMAKE_CURRENT_LIST_DIR}/iaudiostream.h
    ${CMAKE_CURRENT_LIST_DIR}/iclock.h
    ${CMAKE_CURRENT_LIST_DIR}/isequenceio.h
    ${CMAKE_CURRENT_LIST_DIR}/itracksequence.h
    ${CMAKE_CURRENT_LIST_DIR}/isequenceplayer.h

    # internal
    ${CMAKE_CURRENT_LIST_DIR}/internal/workerplayback.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/workerplayback.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/workerchannelcontroller.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/workerchannelcontroller.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/audioengine.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/audioengine.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/mixer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/mixer.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/mixerchannel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/mixerchannel.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/clock.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/clock.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/sequenceio.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/sequenceio.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/tracksequence.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/tracksequence.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/igettracks.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/sequenceplayer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/sequenceplayer.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/audiostream.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/audiostream.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/samplerateconvertor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/samplerateconvertor.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/track.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/abstractaudiosource.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/abstractaudiosource.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/eventaudiosource.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/eventaudiosource.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/sinesource.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/sinesource.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/noisesource.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/noisesource.h

    # DSP
    ${CMAKE_CURRENT_LIST_DIR}/internal/dsp/envelopefilterconfig.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/dsp/compressor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/dsp/compressor.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/dsp/limiter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/internal/dsp/limiter.h
    ${CMAKE_CURRENT_LIST_DIR}/internal/dsp/audiomathutils.h
)
