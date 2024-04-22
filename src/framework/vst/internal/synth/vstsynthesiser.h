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

#ifndef MUSE_VST_VSTSYNTHESISER_H
#define MUSE_VST_VSTSYNTHESISER_H

#include <memory>

#include "audio/internal/abstractsynthesizer.h"
#include "audio/iaudioconfiguration.h"
#include "audio/audiotypes.h"
#include "modularity/ioc.h"
#include "mpe/events.h"

#include "internal/vstaudioclient.h"
#include "ivstpluginsregister.h"
#include "vstsequencer.h"
#include "vsttypes.h"

namespace muse::vst {
class VstSynthesiser : public muse::audio::synth::AbstractSynthesizer
{
    INJECT(IVstPluginsRegister, pluginsRegister)
    INJECT(muse::audio::IAudioConfiguration, config)

public:
    explicit VstSynthesiser(const muse::audio::TrackId trackId, const muse::audio::AudioInputParams& params);
    ~VstSynthesiser() override;

    void init();

    bool isValid() const override;

    muse::audio::AudioSourceType type() const override;
    std::string name() const override;

    void revokePlayingNotes() override;
    void flushSound() override;

    void setupSound(const mpe::PlaybackSetupData& setupData) override;
    void setupEvents(const mpe::PlaybackData& playbackData) override;

    bool isActive() const override;
    void setIsActive(const bool isActive) override;

    muse::audio::msecs_t playbackPosition() const override;
    void setPlaybackPosition(const muse::audio::msecs_t newPosition) override;

    // IAudioSource
    void setSampleRate(unsigned int sampleRate) override;
    unsigned int audioChannelsCount() const override;
    async::Channel<unsigned int> audioChannelsCountChanged() const override;
    muse::audio::samples_t process(float* buffer, muse::audio::samples_t samplesPerChannel) override;

private:
    void toggleVolumeGain(const bool isActive);

    VstPluginPtr m_pluginPtr = nullptr;

    std::unique_ptr<VstAudioClient> m_vstAudioClient = nullptr;

    async::Channel<unsigned int> m_streamsCountChanged;
    muse::audio::samples_t m_samplesPerChannel = 0;

    VstSequencer m_sequencer;

    muse::audio::TrackId m_trackId = muse::audio::INVALID_TRACK_ID;
};

using VstSynthPtr = std::shared_ptr<VstSynthesiser>;
}

#endif // MUSE_VST_VSTSYNTHESISER_H
