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

#include <memory>

#include "global/modularity/ioc.h"
#include "global/async/asyncable.h"

#include "../iplayhead.h"
#include "iaudiofactory.h"

#include "mixerchannel.h"
#include "nodes/fxnode.h"
#include "nodes/controlnode.h"
#include "nodes/signalnode.h"

namespace muse {
class TaskScheduler;
}

namespace muse::audio {
struct MixerContextTag
{
    static constexpr const char* name = "MixerContext";
};
}

namespace muse::audio::engine {
class Mixer : public AudioNode<MixerContextTag>, public async::Asyncable
{
    GlobalInject<IAudioFactory> audioFactory;

public:
    ~Mixer() override;

    void init();

    Ret addChannel(AudioOutputNodePtr output);
    Ret addAuxChannel(AudioOutputNodePtr output);
    Ret removeChannel(const TrackId trackId);

    void setPlayhead(PlayheadPtr playhead);

    AudioOutputParams masterOutputParams() const;
    void setMasterOutputParams(const AudioOutputParams& params);
    void clearMasterOutputParams();
    async::Channel<AudioOutputParams> masterOutputParamsChanged() const;

    AudioSignalChanges masterAudioSignalChanges() const;

    void setIsIdle(bool idle);
    void setTracksToProcessWhenIdle(const std::unordered_set<TrackId>& trackIds);

    void process(float* buffer, samples_t samplesPerChannel) override;

private:

    void onOutputSpecChanged(const OutputSpec& spec) override;
    void onModeChanged(const ProcessMode mode) override;

    void doSelfProcess(float* buffer, samples_t samplesPerChannel) override;

    void processTrackChannels(size_t outBufferSize, size_t samplesPerChannel);
    void mixOutputFromChannel(float* outBuffer, const float* inBuffer, unsigned int samplesCount) const;
    void prepareAuxBuffers(size_t outBufferSize);
    void writeTrackToAuxBuffers(const float* trackBuffer, const AuxSendsParams& auxSends, samples_t samplesPerChannel);
    void processAuxChannels(float* buffer, samples_t samplesPerChannel);
    void processMasterFx(float* buffer, samples_t samplesPerChannel);

    void updateNonMutedTrackCount();
    bool useMultithreading() const;

    void updateShouldProcessMasterFxDuringSilence();

    void notifyAboutAudioSignalChanges();
    void notifyNoAudioSignal();

    TaskScheduler* m_taskScheduler = nullptr;

    size_t m_nonMutedTrackCount = 0;

    AudioOutputParams m_masterParams;
    async::Channel<AudioOutputParams> m_masterOutputParamsChanged;
    std::vector<FxNodePtr> m_masterFxNodes;

    struct TrackData {
        TrackId trackId;
        MixerChannelPtr channel;
        std::vector<float> buffer;
        bool processed = false;
    };

    std::vector<TrackData> m_tracks;

    std::unordered_set<TrackId> m_tracksToProcessWhenIdle;

    struct AuxChannelInfo {
        MixerChannelPtr channel;
        std::vector<float> buffer;
        bool receivedAudioSignal = false;
    };

    std::vector<AuxChannelInfo> m_auxChannelInfoList;

    std::shared_ptr<IPlayhead> m_playhead;

    bool m_chainProcessing = false;
    SignalNodePtr m_signalNode;
    ControlNodePtr m_controlNode;

    bool m_shouldProcessMasterFxDuringSilence = false;
    bool m_isIdle = false;
};

using MixerPtr = std::shared_ptr<Mixer>;
}
