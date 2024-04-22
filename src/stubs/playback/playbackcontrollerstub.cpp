/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore Limited
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
#include "playbackcontrollerstub.h"

using namespace mu::playback;
using namespace muse::actions;

bool PlaybackControllerStub::isPlayAllowed() const
{
    return false;
}

muse::async::Notification PlaybackControllerStub::isPlayAllowedChanged() const
{
    return muse::async::Notification();
}

bool PlaybackControllerStub::isPlaying() const
{
    return false;
}

muse::async::Notification PlaybackControllerStub::isPlayingChanged() const
{
    return muse::async::Notification();
}

void PlaybackControllerStub::seek(const muse::midi::tick_t)
{
}

void PlaybackControllerStub::seek(const muse::audio::msecs_t)
{
}

void PlaybackControllerStub::reset()
{
}

muse::async::Notification PlaybackControllerStub::playbackPositionChanged() const
{
    return muse::async::Notification();
}

muse::async::Channel<uint32_t> PlaybackControllerStub::midiTickPlayed() const
{
    return muse::async::Channel<uint32_t>();
}

float PlaybackControllerStub::playbackPositionInSeconds() const
{
    return 0.f;
}

muse::audio::TrackSequenceId PlaybackControllerStub::currentTrackSequenceId() const
{
    return 0;
}

muse::async::Notification PlaybackControllerStub::currentTrackSequenceIdChanged() const
{
    return muse::async::Notification();
}

const IPlaybackController::InstrumentTrackIdMap& PlaybackControllerStub::instrumentTrackIdMap() const
{
    static const InstrumentTrackIdMap m;
    return m;
}

const IPlaybackController::AuxTrackIdMap& PlaybackControllerStub::auxTrackIdMap() const
{
    static const AuxTrackIdMap m;
    return m;
}

muse::async::Channel<muse::audio::TrackId> PlaybackControllerStub::trackAdded() const
{
    return {};
}

muse::async::Channel<muse::audio::TrackId> PlaybackControllerStub::trackRemoved() const
{
    return {};
}

std::string PlaybackControllerStub::auxChannelName(muse::audio::aux_channel_idx_t) const
{
    return "";
}

muse::async::Channel<muse::audio::aux_channel_idx_t, std::string> PlaybackControllerStub::auxChannelNameChanged() const
{
    return {};
}

muse::async::Promise<muse::audio::SoundPresetList> PlaybackControllerStub::availableSoundPresets(const engraving::InstrumentTrackId&) const
{
    return muse::async::Promise<muse::audio::SoundPresetList>([](auto /*resolve*/, auto reject) {
        return reject(int(muse::Ret::Code::UnknownError), "stub");
    });
}

mu::notation::INotationSoloMuteState::SoloMuteState PlaybackControllerStub::trackSoloMuteState(const engraving::InstrumentTrackId&) const
{
    return notation::INotationSoloMuteState::SoloMuteState();
}

void PlaybackControllerStub::setTrackSoloMuteState(const engraving::InstrumentTrackId&,
                                                   const notation::INotationSoloMuteState::SoloMuteState&) const
{
}

void PlaybackControllerStub::playElements(const std::vector<const notation::EngravingItem*>&)
{
}

void PlaybackControllerStub::playMetronome(int)
{
}

void PlaybackControllerStub::seekElement(const notation::EngravingItem*)
{
}

bool PlaybackControllerStub::actionChecked(const ActionCode&) const
{
    return false;
}

muse::async::Channel<ActionCode> PlaybackControllerStub::actionCheckedChanged() const
{
    return {};
}

QTime PlaybackControllerStub::totalPlayTime() const
{
    return {};
}

muse::async::Notification PlaybackControllerStub::totalPlayTimeChanged() const
{
    return {};
}

mu::notation::Tempo PlaybackControllerStub::currentTempo() const
{
    return {};
}

muse::async::Notification PlaybackControllerStub::currentTempoChanged() const
{
    return {};
}

mu::notation::MeasureBeat PlaybackControllerStub::currentBeat() const
{
    return {};
}

muse::audio::msecs_t PlaybackControllerStub::beatToMilliseconds(int, int) const
{
    return 0;
}

double PlaybackControllerStub::tempoMultiplier() const
{
    return 1.0;
}

void PlaybackControllerStub::setTempoMultiplier(double)
{
}

muse::Progress PlaybackControllerStub::loadingProgress() const
{
    return {};
}

void PlaybackControllerStub::applyProfile(const SoundProfileName&)
{
}

void PlaybackControllerStub::setNotation(notation::INotationPtr)
{
}

void PlaybackControllerStub::setIsExportingAudio(bool)
{
}
