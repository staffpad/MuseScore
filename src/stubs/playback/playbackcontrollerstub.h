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
#ifndef MU_PLAYBACK_PLAYBACKCONTROLLERSTUB_H
#define MU_PLAYBACK_PLAYBACKCONTROLLERSTUB_H

#include "playback/iplaybackcontroller.h"

namespace mu::playback {
class PlaybackControllerStub : public IPlaybackController
{
public:
    bool isPlayAllowed() const override;
    muse::async::Notification isPlayAllowedChanged() const override;

    bool isPlaying() const override;
    muse::async::Notification isPlayingChanged() const override;

    void seek(const muse::midi::tick_t tick) override;
    void seek(const muse::audio::msecs_t msecs) override;
    void reset() override;

    muse::async::Notification playbackPositionChanged() const override;
    muse::async::Channel<uint32_t> midiTickPlayed() const override;
    float playbackPositionInSeconds() const override;

    muse::audio::TrackSequenceId currentTrackSequenceId() const override;
    muse::async::Notification currentTrackSequenceIdChanged() const override;

    const InstrumentTrackIdMap& instrumentTrackIdMap() const override;
    const AuxTrackIdMap& auxTrackIdMap() const override;

    muse::async::Channel<muse::audio::TrackId> trackAdded() const override;
    muse::async::Channel<muse::audio::TrackId> trackRemoved() const override;

    std::string auxChannelName(muse::audio::aux_channel_idx_t index) const override;
    muse::async::Channel<muse::audio::aux_channel_idx_t, std::string> auxChannelNameChanged() const override;

    muse::async::Promise<muse::audio::SoundPresetList> availableSoundPresets(
        const engraving::InstrumentTrackId& instrumentTrackId) const override;

    notation::INotationSoloMuteState::SoloMuteState trackSoloMuteState(const engraving::InstrumentTrackId& trackId) const override;
    void setTrackSoloMuteState(const engraving::InstrumentTrackId& trackId,
                               const notation::INotationSoloMuteState::SoloMuteState& state) const override;

    void playElements(const std::vector<const notation::EngravingItem*>& elements) override;
    void playMetronome(int tick) override;
    void seekElement(const notation::EngravingItem* element) override;

    bool actionChecked(const muse::actions::ActionCode& actionCode) const override;
    muse::async::Channel<muse::actions::ActionCode> actionCheckedChanged() const override;

    QTime totalPlayTime() const override;
    muse::async::Notification totalPlayTimeChanged() const override;

    notation::Tempo currentTempo() const override;
    muse::async::Notification currentTempoChanged() const override;

    notation::MeasureBeat currentBeat() const override;
    muse::audio::msecs_t beatToMilliseconds(int measureIndex, int beatIndex) const override;

    double tempoMultiplier() const override;
    void setTempoMultiplier(double multiplier) override;

    muse::Progress loadingProgress() const override;

    void applyProfile(const SoundProfileName& profileName) override;

    void setNotation(notation::INotationPtr notation) override;
    void setIsExportingAudio(bool exporting) override;
};
}

#endif // MU_PLAYBACK_PLAYBACKCONTROLLERSTUB_H
