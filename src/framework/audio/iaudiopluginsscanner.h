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
#ifndef MUSE_AUDIO_IAUDIOPLUGINSSCANNER_H
#define MUSE_AUDIO_IAUDIOPLUGINSSCANNER_H

#include <memory>

#include "global/io/path.h"

namespace muse::audio {
class IAudioPluginsScanner
{
public:
    virtual ~IAudioPluginsScanner() = default;

    virtual io::paths_t scanPlugins() const = 0;
};

using IAudioPluginsScannerPtr = std::shared_ptr<IAudioPluginsScanner>;
}

#endif // MUSE_AUDIO_IAUDIOPLUGINSSCANNER_H
