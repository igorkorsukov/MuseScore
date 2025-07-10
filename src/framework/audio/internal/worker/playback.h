/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
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
#ifndef MUSE_AUDIO_SEQUENCER_H
#define MUSE_AUDIO_SEQUENCER_H

#include <map>

#include "modularity/ioc.h"
#include "global/async/asyncable.h"

#include "iplayer.h"
#include "itracks.h"
#include "iaudiooutput.h"
#include "igettracksequence.h"
#include "iplayback.h"

namespace muse::audio {
class Playback : public IPlayback, public IGetTrackSequence, public Injectable, public async::Asyncable
{
public:
    Playback(const muse::modularity::ContextPtr& iocCtx)
        : Injectable(iocCtx) {}

    void init() override;
    void deinit() override;
    bool isInited() const override;

    // IPlayback
    async::Promise<TrackSequenceId> addSequence() override;
    async::Promise<TrackSequenceIdList> sequenceIdList() const override;
    void removeSequence(const TrackSequenceId id) override;

    async::Channel<TrackSequenceId> sequenceAdded() const override;
    async::Channel<TrackSequenceId> sequenceRemoved() const override;

    // ITracks
    async::Promise<TrackIdList> trackIdList(const TrackSequenceId sequenceId) const override;
    async::Promise<TrackName> trackName(const TrackSequenceId sequenceId, const TrackId trackId) const override;

    async::Promise<TrackId, AudioParams> addTrack(const TrackSequenceId sequenceId, const std::string& trackName,
                                                  io::IODevice* playbackData, AudioParams&& params) override;
    async::Promise<TrackId, AudioParams> addTrack(const TrackSequenceId sequenceId, const std::string& trackName,
                                                  const mpe::PlaybackData& playbackData, AudioParams&& params) override;

    async::Promise<TrackId, AudioOutputParams> addAuxTrack(const TrackSequenceId sequenceId, const std::string& trackName,
                                                           const AudioOutputParams& outputParams) override;

    void removeTrack(const TrackSequenceId sequenceId, const TrackId trackId) override;
    void removeAllTracks(const TrackSequenceId sequenceId) override;

    async::Channel<TrackSequenceId, TrackId> trackAdded() const override;
    async::Channel<TrackSequenceId, TrackId> trackRemoved() const override;

    async::Promise<AudioResourceMetaList> availableInputResources() const override;
    async::Promise<SoundPresetList> availableSoundPresets(const AudioResourceMeta& resourceMeta) const override;

    async::Promise<AudioInputParams> inputParams(const TrackSequenceId sequenceId, const TrackId trackId) const override;
    void setInputParams(const TrackSequenceId sequenceId, const TrackId trackId, const AudioInputParams& params) override;
    async::Channel<TrackSequenceId, TrackId, AudioInputParams> inputParamsChanged() const override;

    void clearSources() override;
    // ===

    IPlayerPtr player(const TrackSequenceId id) const override;
    IAudioOutputPtr audioOutput() const override;

protected:
    // IGetTrackSequence
    ITrackSequencePtr sequence(const TrackSequenceId id) const override;

private:
    ITracksPtr m_trackHandlersPtr = nullptr;
    IAudioOutputPtr m_audioOutputPtr = nullptr;

    std::map<TrackSequenceId, ITrackSequencePtr> m_sequences;

    async::Channel<TrackSequenceId> m_sequenceAdded;
    async::Channel<TrackSequenceId> m_sequenceRemoved;
};
}

#endif // MUSE_AUDIO_SEQUENCER_H
