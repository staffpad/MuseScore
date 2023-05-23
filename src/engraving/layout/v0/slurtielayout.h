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
#ifndef MU_ENGRAVING_SLURTIELAYOUT_H
#define MU_ENGRAVING_SLURTIELAYOUT_H

#include "layoutcontext.h"

namespace mu::engraving {
class Slur;
struct SlurPos;
class SpannerSegment;
class System;
class Chord;
class TieSegment;
class Tie;
}

namespace mu::engraving::layout::v0 {
class SlurTieLayout
{
public:
    static void layout(Slur* item, LayoutContext& ctx);
    static SpannerSegment* layoutSystem(Slur* item, System* system, LayoutContext& ctx);

    static TieSegment* tieLayoutFor(Tie* item, System* system);
    static TieSegment* tieLayoutBack(Tie* item, System* system);

private:
    static void slurPos(Slur* item, SlurPos* sp, LayoutContext& ctx);
    static void fixArticulations(Slur* item, PointF& pt, Chord* c, double up, bool stemSide);

    static void tiePos(Tie* item, SlurPos* sp);
};
}

#endif // MU_ENGRAVING_SLURTIELAYOUT_H
