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

#include "nodes/fxchain.h"
#include "nodes/controlnode.h"
#include "nodes/signalnode.h"
#include "nodes/trackchain.h"

namespace muse {
class TaskScheduler;
}

namespace muse::audio {
struct ContextMixerTag
{
    static constexpr const char* name = "ContextMixer";
};
}

namespace muse::audio::engine {
class Mixer : public AudioNode<ContextMixerTag>, public async::Asyncable
{
    GlobalInject<IAudioFactory> audioFactory;

public:
    ~Mixer() override;

    void init();

    Ret addChannel(TrackChainPtr trackChain, const AuxSendsParams& auxSends);
    Ret addAuxChannel(TrackChainPtr trackChain);
    Ret removeChannel(const TrackId trackId);

    void setPlayhead(PlayheadPtr playhead);

    AudioOutputParams masterOutputParams() const;
    void setMasterOutputParams(const AudioOutputParams& params);
    void clearMasterOutputParams();
    async::Channel<AudioOutputParams> masterOutputParamsChanged() const;

    AudioSignalChanges masterAudioSignalChanges() const;

    void setIsIdle(bool idle);
    void setTracksToProcessWhenIdle(const std::unordered_set<TrackId>& trackIds);
    void setNonMutedTrackCount(size_t count);

    void process(float* buffer, samples_t samplesPerChannel) override;

private:

    void onOutputSpecChanged(const OutputSpec& spec) override;
    void onModeChanged(const ProcessMode mode) override;

    void doSelfProcess(float* buffer, samples_t samplesPerChannel) override;

    void processTrackChannels(size_t outBufferSize, size_t samplesPerChannel);
    void mixOutputFromChannel(float* outBuffer, const float* inBuffer, unsigned int samplesCount) const;
<<<<<<< HEAD
    void prepareAuxBuffers(size_t outBufferSize);
=======
>>>>>>> 311457d45c ([audio] replaced mixchannel on trackchain)
    void writeTrackToAuxBuffers(const float* trackBuffer, size_t outBufferSize, const AuxSendsParams& auxSends);
    void processAuxChannels(float* buffer, samples_t samplesPerChannel);
    void processMasterFx(float* buffer, samples_t samplesPerChannel);

    bool useMultithreading() const;

    void notifyAboutAudioSignalChanges();
    void notifyNoAudioSignal();

    TaskScheduler* m_taskScheduler = nullptr;

    size_t m_nonMutedTrackCount = 0;

    AudioOutputParams m_masterParams;
    async::Channel<AudioOutputParams> m_masterOutputParamsChanged;
    FxChainPtr m_masterFxChain;

    struct TrackData {
        TrackId trackId;
        TrackChainPtr chain;
        std::vector<float> buffer;
        bool processed = false;
    };

    std::vector<TrackData> m_tracks;
    std::vector<TrackData> m_auxTracks;
    std::map<TrackId, AuxSendsParams> m_auxSends;

    std::unordered_set<TrackId> m_tracksToProcessWhenIdle;

    std::shared_ptr<IPlayhead> m_playhead;

    bool m_chainProcessing = false;
    SignalNodePtr m_signalNode;
    ControlNodePtr m_controlNode;

    bool m_isIdle = false;
};

using MixerPtr = std::shared_ptr<Mixer>;
}
