/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore Limited and others
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

#include "global/modularity/ioc.h"
#include "global/async/asyncable.h"
#include "global/async/notification.h"

#include "nodes/audiooutputnode.h"

#include "iaudiofactory.h"
#include "../iplayhead.h"
#include "nodes/fxnode.h"
#include "nodes/controlnode.h"
#include "nodes/signalnode.h"

namespace muse::audio::engine {
class MixerChannel : public AudioOutputNode, public async::Asyncable
{
    GlobalInject<IAudioFactory> audioFactory;

public:
    explicit MixerChannel(const TrackId trackId, AudioSourceNodePtr source, PlayheadPositionPtr playheadPosition);
    explicit MixerChannel(const TrackId trackId, PlayheadPositionPtr playheadPosition);

    void init();

    void setPlayheadPosition(PlayheadPositionPtr playheadPosition);

    TrackId trackId() const;
    IAudioNodePtr source() const;

    bool muted() const;
    async::Notification mutedChanged() const;

    bool isSilent() const;
    bool shouldProcessDuringSilence() const;
    async::Channel<bool> shouldProcessDuringSilenceChanged() const;

    void setNoAudioSignal();
    void notifyAboutAudioSignalChanges();
    AudioSignalChanges audioSignalChanges() const;

    void process(float* buffer, samples_t samplesPerChannel) override;

private:

    void onOutputSpecChanged(const OutputSpec& spec) override;
    void onModeChanged(const ProcessMode mode) override;
    AudioOutputParams onOutputParamsChanged(const AudioOutputParams& requiredParams) override;

    void doSelfProcess(float* buffer, samples_t samplesPerChannel) override;

    void updateShouldProcessDuringSilence();

    TrackId m_trackId = -1;
    PlayheadPositionPtr m_playheadPosition;

    bool m_chainProcessing = false;
    SignalNodePtr m_signalNode;
    ControlNodePtr m_controlNode;
    std::vector<FxNodePtr> m_fxNodes;
    AudioSourceNodePtr m_sourceNode;

    bool m_shouldProcessDuringSilence = false;
    async::Channel<bool> m_shouldProcessDuringSilenceChanged;

    async::Notification m_mutedChanged;
};

using MixerChannelPtr = std::shared_ptr<MixerChannel>;
}
