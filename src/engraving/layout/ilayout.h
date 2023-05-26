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
#ifndef MU_ENGRAVING_ILAYOUT_H
#define MU_ENGRAVING_ILAYOUT_H

#include "modularity/imoduleinterface.h"

#include "layoutoptions.h"

namespace mu::engraving {
class Score;
class Arpeggio;
class Box;
class BarLine;
class Bracket;
class LedgerLine;
class FiguredBassItem;
}

namespace mu::engraving::layout {
class ILayout : MODULE_INTERNAL_INTERFACE
{
    INTERFACE_ID(IEngravingLayout)

public:
    virtual ~ILayout() = default;

    // Layout Score
    virtual void layoutRange(Score* score, const LayoutOptions& options, const Fraction&, const Fraction&) = 0;

    // Layout Elements on Edit
    virtual void layoutOnEditDrag(Arpeggio* item) = 0;
    virtual void layoutOnEdit(Arpeggio* item) = 0;

    virtual void layoutOnEditDrag(Box* item) = 0;
    virtual void layoutOnEndEdit(Box* item) = 0;

    virtual void layoutOnEditDrag(Bracket* item) = 0;

    // Layout Elements on Drop and Drag
    virtual void layoutOnChordRestDrop(BarLine* item) = 0;

    // Layout others
    //! TODO Need to find out why
    virtual void layoutOnAddLedgerLines(LedgerLine* item) = 0;
    virtual void regenerateDisplayText(FiguredBassItem* item) = 0;
};
}

#endif // MU_ENGRAVING_ILAYOUT_H
