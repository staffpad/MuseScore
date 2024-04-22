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
#ifndef MU_ENGRAVING_DEBUGPAINT_STABLE_H
#define MU_ENGRAVING_DEBUGPAINT_STABLE_H

#include "draw/painter.h"

#include "modularity/ioc.h"
#include "iengravingconfiguration.h"
#include "../devtools/iengravingelementsprovider.h"

namespace mu::engraving {
class EngravingItem;
class Page;
}

namespace mu::engraving::rendering::stable {
class PaintDebugger;
class DebugPaint
{
    INJECT_STATIC(IEngravingConfiguration, configuration)
    INJECT_STATIC(IEngravingElementsProvider, elementsProvider)

public:
    static void paintElementDebug(muse::draw::Painter& painter, const EngravingItem* item);
    static void paintPageDebug(muse::draw::Painter& painter, const Page* page, const std::vector<EngravingItem*>& items);

    static void paintPageTree(muse::draw::Painter& painter, const Page* page);
    static void paintTreeElement(muse::draw::Painter& painter, const EngravingItem* item);
};
}

#endif // MU_ENGRAVING_DEBUGPAINT_STABLE_H
