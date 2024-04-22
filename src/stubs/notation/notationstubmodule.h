/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2023 MuseScore Limited
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
#ifndef MU_NOTATION_NOTATIONSTUBMODULE_H
#define MU_NOTATION_NOTATIONSTUBMODULE_H

#include "modularity/imodulesetup.h"

namespace mu::notation {
class NotationModule : public modularity::IModuleSetup
{
public:
    NotationModule() = default;

    std::string moduleName() const override;
    void registerExports() override;
};
}

#endif // MU_NOTATION_NOTATIONSTUBMODULE_H
