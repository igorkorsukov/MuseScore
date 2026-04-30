/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2026 MuseScore Limited and others
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

#include "audiooutputnode.h"

using namespace muse;
using namespace muse::audio;
using namespace muse::audio::engine;

const AudioOutputParams& AudioOutputNode::outputParams() const
{
    return m_params;
}

void AudioOutputNode::applyOutputParams(const AudioOutputParams& requiredParams)
{
    if (m_params == requiredParams) {
        return;
    }
    m_params = onOutputParamsChanged(requiredParams);
    m_paramsChanges.send(m_params);
}

async::Channel<AudioOutputParams> AudioOutputNode::outputParamsChanged() const
{
    return m_paramsChanges;
}

AudioOutputParams AudioOutputNode::onOutputParamsChanged(const AudioOutputParams& requiredParams)
{
    return requiredParams;
}
