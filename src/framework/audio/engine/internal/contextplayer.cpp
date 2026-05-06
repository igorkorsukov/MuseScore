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

#include "contextplayer.h"

#include "audio/common/audiosanitizer.h"
#include "audio/common/audioerrors.h"

#include "log.h"

using namespace muse;
using namespace muse::audio;
using namespace muse::audio::engine;
using namespace muse::async;

ContextPlayer::ContextPlayer(IGetTrackSource* getTracks, IExecOperation* execOperation)
    : m_trackSource(getTracks)
    , m_execOperation(execOperation)
{
    m_status.set(PlaybackStatus::Stopped);
    m_isActive.set(false);

    m_status.ch.onReceive(this, [this](const PlaybackStatus status) {
        onStatusChanged(status);
    });

    // Forwarding events from the processing thread to the engine thread
    m_timeEvent.onReceive(this, [this](const TimeEvent event) {
        onTimeEvent(event);
    });
}

void ContextPlayer::exec(OperationType type, const Operation& func)
{
    ONLY_AUDIO_ENGINE_THREAD;
    if (m_execOperation) {
        m_execOperation->execOperation(type, func);
    } else {
        func();
    }
}

void ContextPlayer::onStatusChanged(const PlaybackStatus status)
{
    const bool active = status == PlaybackStatus::Running;
    if (active) {
        //! NOTE If there is no countdown, activate the mixer.
        //! Otherwise, it will become active when the countdown ends.
        if (m_countDown.is_zero()) {
            m_isActive.set(true);
        }
    } else {
        m_isActive.set(false);
        flushAllTracks();
    }
}

void ContextPlayer::forward(const TimePosition& delta)
{
    ONLY_AUDIO_PROC_THREAD;

    // Check: Active
    if (m_status.val != PlaybackStatus::Running) {
        return;
    }

    const TimePosition newTime = proc_onTimeChanged(delta);

    if (m_currentPosition == newTime) {
        return;
    }

    m_currentPosition = newTime;
    m_timeChanged.send(m_currentPosition.time());
}

TimePosition ContextPlayer::proc_onTimeChanged(const TimePosition& delta)
{
    ONLY_AUDIO_PROC_THREAD;

    // Check: Count down
    if (!m_countDown.is_zero()) {
        m_countDown -= delta.time();

        if (m_countDown > 0.) {
            return m_currentPosition; // no change
        }

        m_countDown = 0.;
        m_timeEvent.send(TimeEvent { TimeEventType::CountDownEnded, m_currentPosition }); // forwarding an event to the engine thread
    }

    // Check: Loop
    const TimePosition newTime = m_currentPosition.forwarded(delta);
    if (m_timeLoopStart < m_timeLoopEnd && newTime.time() >= m_timeLoopEnd) {
        //! TODO Seek may be necessary to call this directly within the PROC thread.
        m_timeEvent.send(TimeEvent { TimeEventType::LoopEnded, newTime }); // forwarding an event to the engine thread
        const secs_t overshoot = newTime.time() - m_timeLoopEnd;
        return TimePosition::fromTime(m_timeLoopStart + overshoot, delta.sampleRate());
    }

    // Check: Duration
    if (newTime.time() >= m_timeDuration) {
        m_timeEvent.send(TimeEvent { TimeEventType::PlaybackEnded, newTime }); // forwarding an event to the engine thread
        return TimePosition::fromTime(m_timeDuration, delta.sampleRate());
    }

    return newTime;
}

const TimePosition& ContextPlayer::currentPosition() const
{
    return m_currentPosition;
}

void ContextPlayer::onTimeEvent(const TimeEvent event)
{
    ONLY_AUDIO_ENGINE_THREAD;

    switch (event.type) {
    case TimeEventType::CountDownEnded:
        exec(OperationType::QuickOperation, [this]() {
            m_isActive.set(m_status.val == PlaybackStatus::Running);
        });
        break;
    case TimeEventType::LoopEnded:
        exec(OperationType::QuickOperation, [this, event]() {
            seekAllTracks(event.position);
        });
        break;
    case TimeEventType::PlaybackEnded:
        exec(OperationType::QuickOperation, [this]() {
            pause();
        });
        break;
    default:
        break;
    }
}

async::Promise<Ret> ContextPlayer::prepareToPlay()
{
    ONLY_AUDIO_ENGINE_THREAD;

    return async::make_promise<Ret>([this](auto resolve, auto) {
        prepareAllTracksToPlay([resolve]() {
            (void)resolve(make_ok());
        });

        return Promise<Ret>::dummy_result();
    });
}

void ContextPlayer::play(const secs_t delay)
{
    ONLY_AUDIO_ENGINE_THREAD;
    ONLY_ON_OPERATION_EXEC;

    if (playbackStatus() == PlaybackStatus::Running) {
        return;
    }

    m_countDown = delay;
    m_status.set(PlaybackStatus::Running);
}

