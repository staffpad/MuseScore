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
#ifndef MUSE_AUDIO_AUDIOCONFIGURATION_H
#define MUSE_AUDIO_AUDIOCONFIGURATION_H

#include "global/modularity/ioc.h"
#include "global/io/ifilesystem.h"
#include "global/iglobalconfiguration.h"

#include "../iaudioconfiguration.h"

namespace muse::audio {
class AudioConfiguration : public IAudioConfiguration
{
    Inject<IGlobalConfiguration> globalConfiguration;
    Inject<io::IFileSystem> fileSystem;

public:
    AudioConfiguration() = default;

    void init();

    std::vector<std::string> availableAudioApiList() const override;

    std::string currentAudioApi() const override;
    void setCurrentAudioApi(const std::string& name) override;

    std::string audioOutputDeviceId() const override;
    void setAudioOutputDeviceId(const std::string& deviceId) override;
    async::Notification audioOutputDeviceIdChanged() const override;

    audioch_t audioChannelsCount() const override;

    unsigned int driverBufferSize() const override;
    void setDriverBufferSize(unsigned int size) override;
    async::Notification driverBufferSizeChanged() const override;
    samples_t renderStep() const override;

    unsigned int sampleRate() const override;
    void setSampleRate(unsigned int sampleRate) override;
    async::Notification sampleRateChanged() const override;

    size_t minTrackCountForMultithreading() const override;

    // synthesizers
    AudioInputParams defaultAudioInputParams() const override;

    io::paths_t soundFontDirectories() const override;
    io::paths_t userSoundFontDirectories() const override;
    void setUserSoundFontDirectories(const io::paths_t& paths) override;
    async::Channel<io::paths_t> soundFontDirectoriesChanged() const override;

    io::path_t knownAudioPluginsFilePath() const override;

private:
    async::Channel<io::paths_t> m_soundFontDirsChanged;

    async::Notification m_audioOutputDeviceIdChanged;
    async::Notification m_driverBufferSizeChanged;
    async::Notification m_driverSampleRateChanged;
};
}

#endif // MUSE_AUDIO_AUDIOCONFIGURATION_H
