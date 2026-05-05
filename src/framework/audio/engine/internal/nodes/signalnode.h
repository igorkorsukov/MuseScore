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

#pragma once

#include "audionode.h"

namespace muse::audio {
struct SignalTag
{
    static constexpr const char* name = "Signal";
};
}

namespace muse::audio::engine {
class SignalNode : public AudioNode<SignalTag>
{
public:

    bool isSilent() const;
    AudioSignalChanges audioSignalChanges() const;
    void notifyAboutChanges();

protected:

    void onOutputSpecChanged(const OutputSpec& spec) override;

    void doSelfProcess(float* buffer, samples_t samplesPerChannel) override;

    void updateSignalValue(const audioch_t ch, const float newPeak);

    std::vector<float> m_channelPeaks;
    bool m_isSilent = true;
    AudioSignalValuesMap m_signalValuesMap;
    bool m_shouldNotifyAboutChanges = false;

    //! NOTE It would be nice if the driver callback was called in one thread.
    //! But some drivers, for example PipeWire, use queues
    //! And then the callback can be called in different threads.
    //! If a score is open, we will change the audio driver
    //! then the number of threads used may increase...
    //! Channels allow 10 threads by default. Here we're increasing that to the maximum...
    //! If this is not enough, then we need to make sure that the callback is called in one thread,
    //! or use something else here instead of channels, some kind of queues.
    AudioSignalChanges m_audioSignalChanges = AudioSignalChanges(async::makeOpt()
                                                                 .name("audio::audioSignalChanges")
                                                                 .threads(100)
                                                                 .disableWaitPendingsOnSend());
};

using SignalNodePtr = std::shared_ptr<SignalNode>;
}