void ContextPlayer::seek(const TimePosition& position, const bool flushSound)
{
    ONLY_AUDIO_ENGINE_THREAD;
    ONLY_ON_OPERATION_EXEC;

    //! NOTE During export, the current time does not change, it remains at 0
    // but a seek operation is still required to reset the internal state of the sources (synthesizers).
    // if (newPosition == m_currentPosition.time()) {
    //     return;
    // }

    IF_ASSERT_FAILED(position.isValid()) {
        return;
    }

    IF_ASSERT_FAILED(m_trackSource) {
        return;
    }

    m_flushSoundOnSeek = flushSound;
    m_currentPosition = position;
    m_timeChanged.send(m_currentPosition.time());
    seekAllTracks(position);
    m_flushSoundOnSeek = true;
}

void ContextPlayer::stop()
{
    ONLY_AUDIO_ENGINE_THREAD;
    ONLY_ON_OPERATION_EXEC;

    if (playbackStatus() == PlaybackStatus::Stopped) {
        return;
    }

    m_status.set(PlaybackStatus::Stopped);
    m_countDown = 0.;
    seek(TimePosition::zero(m_currentPosition.sampleRate()));
    m_notYetReadyToPlayTracks.clear();
}

void ContextPlayer::pause()
{
    ONLY_AUDIO_ENGINE_THREAD;
    ONLY_ON_OPERATION_EXEC;

    if (playbackStatus() == PlaybackStatus::Paused) {
        return;
    }

    m_status.set(PlaybackStatus::Paused);
    m_notYetReadyToPlayTracks.clear();
}

void ContextPlayer::resume(const secs_t delay)
{
    ONLY_AUDIO_ENGINE_THREAD;
    ONLY_ON_OPERATION_EXEC;

    if (playbackStatus() == PlaybackStatus::Running) {
        return;
    }

    m_countDown = delay;
    seek(m_currentPosition);
    m_status.set(PlaybackStatus::Running);
}

secs_t ContextPlayer::duration() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_timeDuration;
}

void ContextPlayer::setDuration(const secs_t duration)
{
    ONLY_AUDIO_ENGINE_THREAD;
    m_timeDuration = duration;
}

Ret ContextPlayer::setLoop(const secs_t from, const secs_t to)
{
    ONLY_AUDIO_ENGINE_THREAD;
    ONLY_ON_OPERATION_EXEC;

    if (from >= to) {
        return make_ret(Err::InvalidTimeLoop);
    }

    m_timeLoopStart = from;
    m_timeLoopEnd = to;

    return Ret(Ret::Code::Ok);
}

void ContextPlayer::resetLoop()
{
    ONLY_AUDIO_ENGINE_THREAD;
    ONLY_ON_OPERATION_EXEC;
    m_timeLoopStart = 0;
    m_timeLoopEnd = 0;
}

secs_t ContextPlayer::playbackPosition() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_currentPosition.time();
}

Channel<secs_t> ContextPlayer::playbackPositionChanged() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_timeChanged;
}

PlaybackStatus ContextPlayer::playbackStatus() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_status.val;
}

Channel<PlaybackStatus> ContextPlayer::playbackStatusChanged() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_status.ch;
}

bool ContextPlayer::isActive() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_isActive.val;
}

Channel<bool> ContextPlayer::isActiveChanged() const
{
    ONLY_AUDIO_ENGINE_THREAD;
    return m_isActive.ch;
}

void ContextPlayer::seekAllTracks(const TimePosition& position)
{
    ONLY_AUDIO_ENGINE_THREAD;
    ONLY_ON_OPERATION_EXEC;

    IF_ASSERT_FAILED(m_trackSource) {
        return;
    }

    for (const auto& source : m_trackSource->allTracksSources()) {
        source->seek(position, m_flushSoundOnSeek);
    }
}

void ContextPlayer::flushAllTracks()
{
    ONLY_AUDIO_ENGINE_THREAD;
    ONLY_ON_OPERATION_EXEC;

    IF_ASSERT_FAILED(m_trackSource) {
        return;
    }

    for (const auto& source : m_trackSource->allTracksSources()) {
        source->flush();
    }
}

void ContextPlayer::prepareAllTracksToPlay(AllTracksReadyCallback allTracksReadyCallback)
{
    ONLY_AUDIO_ENGINE_THREAD;

    IF_ASSERT_FAILED(m_trackSource) {
        return;
    }

    m_notYetReadyToPlayTracks.clear();

    for (const auto& source : m_trackSource->allTracksSources()) {
        IF_ASSERT_FAILED(source) {
            continue;
        }

        IF_ASSERT_FAILED(source->mode() == ProcessMode::Idle) {
            continue;
        }

        source->prepareToPlay();

        if (!source->readyToPlay()) {
            m_notYetReadyToPlayTracks.insert(source);
        }
    }

    if (m_notYetReadyToPlayTracks.empty()) {
        allTracksReadyCallback();
        return;
    }

    for (const auto& source : m_notYetReadyToPlayTracks) {
        std::weak_ptr<AudioSourceNode> weakPtr = source;
        source->readyToPlayChanged().onNotify(this, [this, weakPtr, allTracksReadyCallback]() {
            if (auto source = weakPtr.lock()) {
                muse::remove(m_notYetReadyToPlayTracks, source);

                if (m_notYetReadyToPlayTracks.empty()) {
                    allTracksReadyCallback();
                }

                source->readyToPlayChanged().disconnect(this);
            }
        }, Asyncable::Mode::SetReplace);
    }
}
