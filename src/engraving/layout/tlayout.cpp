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

#include "tlayout.h"

#include "draw/fontmetrics.h"

#include "../infrastructure/rtti.h"

#include "../iengravingfont.h"
#include "../types/typesconv.h"
#include "../types/symnames.h"
#include "../libmscore/score.h"
#include "../libmscore/utils.h"

#include "../libmscore/accidental.h"
#include "../libmscore/actionicon.h"
#include "../libmscore/ambitus.h"
#include "../libmscore/arpeggio.h"
#include "../libmscore/articulation.h"

#include "../libmscore/bagpembell.h"
#include "../libmscore/barline.h"
#include "../libmscore/beam.h"
#include "../libmscore/bend.h"
#include "../libmscore/box.h"
#include "../libmscore/bracket.h"
#include "../libmscore/breath.h"

#include "../libmscore/chord.h"
#include "../libmscore/chordline.h"
#include "../libmscore/clef.h"

#include "../libmscore/deadslapped.h"
#include "../libmscore/dynamic.h"

#include "../libmscore/expression.h"

#include "../libmscore/fermata.h"
#include "../libmscore/figuredbass.h"
#include "../libmscore/fingering.h"
#include "../libmscore/fret.h"
#include "../libmscore/fretcircle.h"

#include "../libmscore/glissando.h"
#include "../libmscore/gradualtempochange.h"

#include "../libmscore/hairpin.h"
#include "../libmscore/harmonicmark.h"
#include "../libmscore/harmony.h"
#include "../libmscore/hook.h"

#include "../libmscore/image.h"
#include "../libmscore/instrchange.h"

#include "../libmscore/jump.h"

#include "../libmscore/keysig.h"

#include "../libmscore/note.h"

#include "../libmscore/part.h"

#include "../libmscore/rest.h"

#include "../libmscore/staff.h"
#include "../libmscore/stem.h"
#include "../libmscore/system.h"

#include "../libmscore/text.h"
#include "../libmscore/textframe.h"

#include "beamlayout.h"
#include "chordlayout.h"

using namespace mu::draw;
using namespace mu::engraving;
using namespace mu::engraving::v0;

void TLayout::layout(Accidental* item, LayoutContext& ctx)
{
    item->clearElements();

    // TODO: remove Accidental in layout()
    // don't show accidentals for tab or slash notation
    if (item->onTabStaff() || (item->note() && item->note()->fixed())) {
        item->setbbox(RectF());
        return;
    }

    double m = item->explicitParent() ? item->parentItem()->mag() : 1.0;
    if (item->isSmall()) {
        m *= item->score()->styleD(Sid::smallNoteMag);
    }
    item->setMag(m);

    // if the accidental is standard (doubleflat, flat, natural, sharp or double sharp)
    // and it has either no bracket or parentheses, then we have glyphs straight from smufl.
    if (item->bracket() == AccidentalBracket::NONE
        || (item->bracket() == AccidentalBracket::PARENTHESIS
            && (item->accidentalType() == AccidentalType::FLAT
                || item->accidentalType() == AccidentalType::NATURAL
                || item->accidentalType() == AccidentalType::SHARP
                || item->accidentalType() == AccidentalType::SHARP2
                || item->accidentalType() == AccidentalType::FLAT2))) {
        layoutSingleGlyphAccidental(item, ctx);
    } else {
        layoutMultiGlyphAccidental(item, ctx);
    }
}

void TLayout::layoutSingleGlyphAccidental(Accidental* item, LayoutContext& ctx)
{
    RectF r;

    SymId s = item->symbol();
    if (item->bracket() == AccidentalBracket::PARENTHESIS) {
        switch (item->accidentalType()) {
        case AccidentalType::FLAT2:
            s = SymId::accidentalDoubleFlatParens;
            break;
        case AccidentalType::FLAT:
            s = SymId::accidentalFlatParens;
            break;
        case AccidentalType::NATURAL:
            s = SymId::accidentalNaturalParens;
            break;
        case AccidentalType::SHARP:
            s = SymId::accidentalSharpParens;
            break;
        case AccidentalType::SHARP2:
            s = SymId::accidentalDoubleSharpParens;
            break;
        default:
            break;
        }
        if (!item->score()->engravingFont()->isValid(s)) {
            layoutMultiGlyphAccidental(item, ctx);
            return;
        }
    }

    SymElement e(s, 0.0, 0.0);
    item->addElement(e);
    r.unite(item->symBbox(s));
    item->setbbox(r);
}

void TLayout::layoutMultiGlyphAccidental(Accidental* item, LayoutContext&)
{
    double margin = item->score()->styleMM(Sid::bracketedAccidentalPadding);
    RectF r;
    double x = 0.0;

    // should always be true
    if (item->bracket() != AccidentalBracket::NONE) {
        SymId id = SymId::noSym;
        switch (item->bracket()) {
        case AccidentalBracket::PARENTHESIS:
            id = SymId::accidentalParensLeft;
            break;
        case AccidentalBracket::BRACKET:
            id = SymId::accidentalBracketLeft;
            break;
        case AccidentalBracket::BRACE:
            id = SymId::accidentalCombiningOpenCurlyBrace;
            break;
        case AccidentalBracket::NONE: // can't happen
            break;
        }
        SymElement se(id, 0.0, item->bracket() == AccidentalBracket::BRACE ? item->spatium() * 0.4 : 0.0);
        item->addElement(se);
        r.unite(item->symBbox(id));
        x += item->symAdvance(id) + margin;
    }

    SymId s = item->symbol();
    SymElement e(s, x, 0.0);
    item->addElement(e);
    r.unite(item->symBbox(s).translated(x, 0.0));

    // should always be true
    if (item->bracket() != AccidentalBracket::NONE) {
        x += item->symAdvance(s) + margin;
        SymId id = SymId::noSym;
        switch (item->bracket()) {
        case AccidentalBracket::PARENTHESIS:
            id = SymId::accidentalParensRight;
            break;
        case AccidentalBracket::BRACKET:
            id = SymId::accidentalBracketRight;
            break;
        case AccidentalBracket::BRACE:
            id = SymId::accidentalCombiningCloseCurlyBrace;
            break;
        case AccidentalBracket::NONE: // can't happen
            break;
        }
        SymElement se(id, x, item->bracket() == AccidentalBracket::BRACE ? item->spatium() * 0.4 : 0.0);
        item->addElement(se);
        r.unite(item->symBbox(id).translated(x, 0.0));
    }
    item->setbbox(r);
}

void TLayout::layout(ActionIcon* item, LayoutContext&)
{
    FontMetrics fontMetrics(item->iconFont());
    item->setbbox(fontMetrics.boundingRect(Char(item->icon())));
}

void TLayout::layout(Ambitus* item, LayoutContext&)
{
    int bottomLine, topLine;
    ClefType clf;
    double headWdt     = item->headWidth();
    Key key;
    double lineDist;
    int numOfLines;
    Segment* segm        = item->segment();
    double _spatium    = item->spatium();
    Staff* stf         = nullptr;
    if (segm && item->track() != mu::nidx) {
        Fraction tick    = segm->tick();
        stf         = item->score()->staff(item->staffIdx());
        lineDist    = stf->lineDistance(tick) * _spatium;
        numOfLines  = stf->lines(tick);
        clf         = stf->clef(tick);
    } else {                              // for use in palettes
        lineDist    = _spatium;
        numOfLines  = 3;
        clf         = ClefType::G;
    }

    //
    // NOTEHEADS Y POS
    //
    // if pitch == INVALID_PITCH or tpc == Tpc::TPC_INVALID, set to some default:
    // for use in palettes and when actual range cannot be calculated (new ambitus or no notes in staff)
    //
    double xAccidOffTop    = 0;
    double xAccidOffBottom = 0;
    if (stf) {
        key = stf->key(segm->tick());
    } else {
        key = Key::C;
    }

    // top notehead
    if (item->topPitch() == INVALID_PITCH || item->topTpc() == Tpc::TPC_INVALID) {
        item->setTopPosY(0.0);  // if uninitialized, set to top staff line
    } else {
        topLine  = absStep(item->topTpc(), item->topPitch());
        topLine  = relStep(topLine, clf);
        item->setTopPosY(topLine * lineDist * 0.5);
        // compute accidental
        AccidentalType accidType;
        // if (13 <= (tpc - key) <= 19) there is no accidental)
        if (item->topTpc() - int(key) >= 13 && item->topTpc() - int(key) <= 19) {
            accidType = AccidentalType::NONE;
        } else {
            AccidentalVal accidVal = tpc2alter(item->topTpc());
            accidType = Accidental::value2subtype(accidVal);
            if (accidType == AccidentalType::NONE) {
                accidType = AccidentalType::NATURAL;
            }
        }
        item->topAccidental()->setAccidentalType(accidType);
        if (accidType != AccidentalType::NONE) {
            item->topAccidental()->layout();
        } else {
            item->topAccidental()->setbbox(RectF());
        }
        item->topAccidental()->setPosY(item->topPos().y());
    }

    // bottom notehead
    if (item->bottomPitch() == INVALID_PITCH || item->bottomTpc() == Tpc::TPC_INVALID) {
        item->setBottomPosY((numOfLines - 1) * lineDist);             // if uninitialized, set to last staff line
    } else {
        bottomLine  = absStep(item->bottomTpc(), item->bottomPitch());
        bottomLine  = relStep(bottomLine, clf);
        item->setBottomPosY(bottomLine * lineDist * 0.5);
        // compute accidental
        AccidentalType accidType;
        if (item->bottomTpc() - int(key) >= 13 && item->bottomTpc() - int(key) <= 19) {
            accidType = AccidentalType::NONE;
        } else {
            AccidentalVal accidVal = tpc2alter(item->bottomTpc());
            accidType = Accidental::value2subtype(accidVal);
            if (accidType == AccidentalType::NONE) {
                accidType = AccidentalType::NATURAL;
            }
        }
        item->bottomAccidental()->setAccidentalType(accidType);
        if (accidType != AccidentalType::NONE) {
            item->bottomAccidental()->layout();
        } else {
            item->bottomAccidental()->setbbox(RectF());
        }
        item->bottomAccidental()->setPosY(item->bottomPos().y());
    }

    //
    // NOTEHEAD X POS
    //
    // Note: manages colliding accidentals
    //
    double accNoteDist = item->point(item->score()->styleS(Sid::accidentalNoteDistance));
    xAccidOffTop      = item->topAccidental()->width() + accNoteDist;
    xAccidOffBottom   = item->bottomAccidental()->width() + accNoteDist;

    // if top accidental extends down more than bottom accidental extends up,
    // AND ambitus is not leaning right, bottom accidental needs to be displaced
    bool collision
        =(item->topAccidental()->ipos().y() + item->topAccidental()->bbox().y() + item->topAccidental()->height()
          > item->bottomAccidental()->ipos().y() + item->bottomAccidental()->bbox().y())
          && item->direction() != DirectionH::RIGHT;
    if (collision) {
        // displace bottom accidental (also attempting to 'undercut' flats)
        xAccidOffBottom = xAccidOffTop
                          + ((item->bottomAccidental()->accidentalType() == AccidentalType::FLAT
                              || item->bottomAccidental()->accidentalType() == AccidentalType::FLAT2
                              || item->bottomAccidental()->accidentalType() == AccidentalType::NATURAL)
                             ? item->bottomAccidental()->width() * 0.5 : item->bottomAccidental()->width());
    }

    switch (item->direction()) {
    case DirectionH::AUTO:                       // noteheads one above the other
        // left align noteheads and right align accidentals 'hanging' on the left
        item->setTopPosX(0.0);
        item->setBottomPosX(0.0);
        item->topAccidental()->setPosX(-xAccidOffTop);
        item->bottomAccidental()->setPosX(-xAccidOffBottom);
        break;
    case DirectionH::LEFT:                       // top notehead at the left of bottom notehead
        // place top notehead at left margin; bottom notehead at right of top head;
        // top accid. 'hanging' on left of top head and bottom accid. 'hanging' at left of bottom head
        item->setTopPosX(0.0);
        item->setBottomPosX(headWdt);
        item->topAccidental()->setPosX(-xAccidOffTop);
        item->bottomAccidental()->setPosX(collision ? -xAccidOffBottom : headWdt - xAccidOffBottom);
        break;
    case DirectionH::RIGHT:                      // top notehead at the right of bottom notehead
        // bottom notehead at left margin; top notehead at right of bottomnotehead
        // top accid. 'hanging' on left of top head and bottom accid. 'hanging' at left of bottom head
        item->setBottomPosX(0.0);
        item->setTopPosX(headWdt);
        item->bottomAccidental()->setPosX(-xAccidOffBottom);
        item->topAccidental()->setPosX(headWdt - xAccidOffTop);
        break;
    }

    // compute line from top note centre to bottom note centre
    LineF fullLine(item->topPos().x() + headWdt * 0.5,
                   item->topPos().y(),
                   item->bottomPos().x() + headWdt * 0.5,
                   item->bottomPos().y());
    // shorten line on each side by offsets
    double yDelta = item->bottomPos().y() - item->topPos().y();
    if (yDelta != 0.0) {
        double off = _spatium * Ambitus::LINEOFFSET_DEFAULT;
        PointF p1 = fullLine.pointAt(off / yDelta);
        PointF p2 = fullLine.pointAt(1 - (off / yDelta));
        item->setLine(LineF(p1, p2));
    } else {
        item->setLine(fullLine);
    }

    RectF headRect(0, -0.5 * _spatium, headWdt, 1 * _spatium);
    item->setbbox(headRect.translated(item->topPos()).united(headRect.translated(item->bottomPos()))
                  .united(item->topAccidental()->bbox().translated(item->topAccidental()->ipos()))
                  .united(item->bottomAccidental()->bbox().translated(item->bottomAccidental()->ipos()))
                  );
}

void TLayout::layout(Arpeggio* item, LayoutContext&)
{
    double top = item->calcTop();
    double bottom = item->calcBottom();
    if (item->score()->styleB(Sid::ArpeggioHiddenInStdIfTab)) {
        if (item->staff() && item->staff()->isPitchedStaff(item->tick())) {
            for (Staff* s : item->staff()->staffList()) {
                if (s->score() == item->score() && s->isTabStaff(item->tick()) && s->visible()) {
                    item->setbbox(RectF());
                    return;
                }
            }
        }
    }
    if (item->staff()) {
        item->setMag(item->staff()->staffMag(item->tick()));
    }
    switch (item->arpeggioType()) {
    case ArpeggioType::NORMAL: {
        item->symbolLine(SymId::wiggleArpeggiatoUp, SymId::wiggleArpeggiatoUp);
        // string is rotated -90 degrees
        RectF r(item->symBbox(item->symbols()));
        item->setbbox(RectF(0.0, -r.x() + top, r.height(), r.width()));
    }
    break;

    case ArpeggioType::UP: {
        item->symbolLine(SymId::wiggleArpeggiatoUpArrow, SymId::wiggleArpeggiatoUp);
        // string is rotated -90 degrees
        RectF r(item->symBbox(item->symbols()));
        item->setbbox(RectF(0.0, -r.x() + top, r.height(), r.width()));
    }
    break;

    case ArpeggioType::DOWN: {
        item->symbolLine(SymId::wiggleArpeggiatoUpArrow, SymId::wiggleArpeggiatoUp);
        // string is rotated +90 degrees (so that UpArrow turns into a DownArrow)
        RectF r(item->symBbox(item->symbols()));
        item->setbbox(RectF(0.0, r.x() + top, r.height(), r.width()));
    }
    break;

    case ArpeggioType::UP_STRAIGHT: {
        double _spatium = item->spatium();
        double x1 = _spatium * .5;
        double w  = item->symBbox(SymId::arrowheadBlackUp).width();
        item->setbbox(RectF(x1 - w * .5, top, w, bottom));
    }
    break;

    case ArpeggioType::DOWN_STRAIGHT: {
        double _spatium = item->spatium();
        double x1 = _spatium * .5;
        double w  = item->symBbox(SymId::arrowheadBlackDown).width();
        item->setbbox(RectF(x1 - w * .5, top, w, bottom));
    }
    break;

    case ArpeggioType::BRACKET: {
        double _spatium = item->spatium();
        double w  = item->score()->styleS(Sid::ArpeggioHookLen).val() * _spatium;
        item->setbbox(RectF(0.0, top, w, bottom));
        break;
    }
    }
}

void TLayout::layout(Articulation* item, LayoutContext&)
{
    item->setSkipDraw(false);
    if (item->isHiddenOnTabStaff()) {
        item->setSkipDraw(true);
        return;
    }

    RectF bRect;

    if (item->textType() != ArticulationTextType::NO_TEXT) {
        mu::draw::Font scaledFont(item->font());
        scaledFont.setPointSizeF(item->font().pointSizeF() * item->magS());
        mu::draw::FontMetrics fm(scaledFont);

        bRect = fm.boundingRect(scaledFont, TConv::text(item->textType()));
    } else {
        bRect = item->symBbox(item->symId());
    }

    item->setbbox(bRect.translated(-0.5 * bRect.width(), 0.0));
}

void TLayout::layout(BagpipeEmbellishment* item, LayoutContext&)
{
    /*
    if (_embelType == 0 || _embelType == 8 || _embelType == 9) {
          LOGD("BagpipeEmbellishment::layout st %d", _embelType);
          }
     */
    SymId headsym = SymId::noteheadBlack;
    SymId flagsym = SymId::flag32ndUp;

    noteList nl = item->getNoteList();
    BagpipeEmbellishment::BEDrawingDataX dx(headsym, flagsym, item->magS(), item->score()->spatium(), static_cast<int>(nl.size()));

    item->setbbox(RectF());
    /*
    if (_embelType == 0 || _embelType == 8 || _embelType == 9) {
          symMetrics("headsym", headsym);
          symMetrics("flagsym", flagsym);
          LOGD("mags %f headw %f headp %f spatium %f xl %f",
                 dx.mags, dx.headw, dx.headp, dx.spatium, dx.xl);
          }
     */

    bool drawFlag = nl.size() == 1;

    // draw the notes including stem, (optional) flag and (optional) ledger line
    double x = dx.xl;
    for (int note : nl) {
        int line = BagpipeEmbellishment::BagpipeNoteInfoList[note].line;
        BagpipeEmbellishment::BEDrawingDataY dy(line, item->score()->spatium());

        // head
        item->addbbox(item->score()->engravingFont()->bbox(headsym, dx.mags).translated(PointF(x - dx.lw * .5 - dx.headw, dy.y2)));
        /*
        if (_embelType == 0 || _embelType == 8 || _embelType == 9) {
              printBBox(" notehead", bbox());
              }
         */

        // stem
        // highest top of stems actually used is y1b
        item->addbbox(RectF(x - dx.lw * .5 - dx.headw, dy.y1b, dx.lw, dy.y2 - dy.y1b));
        /*
        if (_embelType == 0 || _embelType == 8 || _embelType == 9) {
              printBBox(" notehead + stem", bbox());
              }
         */

        // flag
        if (drawFlag) {
            item->addbbox(item->score()->engravingFont()->bbox(flagsym,
                                                               dx.mags).translated(PointF(x - dx.lw * .5 + dx.xcorr, dy.y1f + dy.ycorr)));
            // printBBox(" notehead + stem + flag", bbox());
        }

        // draw the ledger line for high A
        if (line == -2) {
            item->addbbox(RectF(x - dx.headw * 1.5 - dx.lw * .5, dy.y2 - dx.lw * 2, dx.headw * 2, dx.lw));
            /*
            if (_embelType == 8) {
                  printBBox(" notehead + stem + ledger line", bbox());
                  }
             */
        }

        // move x to next note x position
        x += dx.headp;
    }
}

void TLayout::layout(BarLine* item, LayoutContext&)
{
    item->setPos(PointF());
    // barlines hidden on this staff
    if (item->staff() && item->segment()) {
        if ((!item->staff()->staffTypeForElement(item)->showBarlines() && item->segment()->segmentType() == SegmentType::EndBarLine)
            || (item->staff()->hideSystemBarLine() && item->segment()->segmentType() == SegmentType::BeginBarLine)) {
            item->setbbox(RectF());
            return;
        }
    }

    item->setMag(item->score()->styleB(Sid::scaleBarlines) && item->staff() ? item->staff()->staffMag(item->tick()) : 1.0);
    // Note: the true values of y1 and y2 are computed in layout2() (can be done only
    // after staff distances are known). This is a temporary layout.
    double _spatium = item->spatium();
    item->setY1(_spatium * .5 * item->spanFrom());
    if (RealIsEqual(item->y2(), 0.0)) {
        item->setY2(_spatium * .5 * (8.0 + item->spanTo()));
    }

    double w = item->layoutWidth() * item->mag();
    RectF r(0.0, item->y1(), w, item->y2() - item->y1());

    if (item->score()->styleB(Sid::repeatBarTips)) {
        switch (item->barLineType()) {
        case BarLineType::START_REPEAT:
            r.unite(item->symBbox(SymId::bracketTop).translated(0, item->y1()));
            // r |= symBbox(SymId::bracketBottom).translated(0, y2);
            break;
        case BarLineType::END_REPEAT: {
            double w1 = 0.0;               //symBbox(SymId::reversedBracketTop).width();
            r.unite(item->symBbox(SymId::reversedBracketTop).translated(-w1, item->y1()));
            // r |= symBbox(SymId::reversedBracketBottom).translated(0, y2);
        }
        break;
        case BarLineType::END_START_REPEAT: {
            double w1 = 0.0;               //symBbox(SymId::reversedBracketTop).width();
            r.unite(item->symBbox(SymId::reversedBracketTop).translated(-w1, item->y1()));
            r.unite(item->symBbox(SymId::bracketTop).translated(0, item->y1()));
            // r |= symBbox(SymId::reversedBracketBottom).translated(0, y2);
        }
        break;
        default:
            break;
        }
    }
    item->setbbox(r);

    for (EngravingItem* e : *item->el()) {
        e->layout();
        if (e->isArticulation()) {
            Articulation* a  = toArticulation(e);
            DirectionV dir    = a->direction();
            double distance   = 0.5 * item->spatium();
            double x          = item->width() * .5;
            if (dir == DirectionV::DOWN) {
                double botY = item->y2() + distance;
                a->setPos(PointF(x, botY));
            } else {
                double topY = item->y1() - distance;
                a->setPos(PointF(x, topY));
            }
        }
    }
}

//---------------------------------------------------------
//    called after system layout; set vertical dimensions
//---------------------------------------------------------
void TLayout::layout2(BarLine* item, LayoutContext&)
{
    // barlines hidden on this staff
    if (item->staff() && item->segment()) {
        if ((!item->staff()->staffTypeForElement(item)->showBarlines() && item->segment()->segmentType() == SegmentType::EndBarLine)
            || (item->staff()->hideSystemBarLine() && item->segment()->segmentType() == SegmentType::BeginBarLine)) {
            item->setbbox(RectF());
            return;
        }
    }

    item->getY();
    item->bbox().setTop(item->y1());
    item->bbox().setBottom(item->y2());

    if (item->score()->styleB(Sid::repeatBarTips)) {
        switch (item->barLineType()) {
        case BarLineType::START_REPEAT:
            item->bbox().unite(item->symBbox(SymId::bracketTop).translated(0, item->y1()));
            item->bbox().unite(item->symBbox(SymId::bracketBottom).translated(0, item->y2()));
            break;
        case BarLineType::END_REPEAT:
        {
            double w1 = 0.0;               //symBbox(SymId::reversedBracketTop).width();
            item->bbox().unite(item->symBbox(SymId::reversedBracketTop).translated(-w1, item->y1()));
            item->bbox().unite(item->symBbox(SymId::reversedBracketBottom).translated(-w1, item->y2()));
            break;
        }
        case BarLineType::END_START_REPEAT:
        {
            double w1 = 0.0;               //symBbox(SymId::reversedBracketTop).width();
            item->bbox().unite(item->symBbox(SymId::reversedBracketTop).translated(-w1, item->y1()));
            item->bbox().unite(item->symBbox(SymId::reversedBracketBottom).translated(-w1, item->y2()));
            item->bbox().unite(item->symBbox(SymId::bracketTop).translated(0, item->y1()));
            item->bbox().unite(item->symBbox(SymId::bracketBottom).translated(0, item->y2()));
            break;
        }
        default:
            break;
        }
    }
}

void TLayout::layout(Beam* item, LayoutContext& ctx)
{
    BeamLayout::layout(item, ctx);
}

void TLayout::layout1(Beam* item, LayoutContext& ctx)
{
    BeamLayout::layout1(item, ctx);
}

void TLayout::layout(Bend* item, LayoutContext&)
{
    // during mtest, there may be no score. If so, exit.
    if (!item->score()) {
        return;
    }

    double _spatium = item->spatium();

    if (item->staff() && !item->staff()->isTabStaff(item->tick())) {
        if (!item->explicitParent()) {
            item->setNoteWidth(-_spatium * 2);
            item->setNotePos(PointF(0.0, _spatium * 3));
        }
    }

    double _lw = item->lineWidth();
    Note* note = toNote(item->explicitParent());
    if (note == 0) {
        item->setNoteWidth(0.0);
        item->setNotePos(PointF());
    } else {
        PointF notePos = note->pos();
        notePos.ry() = std::max(notePos.y(), 0.0);

        item->setNoteWidth(note->width());
        item->setNotePos(notePos);
    }
    RectF bb;

    mu::draw::FontMetrics fm(item->font(_spatium));

    size_t n   = item->points().size();
    double x = item->noteWidth();
    double y = -_spatium * .8;
    double x2, y2;

    double aw = _spatium * .5;
    PolygonF arrowUp;
    arrowUp << PointF(0, 0) << PointF(aw * .5, aw) << PointF(-aw * .5, aw);
    PolygonF arrowDown;
    arrowDown << PointF(0, 0) << PointF(aw * .5, -aw) << PointF(-aw * .5, -aw);

    for (size_t pt = 0; pt < n; ++pt) {
        if (pt == (n - 1)) {
            break;
        }
        int pitch = item->points().at(pt).pitch;
        if (pt == 0 && pitch) {
            y2 = -item->notePos().y() - _spatium * 2;
            x2 = x;
            bb.unite(RectF(x, y, x2 - x, y2 - y));

            bb.unite(arrowUp.translated(x2, y2 + _spatium * .2).boundingRect());

            int idx = (pitch + 12) / 25;
            const char* l = Bend::label[idx];
            bb.unite(fm.boundingRect(RectF(x2, y2, 0, 0),
                                     draw::AlignHCenter | draw::AlignBottom | draw::TextDontClip,
                                     String::fromAscii(l)));
            y = y2;
        }
        if (pitch == item->points().at(pt + 1).pitch) {
            if (pt == (n - 2)) {
                break;
            }
            x2 = x + _spatium;
            y2 = y;
            bb.unite(RectF(x, y, x2 - x, y2 - y));
        } else if (pitch < item->points().at(pt + 1).pitch) {
            // up
            x2 = x + _spatium * .5;
            y2 = -item->notePos().y() - _spatium * 2;
            double dx = x2 - x;
            double dy = y2 - y;

            PainterPath path;
            path.moveTo(x, y);
            path.cubicTo(x + dx / 2, y, x2, y + dy / 4, x2, y2);
            bb.unite(path.boundingRect());
            bb.unite(arrowUp.translated(x2, y2 + _spatium * .2).boundingRect());

            int idx = (item->points().at(pt + 1).pitch + 12) / 25;
            const char* l = Bend::label[idx];
            bb.unite(fm.boundingRect(RectF(x2, y2, 0, 0),
                                     draw::AlignHCenter | draw::AlignBottom | draw::TextDontClip,
                                     String::fromAscii(l)));
        } else {
            // down
            x2 = x + _spatium * .5;
            y2 = y + _spatium * 3;
            double dx = x2 - x;
            double dy = y2 - y;

            PainterPath path;
            path.moveTo(x, y);
            path.cubicTo(x + dx / 2, y, x2, y + dy / 4, x2, y2);
            bb.unite(path.boundingRect());

            bb.unite(arrowDown.translated(x2, y2 - _spatium * .2).boundingRect());
        }
        x = x2;
        y = y2;
    }
    bb.adjust(-_lw, -_lw, _lw, _lw);
    item->setbbox(bb);
    item->setPos(0.0, 0.0);
}

using BoxTypes = rtti::TypeList<HBox, VBox, FBox, TBox>;

class BoxVisitor : public rtti::Visitor<BoxVisitor>
{
public:
    template<typename T>
    static bool doVisit(EngravingItem* item, LayoutContext& ctx)
    {
        if (T::classof(item)) {
            TLayout::layout(static_cast<T*>(item), ctx);
            return true;
        }
        return false;
    }
};

void TLayout::layout(Box* item, LayoutContext& ctx)
{
    BoxVisitor::visit(BoxVisitor::ShouldBeFound, BoxTypes {}, item, ctx);
}

void TLayout::layoutBox(Box* item, LayoutContext&)
{
    item->MeasureBase::layout();
    for (EngravingItem* e : item->el()) {
        if (!e->isLayoutBreak()) {
            e->layout();
        }
    }
}

void TLayout::layout(HBox* item, LayoutContext& ctx)
{
    if (item->explicitParent() && item->explicitParent()->isVBox()) {
        VBox* vb = toVBox(item->explicitParent());
        double x = vb->leftMargin() * DPMM;
        double y = vb->topMargin() * DPMM;
        double w = item->point(item->boxWidth());
        double h = vb->height() - (vb->topMargin() + vb->bottomMargin()) * DPMM;
        item->setPos(x, y);
        item->bbox().setRect(0.0, 0.0, w, h);
    } else if (item->system()) {
        item->bbox().setRect(0.0, 0.0, item->point(item->boxWidth()), item->system()->height());
    } else {
        item->bbox().setRect(0.0, 0.0, 50, 50);
    }
    layoutBox(static_cast<Box*>(item), ctx);
}

void TLayout::layout(VBox* item, LayoutContext& ctx)
{
    item->setPos(PointF());

    if (item->system()) {
        item->bbox().setRect(0.0, 0.0, item->system()->width(), item->point(item->boxHeight()));
    } else {
        item->bbox().setRect(0.0, 0.0, 50, 50);
    }

    for (EngravingItem* e : item->el()) {
        if (!e->isLayoutBreak()) {
            e->layout();
        }
    }

    if (item->getProperty(Pid::BOX_AUTOSIZE).toBool()) {
        double contentHeight = item->contentRect().height();

        if (contentHeight < item->minHeight()) {
            contentHeight = item->minHeight();
        }

        item->setHeight(contentHeight);
    }

    item->MeasureBase::layout();

    if (MScore::noImages) {
        adjustLayoutWithoutImages(item, ctx);
    }
}

void TLayout::adjustLayoutWithoutImages(VBox* item, LayoutContext& ctx)
{
    double calculatedVBoxHeight = 0;
    const int padding = item->score()->spatium();
    auto elementList = item->el();

    for (auto pElement : elementList) {
        if (pElement->isText()) {
            Text* txt = toText(pElement);
            txt->bbox().moveTop(0);
            calculatedVBoxHeight += txt->height() + padding;
        }
    }

    item->setHeight(calculatedVBoxHeight);
    layoutBox(static_cast<Box*>(item), ctx);
}

void TLayout::layout(FBox* item, LayoutContext& ctx)
{
    item->bbox().setRect(0.0, 0.0, item->system()->width(), item->point(item->boxHeight()));
    layoutBox(static_cast<Box*>(item), ctx);
}

void TLayout::layout(TBox* item, LayoutContext&)
{
    item->setPos(PointF());        // !?
    item->bbox().setRect(0.0, 0.0, item->system()->width(), 0);
    item->text()->layout();

    double h = 0.;
    if (item->text()->empty()) {
        h = mu::draw::FontMetrics::ascent(item->text()->font());
    } else {
        h = item->text()->height();
    }
    double y = item->topMargin() * DPMM;
    item->text()->setPos(item->leftMargin() * DPMM, y);
    h += item->topMargin() * DPMM + item->bottomMargin() * DPMM;
    item->bbox().setRect(0.0, 0.0, item->system()->width(), h);

    item->MeasureBase::layout();    // layout LayoutBreak's
}

void TLayout::layout(Bracket* item, LayoutContext&)
{
    PainterPath& path = item->path;

    path = PainterPath();
    if (item->h2 == 0.0) {
        return;
    }

    item->setVisible(item->_bi->visible());
    item->_shape.clear();
    switch (item->bracketType()) {
    case BracketType::BRACE: {
        if (item->score()->styleSt(Sid::MusicalSymbolFont) == "Emmentaler"
            || item->score()->styleSt(Sid::MusicalSymbolFont) == "Gonville") {
            item->_braceSymbol = SymId::noSym;
            double w = item->score()->styleMM(Sid::akkoladeWidth);

#define XM(a) (a + 700) * w / 700
#define YM(a) (a + 7100) * item->h2 / 7100

            path.moveTo(XM(-8), YM(-2048));
            path.cubicTo(XM(-8), YM(-3192), XM(-360), YM(-4304), XM(-360), YM(-5400));                 // c 0
            path.cubicTo(XM(-360), YM(-5952), XM(-264), YM(-6488), XM(32), YM(-6968));                 // c 1
            path.cubicTo(XM(36), YM(-6974), XM(38), YM(-6984), XM(38), YM(-6990));                     // c 0
            path.cubicTo(XM(38), YM(-7008), XM(16), YM(-7024), XM(0), YM(-7024));                      // c 0
            path.cubicTo(XM(-8), YM(-7024), XM(-22), YM(-7022), XM(-32), YM(-7008));                   // c 1
            path.cubicTo(XM(-416), YM(-6392), XM(-544), YM(-5680), XM(-544), YM(-4960));               // c 0
            path.cubicTo(XM(-544), YM(-3800), XM(-168), YM(-2680), XM(-168), YM(-1568));               // c 0
            path.cubicTo(XM(-168), YM(-1016), XM(-264), YM(-496), XM(-560), YM(-16));                  // c 1
            path.lineTo(XM(-560), YM(0));                    //  l 1
            path.lineTo(XM(-560), YM(16));                   //  l 1
            path.cubicTo(XM(-264), YM(496), XM(-168), YM(1016), XM(-168), YM(1568));                   // c 0
            path.cubicTo(XM(-168), YM(2680), XM(-544), YM(3800), XM(-544), YM(4960));                  // c 0
            path.cubicTo(XM(-544), YM(5680), XM(-416), YM(6392), XM(-32), YM(7008));                   // c 1
            path.cubicTo(XM(-22), YM(7022), XM(-8), YM(7024), XM(0), YM(7024));                        // c 0
            path.cubicTo(XM(16), YM(7024), XM(38), YM(7008), XM(38), YM(6990));                        // c 0
            path.cubicTo(XM(38), YM(6984), XM(36), YM(6974), XM(32), YM(6968));                        // c 1
            path.cubicTo(XM(-264), YM(6488), XM(-360), YM(5952), XM(-360), YM(5400));                  // c 0
            path.cubicTo(XM(-360), YM(4304), XM(-8), YM(3192), XM(-8), YM(2048));                      // c 0
            path.cubicTo(XM(-8), YM(1320), XM(-136), YM(624), XM(-512), YM(0));                        // c 1
            path.cubicTo(XM(-136), YM(-624), XM(-8), YM(-1320), XM(-8), YM(-2048));                    // c 0*/
            item->setbbox(path.boundingRect());
            item->_shape.add(item->bbox());
        } else {
            if (item->_braceSymbol == SymId::noSym) {
                item->_braceSymbol = SymId::brace;
            }
            double h = item->h2 * 2;
            double w = item->symWidth(item->_braceSymbol) * item->_magx;
            item->bbox().setRect(0, 0, w, h);
            item->_shape.add(item->bbox());
        }
    }
    break;
    case BracketType::NORMAL: {
        double _spatium = item->spatium();
        double w = item->score()->styleMM(Sid::bracketWidth) * .5;
        double x = -w;

        double bd   = (item->score()->styleSt(Sid::MusicalSymbolFont) == "Leland") ? _spatium * .5 : _spatium * .25;
        item->_shape.add(RectF(x, -bd, w * 2, 2 * (item->h2 + bd)));
        item->_shape.add(item->symBbox(SymId::bracketTop).translated(PointF(-w, -bd)));
        item->_shape.add(item->symBbox(SymId::bracketBottom).translated(PointF(-w, bd + 2 * item->h2)));

        w      += item->symWidth(SymId::bracketTop);
        double y = -item->symHeight(SymId::bracketTop) - bd;
        double h = (-y + item->h2) * 2;
        item->bbox().setRect(x, y, w, h);
    }
    break;
    case BracketType::SQUARE: {
        double w = item->score()->styleMM(Sid::staffLineWidth) * .5;
        double x = -w;
        double y = -w;
        double h = (item->h2 + w) * 2;
        w      += (.5 * item->spatium() + 3 * w);
        item->bbox().setRect(x, y, w, h);
        item->_shape.add(item->bbox());
    }
    break;
    case BracketType::LINE: {
        double _spatium = item->spatium();
        double w = 0.67 * item->score()->styleMM(Sid::bracketWidth) * .5;
        double x = -w;
        double bd = _spatium * .25;
        double y = -bd;
        double h = (-y + item->h2) * 2;
        item->bbox().setRect(x, y, w, h);
        item->_shape.add(item->bbox());
    }
    break;
    case BracketType::NO_BRACKET:
        break;
    }
}

void TLayout::layout(Breath* item, LayoutContext&)
{
    bool palette = (!item->staff() || item->track() == mu::nidx);
    if (!palette) {
        int voiceOffset = item->placeBelow() * (item->staff()->lines(item->tick()) - 1) * item->spatium();
        if (item->isCaesura()) {
            item->setPos(item->xpos(), item->spatium() + voiceOffset);
        } else if ((item->score()->styleSt(Sid::MusicalSymbolFont) == "Emmentaler")
                   && (item->symId() == SymId::breathMarkComma)) {
            item->setPos(item->xpos(), 0.5 * item->spatium() + voiceOffset);
        } else {
            item->setPos(item->xpos(), -0.5 * item->spatium() + voiceOffset);
        }
    }
    item->setbbox(item->symBbox(item->symId()));
}

void TLayout::layout(Chord* item, LayoutContext& ctx)
{
    ChordLayout::layout(item, ctx);
}

void TLayout::layout(ChordLine* item, LayoutContext&)
{
    item->setMag(item->chord() ? item->chord()->mag() : 1);
    if (!item->modified()) {
        double x2 = 0;
        double y2 = 0;
        double baseLength = item->spatium() * (item->chord() ? item->chord()->intrinsicMag() : 1);
        double horBaseLength = 1.2 * baseLength;     // let the symbols extend a bit more horizontally
        x2 += item->isToTheLeft() ? -horBaseLength : horBaseLength;
        y2 += item->isBelow() ? baseLength : -baseLength;
        if (item->chordLineType() != ChordLineType::NOTYPE && !item->isWavy()) {
            PainterPath path;
            if (!item->isToTheLeft()) {
                if (item->isStraight()) {
                    path.lineTo(x2, y2);
                } else {
                    path.cubicTo(x2 / 2, 0.0, x2, y2 / 2, x2, y2);
                }
            } else {
                if (item->isStraight()) {
                    path.lineTo(x2, y2);
                } else {
                    path.cubicTo(0.0, y2 / 2, x2 / 2, y2, x2, y2);
                }
            }
            item->setPath(path);
        }
    }

    if (item->explicitParent()) {
        Note* note = nullptr;

        if (item->note()) {
            note = item->chord()->findNote(item->note()->pitch());
        }

        if (!note) {
            note = item->chord()->upNote();
        }

        double x = 0.0;
        double y = note->pos().y();
        double horOffset = 0.33 * item->spatium();     // one third of a space away from the note
        double vertOffset = 0.25 * item->spatium();     // one quarter of a space from the center line
        // Get chord shape
        Shape chordShape = item->chord()->shape();
        // ...but remove from the shape items that the chordline shouldn't try to avoid
        // (especially the chordline itself)
        mu::remove_if(chordShape, [](ShapeElement& shapeEl){
            if (!shapeEl.toItem) {
                return true;
            }
            const EngravingItem* item = shapeEl.toItem;
            if (item->isChordLine() || item->isHarmony() || item->isLyrics()) {
                return true;
            }
            return false;
        });
        x += item->isToTheLeft() ? -chordShape.left() - horOffset : chordShape.right() + horOffset;
        y += item->isBelow() ? vertOffset : -vertOffset;

        /// TODO: calculate properly the position for wavy type
        if (item->isWavy()) {
            bool upDir = item->chordLineType() == ChordLineType::DOIT;
            y += note->height() * (upDir ? 0.8 : -0.3);
        }

        item->setPos(x, y);
    } else {
        item->setPos(0.0, 0.0);
    }

    if (!item->isWavy()) {
        RectF r = item->path().boundingRect();
        int x1 = 0, y1 = 0, width = 0, height = 0;

        x1 = r.x();
        y1 = r.y();
        width = r.width();
        height = r.height();
        item->bbox().setRect(x1, y1, width, height);
    } else {
        RectF r(item->score()->engravingFont()->bbox(ChordLine::WAVE_SYMBOLS, item->magS()));
        double angle = ChordLine::WAVE_ANGEL * M_PI / 180;

        r.setHeight(r.height() + r.width() * sin(angle));

        /// TODO: calculate properly the rect for wavy type
        if (item->chordLineType() == ChordLineType::DOIT) {
            r.setY(item->y() - r.height() * (item->onTabStaff() ? 1.25 : 1));
        }

        item->setbbox(r);
    }
}

void TLayout::layout(Clef* item, LayoutContext&)
{
    // determine current number of lines and line distance
    int lines;
    double lineDist;
    Segment* clefSeg  = item->segment();
    int stepOffset;

    // check clef visibility and type compatibility
    if (clefSeg && item->staff()) {
        Fraction tick = clefSeg->tick();
        const StaffType* st = item->staff()->staffType(tick);
        bool show     = st->genClef();            // check staff type allows clef display
        StaffGroup staffGroup = st->group();

        // if not tab, use instrument->useDrumset to set staffGroup (to allow pitched to unpitched in same staff)
        if (staffGroup != StaffGroup::TAB) {
            staffGroup = item->staff()->part()->instrument(item->tick())->useDrumset() ? StaffGroup::PERCUSSION : StaffGroup::STANDARD;
        }

        // check clef is compatible with staff type group:
        if (ClefInfo::staffGroup(item->clefType()) != staffGroup) {
            if (tick > Fraction(0, 1) && !item->generated()) {     // if clef is not generated, hide it
                show = false;
            } else {                            // if generated, replace with initial clef type
                // TODO : instead of initial staff clef (which is assumed to be compatible)
                // use the last compatible clef previously found in staff
                item->setClefType(item->staff()->clefType(Fraction(0, 1)));
            }
        }

        // if clef not to show or not compatible with staff group
        if (!show) {
            item->setbbox(RectF());
            item->setSymId(SymId::noSym);
            LOGD("Clef::layout(): invisible clef at tick %d(%d) staff %zu",
                 item->segment()->tick().ticks(), item->segment()->tick().ticks() / 1920, item->staffIdx());
            return;
        }
        lines      = st->lines();             // init values from staff type
        lineDist   = st->lineDistance().val();
        stepOffset = st->stepOffset();
    } else {
        lines      = 5;
        lineDist   = 1.0;
        stepOffset = 0;
    }

    double _spatium = item->spatium();
    double yoff     = 0.0;
    if (item->clefType() != ClefType::INVALID && item->clefType() != ClefType::MAX) {
        item->setSymId(ClefInfo::symId(item->clefType()));
        yoff = lineDist * (5 - ClefInfo::line(item->clefType()));
    } else {
        item->setSymId(SymId::noSym);
    }

    switch (item->clefType()) {
    case ClefType::C_19C:                                    // 19th C clef is like a G clef
        yoff = lineDist * 1.5;
        break;
    case ClefType::TAB:                                    // TAB clef
        // on tablature, position clef at half the number of spaces * line distance
        yoff = lineDist * (lines - 1) * .5;
        stepOffset = 0;           //  ignore stepOffset for TAB and percussion clefs
        break;
    case ClefType::TAB4:                                    // TAB clef 4 strings
        // on tablature, position clef at half the number of spaces * line distance
        yoff = lineDist * (lines - 1) * .5;
        stepOffset = 0;
        break;
    case ClefType::TAB_SERIF:                                   // TAB clef alternate style
        // on tablature, position clef at half the number of spaces * line distance
        yoff = lineDist * (lines - 1) * .5;
        stepOffset = 0;
        break;
    case ClefType::TAB4_SERIF:                                   // TAB clef alternate style
        // on tablature, position clef at half the number of spaces * line distance
        yoff = lineDist * (lines - 1) * .5;
        stepOffset = 0;
        break;
    case ClefType::PERC:                                   // percussion clefs
        yoff = lineDist * (lines - 1) * 0.5;
        stepOffset = 0;
        break;
    case ClefType::PERC2:
        yoff = lineDist * (lines - 1) * 0.5;
        stepOffset = 0;
        break;
    case ClefType::INVALID:
    case ClefType::MAX:
        LOGD("Clef::layout: invalid type");
        return;
    default:
        break;
    }
    // clefs on palette or at start of system/measure are left aligned
    // other clefs are right aligned
    RectF r(item->symBbox(item->symId()));
    double x = item->segment() && item->segment()->rtick().isNotZero() ? -r.right() : 0.0;
    item->setPos(x, yoff * _spatium + (stepOffset * 0.5 * _spatium));

    item->setbbox(r);
}

void TLayout::layout(DeadSlapped* item, LayoutContext&)
{
    const double deadSlappedWidth = item->spatium() * 2;
    RectF rect = RectF(0, 0, deadSlappedWidth, item->staff()->height());
    item->setbbox(rect);

    // fillPath
    {
        constexpr double crossThinknessPercentage = 0.1;
        double height = rect.height();
        double width = rect.width();
        double crossThickness = width * crossThinknessPercentage;

        PointF topLeft = PointF(rect.x(), rect.y());
        PointF bottomRight = topLeft + PointF(width, height);
        PointF topRight = topLeft + PointF(width, 0);
        PointF bottomLeft = topLeft + PointF(0, height);
        PointF offsetX = PointF(crossThickness, 0);

        item->m_path1 = mu::draw::PainterPath();

        item->m_path1.moveTo(topLeft);
        item->m_path1.lineTo(topLeft + offsetX);
        item->m_path1.lineTo(bottomRight);
        item->m_path1.lineTo(bottomRight - offsetX);
        item->m_path1.lineTo(topLeft);

        item->m_path2 = mu::draw::PainterPath();

        item->m_path2.moveTo(topRight);
        item->m_path2.lineTo(topRight - offsetX);
        item->m_path2.lineTo(bottomLeft);
        item->m_path2.lineTo(bottomLeft + offsetX);
        item->m_path2.lineTo(topRight);
    }
}

void TLayout::layout(Dynamic* item, LayoutContext&)
{
    item->_snappedExpression = nullptr; // Here we reset it. It will become known again when we layout expression

    const StaffType* stType = item->staffType();

    item->_skipDraw = false;
    if (stType && stType->isHiddenElementOnTab(item->score(), Sid::dynamicsShowTabCommon, Sid::dynamicsShowTabSimple)) {
        item->_skipDraw = true;
        return;
    }

    item->TextBase::layout();

    Segment* s = item->segment();
    if (!s || (!item->_centerOnNotehead && item->align().horizontal == AlignH::LEFT)) {
        return;
    }

    EngravingItem* itemToAlign = nullptr;
    track_idx_t startTrack = staff2track(item->staffIdx());
    track_idx_t endTrack = startTrack + VOICES;
    for (track_idx_t track = startTrack; track < endTrack; ++track) {
        EngravingItem* e = s->elementAt(track);
        if (!e || (e->isRest() && toRest(e)->ticks() >= item->measure()->ticks() && item->measure()->hasVoices(e->staffIdx()))) {
            continue;
        }
        itemToAlign = e;
        break;
    }

    if (!itemToAlign->isChord()) {
        item->movePosX(itemToAlign->width() * 0.5);
        return;
    }

    Chord* chord = toChord(itemToAlign);
    bool centerOnNote = item->_centerOnNotehead || (!item->_centerOnNotehead && item->align().horizontal == AlignH::HCENTER);

    // Move to center of notehead width
    Note* note = chord->notes().at(0);
    double noteHeadWidth = note->headWidth();
    item->movePosX(noteHeadWidth * (centerOnNote ? 0.5 : 1));

    if (!item->_centerOnNotehead) {
        return;
    }

    // Use Smufl optical center for dynamic if available
    SymId symId = TConv::symId(item->dynamicType());
    double opticalCenter = item->symSmuflAnchor(symId, SmuflAnchorId::opticalCenter).x();
    if (symId != SymId::noSym && opticalCenter) {
        double symWidth = item->symBbox(symId).width();
        double offset = symWidth / 2 - opticalCenter + item->symBbox(symId).left();
        double spatiumScaling = item->spatium() / item->score()->spatium();
        offset *= spatiumScaling;
        item->movePosX(offset);
    }

    // If the dynamic contains custom text, keep it aligned
    item->movePosX(-item->customTextOffset());
}

void TLayout::layout(Expression* item, LayoutContext&)
{
    item->TextBase::layout();

    Segment* segment = item->explicitParent() ? toSegment(item->explicitParent()) : nullptr;
    if (!segment) {
        return;
    }

    if (item->align().horizontal != AlignH::LEFT) {
        Chord* chordToAlign = nullptr;
        // Look for chord in this staff
        track_idx_t startTrack = track2staff(item->staffIdx());
        track_idx_t endTrack = startTrack + VOICES;
        for (track_idx_t track = startTrack; track < endTrack; ++track) {
            EngravingItem* item = segment->elementAt(track);
            if (item && item->isChord()) {
                chordToAlign = toChord(item);
                break;
            }
        }

        if (chordToAlign) {
            Note* note = chordToAlign->notes().at(0);
            double headWidth = note->headWidth();
            bool center = item->align().horizontal == AlignH::HCENTER;
            item->movePosX(headWidth * (center ? 0.5 : 1));
        }
    }

    item->_snappedDynamic = nullptr;
    if (!item->_snapToDynamics) {
        item->autoplaceSegmentElement();
        return;
    }

    Dynamic* dynamic = toDynamic(segment->findAnnotation(ElementType::DYNAMIC, item->track(), item->track()));
    if (!dynamic || dynamic->placeAbove() != item->placeAbove()) {
        item->autoplaceSegmentElement();
        return;
    }

    item->_snappedDynamic = dynamic;
    dynamic->setSnappedExpression(item);

    // If there is a dynamic on same segment and track, lock this expression to it
    double padding = item->computeDynamicExpressionDistance();
    double dynamicRight = dynamic->shape().translate(dynamic->pos()).right();
    double expressionLeft = item->bbox().translated(item->pos()).left();
    double difference = expressionLeft - dynamicRight - padding;
    item->movePosX(-difference);

    // Keep expression and dynamic vertically aligned
    item->autoplaceSegmentElement();
    bool above = item->placeAbove();
    double yExpression = item->pos().y();
    double yDynamic = dynamic->pos().y();
    bool expressionIsOuter = above ? yExpression < yDynamic : yExpression > yDynamic;
    if (expressionIsOuter) {
        dynamic->movePosY((yExpression - yDynamic));
    } else {
        item->movePosY((yDynamic - yExpression));
    }
}

void TLayout::layout(Fermata* item, LayoutContext&)
{
    const StaffType* stType = item->staffType();

    item->_skipDraw = false;
    if (stType && stType->isHiddenElementOnTab(item->score(), Sid::fermataShowTabCommon, Sid::fermataShowTabSimple)) {
        item->_skipDraw = true;
        return;
    }

    Segment* s = item->segment();
    item->setPos(PointF());
    if (!s) {            // for use in palette
        item->setOffset(0.0, 0.0);
        RectF b(item->symBbox(item->symId()));
        item->setbbox(b.translated(-0.5 * b.width(), 0.0));
        return;
    }

    if (item->isStyled(Pid::OFFSET)) {
        item->setOffset(item->propertyDefault(Pid::OFFSET).value<PointF>());
    }
    EngravingItem* e = s->element(item->track());
    if (e) {
        if (e->isChord()) {
            Chord* chord = toChord(e);
            Note* note = chord->up() ? chord->downNote() : chord->upNote();
            double offset = chord->xpos() + note->xpos() + note->headWidth() / 2;
            item->movePosX(offset);
        } else {
            item->movePosX(e->x() - e->shape().left() + e->width() * item->staff()->staffMag(Fraction(0, 1)) * .5);
        }
    }

    String name = String::fromAscii(SymNames::nameForSymId(item->_symId).ascii());
    if (item->placeAbove()) {
        if (name.endsWith(u"Below")) {
            item->_symId = SymNames::symIdByName(name.left(name.size() - 5) + u"Above");
        }
    } else {
        item->movePosY(item->staff()->height());
        if (name.endsWith(u"Above")) {
            item->_symId = SymNames::symIdByName(name.left(name.size() - 5) + u"Below");
        }
    }
    RectF b(item->symBbox(item->_symId));
    item->setbbox(b.translated(-0.5 * b.width(), 0.0));
    item->autoplaceSegmentElement();
}

//---------------------------------------------------------
//   FiguredBassItem layout
//    creates the display text (set as element text) and computes
//    the horiz. offset needed to align the right part as well as the vert. offset
//---------------------------------------------------------

void TLayout::layout(FiguredBassItem* item, LayoutContext&)
{
    double h, w, x, x1, x2, y;

    // construct font metrics
    int fontIdx = 0;
    mu::draw::Font f(FiguredBass::FBFonts().at(fontIdx).family, draw::Font::Type::Tablature);

    // font size in pixels, scaled according to spatium()
    // (use the same font selection as used in draw() below)
    double m = item->score()->styleD(Sid::figuredBassFontSize) * item->spatium() / SPATIUM20;
    f.setPointSizeF(m);
    mu::draw::FontMetrics fm(f);

    String str;
    x  = item->symWidth(SymId::noteheadBlack) * .5;
    x1 = x2 = 0.0;

    // create display text
    int font = 0;
    int style = item->score()->styleI(Sid::figuredBassStyle);

    if (item->m_parenth[0] != FiguredBassItem::Parenthesis::NONE) {
        str.append(FiguredBass::FBFonts().at(font).displayParenthesis[int(item->m_parenth[0])]);
    }

    // prefix
    if (item->_prefix != FiguredBassItem::Modifier::NONE) {
        // if no digit, the string created so far 'hangs' to the left of the note
        if (item->_digit == FBIDigitNone) {
            x1 = fm.width(str);
        }
        str.append(FiguredBass::FBFonts().at(font).displayAccidental[int(item->_prefix)]);
        // if no digit, the string from here onward 'hangs' to the right of the note
        if (item->_digit == FBIDigitNone) {
            x2 = fm.width(str);
        }
    }

    if (item->m_parenth[1] != FiguredBassItem::Parenthesis::NONE) {
        str.append(FiguredBass::FBFonts().at(font).displayParenthesis[int(item->m_parenth[1])]);
    }

    // digit
    if (item->_digit != FBIDigitNone) {
        // if some digit, the string created so far 'hangs' to the left of the note
        x1 = fm.width(str);
        // if suffix is a combining shape, combine it with digit (multi-digit numbers cannot be combined)
        // unless there is a parenthesis in between
        if ((item->_digit < 10)
            && (item->_suffix == FiguredBassItem::Modifier::CROSS
                || item->_suffix == FiguredBassItem::Modifier::BACKSLASH
                || item->_suffix == FiguredBassItem::Modifier::SLASH)
            && item->m_parenth[2] == FiguredBassItem::Parenthesis::NONE) {
            str.append(FiguredBass::FBFonts().at(font).displayDigit[style][item->_digit][int(item->_suffix)
                                                                                         - (int(FiguredBassItem::Modifier::CROSS)
                                                                                            - 1)]);
        }
        // if several digits or no shape combination, convert _digit to font styled chars
        else {
            String digits;
            int digit = item->_digit;
            while (true) {
                digits.prepend(FiguredBass::FBFonts().at(font).displayDigit[style][(digit % 10)][0]);
                digit /= 10;
                if (digit == 0) {
                    break;
                }
            }
            str.append(digits);
        }
        // if some digit, the string from here onward 'hangs' to the right of the note
        x2 = fm.width(str);
    }

    if (item->m_parenth[2] != FiguredBassItem::Parenthesis::NONE) {
        str.append(FiguredBass::FBFonts().at(font).displayParenthesis[int(item->m_parenth[2])]);
    }

    // suffix
    // append only if non-combining shape or cannot combine (no digit or parenthesis in between)
    if (item->_suffix != FiguredBassItem::Modifier::NONE
        && ((item->_suffix != FiguredBassItem::Modifier::CROSS
             && item->_suffix != FiguredBassItem::Modifier::BACKSLASH
             && item->_suffix != FiguredBassItem::Modifier::SLASH)
            || item->_digit == FBIDigitNone
            || item->m_parenth[2] != FiguredBassItem::Parenthesis::NONE)) {
        str.append(FiguredBass::FBFonts().at(font).displayAccidental[int(item->_suffix)]);
    }

    if (item->m_parenth[3] != FiguredBassItem::Parenthesis::NONE) {
        str.append(FiguredBass::FBFonts().at(font).displayParenthesis[int(item->m_parenth[3])]);
    }

    item->setDisplayText(str);                  // this text will be displayed

    if (str.size()) {                     // if some text
        x = x - (x1 + x2) * 0.5;          // position the text so that [x1<-->x2] is centered below the note
    } else {                              // if no text (but possibly a line)
        x = 0;                            // start at note left margin
    }
    // vertical position
    h = fm.lineSpacing();
    h *= item->score()->styleD(Sid::figuredBassLineHeight);
    if (item->score()->styleI(Sid::figuredBassAlignment) == 0) {          // top alignment: stack down from first item
        y = h * item->m_ord;
    } else {                                                      // bottom alignment: stack up from last item
        y = -h * (item->figuredBass()->numOfItems() - item->m_ord);
    }
    item->setPos(x, y);
    // determine bbox from text width
//      w = fm.width(str);
    w = fm.width(str);
    item->m_textWidth = w;
    // if there is a cont.line, extend width to cover the whole FB element duration line
    int lineLen;
    if (item->_contLine != FiguredBassItem::ContLine::NONE && (lineLen = item->figuredBass()->lineLength(0)) > w) {
        w = lineLen;
    }
    item->bbox().setRect(0, 0, w, h);
}

void TLayout::layout(Fingering* item, LayoutContext&)
{
    if (item->explicitParent()) {
        Fraction tick = item->parentItem()->tick();
        const Staff* st = item->staff();
        item->setSkipDraw(false);
        if (st && st->isTabStaff(tick)
            && (!st->staffType(tick)->showTabFingering() || item->textStyleType() == TextStyleType::STRING_NUMBER)) {
            item->setSkipDraw(true);
            return;
        }
    }

    item->TextBase::layout();
    item->setPosY(0.0);      // handle placement below

    if (item->autoplace() && item->note()) {
        Note* n      = item->note();
        Chord* chord = n->chord();
        bool voices  = chord->measure()->hasVoices(chord->staffIdx(), chord->tick(), chord->actualTicks());
        bool tight   = voices && chord->notes().size() == 1 && !chord->beam() && item->textStyleType() != TextStyleType::STRING_NUMBER;

        double headWidth = n->bboxRightPos();

        // update offset after drag
        double rebase = 0.0;
        if (item->offsetChanged() != OffsetChange::NONE && !tight) {
            rebase = item->rebaseOffset();
        }

        // temporarily exclude self from chord shape
        item->setAutoplace(false);

        if (item->layoutType() == ElementType::CHORD) {
            bool above = item->placeAbove();
            Stem* stem = chord->stem();
            Segment* s = chord->segment();
            Measure* m = s->measure();
            double sp = item->spatium();
            double md = item->minDistance().val() * sp;
            SysStaff* ss = m->system()->staff(chord->vStaffIdx());
            Staff* vStaff = chord->staff();           // TODO: use current height at tick

            if (n->mirror()) {
                item->movePosX(-n->ipos().x());
            }
            item->movePosX(headWidth * .5);
            if (above) {
                if (tight) {
                    if (chord->stem()) {
                        item->movePosX(-0.8 * sp);
                    }
                    item->movePosY(-1.5 * sp);
                } else {
                    RectF r = item->bbox().translated(m->pos() + s->pos() + chord->pos() + n->pos() + item->pos());
                    SkylineLine sk(false);
                    sk.add(r.x(), r.bottom(), r.width());
                    double d = sk.minDistance(ss->skyline().north());
                    double yd = 0.0;
                    if (d > 0.0 && item->isStyled(Pid::MIN_DISTANCE)) {
                        yd -= d + item->height() * .25;
                    }
                    // force extra space above staff & chord (but not other fingerings)
                    double top;
                    if (chord->up() && chord->beam() && stem) {
                        top = stem->y() + stem->bbox().top();
                    } else {
                        Note* un = chord->upNote();
                        top = std::min(0.0, un->y() + un->bbox().top());
                    }
                    top -= md;
                    double diff = (item->bbox().bottom() + item->ipos().y() + yd + n->y()) - top;
                    if (diff > 0.0) {
                        yd -= diff;
                    }
                    if (item->offsetChanged() != OffsetChange::NONE) {
                        // user moved element within the skyline
                        // we may need to adjust minDistance, yd, and/or offset
                        bool inStaff = above ? r.bottom() + rebase > 0.0 : r.top() + rebase < item->staff()->height();
                        item->rebaseMinDistance(md, yd, sp, rebase, above, inStaff);
                    }
                    item->movePosY(yd);
                }
            } else {
                if (tight) {
                    if (chord->stem()) {
                        item->movePosX(0.8 * sp);
                    }
                    item->movePosY(1.5 * sp);
                } else {
                    RectF r = item->bbox().translated(m->pos() + s->pos() + chord->pos() + n->pos() + item->pos());
                    SkylineLine sk(true);
                    sk.add(r.x(), r.top(), r.width());
                    double d = ss->skyline().south().minDistance(sk);
                    double yd = 0.0;
                    if (d > 0.0 && item->isStyled(Pid::MIN_DISTANCE)) {
                        yd += d + item->height() * .25;
                    }
                    // force extra space below staff & chord (but not other fingerings)
                    double bottom;
                    if (!chord->up() && chord->beam() && stem) {
                        bottom = stem->y() + stem->bbox().bottom();
                    } else {
                        Note* dn = chord->downNote();
                        bottom = std::max(vStaff->height(), dn->y() + dn->bbox().bottom());
                    }
                    bottom += md;
                    double diff = bottom - (item->bbox().top() + item->ipos().y() + yd + n->y());
                    if (diff > 0.0) {
                        yd += diff;
                    }
                    if (item->offsetChanged() != OffsetChange::NONE) {
                        // user moved element within the skyline
                        // we may need to adjust minDistance, yd, and/or offset
                        bool inStaff = above ? r.bottom() + rebase > 0.0 : r.top() + rebase < item->staff()->height();
                        item->rebaseMinDistance(md, yd, sp, rebase, above, inStaff);
                    }
                    item->movePosY(yd);
                }
            }
        } else if (item->textStyleType() == TextStyleType::LH_GUITAR_FINGERING) {
            // place to left of note
            double left = n->shape().left();
            if (left - n->x() > 0.0) {
                item->movePosX(-left);
            } else {
                item->movePosX(-n->x());
            }
        }
        // for other fingering styles, do not autoplace

        // restore autoplace
        item->setAutoplace(true);
    } else if (item->offsetChanged() != OffsetChange::NONE) {
        // rebase horizontally too, as autoplace may have adjusted it
        item->rebaseOffset(false);
    }
    item->setOffsetChanged(false);
}

void TLayout::layout(FretDiagram* item, LayoutContext&)
{
    double _spatium  = item->spatium() * item->_userMag;
    item->m_stringLw        = _spatium * 0.08;
    item->m_nutLw           = (item->_fretOffset || !item->_showNut) ? item->m_stringLw : _spatium * 0.2;
    item->m_stringDist      = item->score()->styleMM(Sid::fretStringSpacing) * item->_userMag;
    item->m_fretDist        = item->score()->styleMM(Sid::fretFretSpacing) * item->_userMag;
    item->m_markerSize      = item->m_stringDist * .8;

    double w    = item->m_stringDist * (item->_strings - 1) + item->m_markerSize;
    double h    = (item->_frets + 1) * item->m_fretDist + item->m_markerSize;
    double y    = -(item->m_markerSize * .5 + item->m_fretDist);
    double x    = -(item->m_markerSize * .5);

    // Allocate space for fret offset number
    if (item->_fretOffset > 0) {
        mu::draw::Font scaledFont(item->m_font);
        scaledFont.setPointSizeF(item->m_font.pointSizeF() * item->_userMag);

        double fretNumMag = item->score()->styleD(Sid::fretNumMag);
        scaledFont.setPointSizeF(scaledFont.pointSizeF() * fretNumMag);
        mu::draw::FontMetrics fm2(scaledFont);
        double numw = fm2.width(String::number(item->_fretOffset + 1));
        double xdiff = numw + item->m_stringDist * .4;
        w += xdiff;
        x += (item->_numPos == 0) == (item->_orientation == Orientation::VERTICAL) ? -xdiff : 0;
    }

    if (item->_orientation == Orientation::HORIZONTAL) {
        double tempW = w,
               tempX = x;
        w = h;
        h = tempW;
        x = y;
        y = tempX;
    }

    // When changing how bbox is calculated, don't forget to update the centerX and rightX methods too.
    item->bbox().setRect(x, y, w, h);

    if (!item->explicitParent() || !item->explicitParent()->isSegment()) {
        item->setPos(PointF());
        return;
    }

    // We need to get the width of the notehead/rest in order to position the fret diagram correctly
    Segment* pSeg = toSegment(item->explicitParent());
    double noteheadWidth = 0;
    if (pSeg->isChordRestType()) {
        staff_idx_t idx = item->staff()->idx();
        for (EngravingItem* e = pSeg->firstElementOfSegment(pSeg, idx); e; e = pSeg->nextElementOfSegment(pSeg, e, idx)) {
            if (e->isRest()) {
                Rest* r = toRest(e);
                noteheadWidth = item->symWidth(r->sym());
                break;
            } else if (e->isNote()) {
                Note* n = toNote(e);
                noteheadWidth = n->headWidth();
                break;
            }
        }
    }

    double mainWidth = 0.0;
    if (item->_orientation == Orientation::VERTICAL) {
        mainWidth = item->m_stringDist * (item->_strings - 1);
    } else if (item->_orientation == Orientation::HORIZONTAL) {
        mainWidth = item->m_fretDist * (item->_frets + 0.5);
    }
    item->setPos((noteheadWidth - mainWidth) / 2, -(h + item->styleP(Sid::fretY)));

    item->autoplaceSegmentElement();

    // don't display harmony in palette
    if (!item->explicitParent()) {
        return;
    }

    if (item->_harmony) {
        item->_harmony->layout();
    }
    if (item->_harmony && item->_harmony->autoplace() && item->_harmony->explicitParent()) {
        Segment* s = toSegment(item->explicitParent());
        Measure* m = s->measure();
        staff_idx_t si = item->staffIdx();

        SysStaff* ss = m->system()->staff(si);
        RectF r = item->_harmony->bbox().translated(m->pos() + s->pos() + item->pos() + item->_harmony->pos());

        double minDistance = item->_harmony->minDistance().val() * item->spatium();
        SkylineLine sk(false);
        sk.add(r.x(), r.bottom(), r.width());
        double d = sk.minDistance(ss->skyline().north());
        if (d > -minDistance) {
            double yd = d + minDistance;
            yd *= -1.0;
            item->_harmony->movePosY(yd);
            r.translate(PointF(0.0, yd));
        }
        if (item->_harmony->addToSkyline()) {
            ss->skyline().add(r);
        }
    }
}

void TLayout::layout(FretCircle* item, LayoutContext&)
{
    item->setSkipDraw(false);
    if (!item->tabEllipseEnabled()) {
        item->setSkipDraw(true);
        item->setbbox(RectF());
        return;
    }

    double lw = item->spatium() * FretCircle::CIRCLE_WIDTH / 2;
    item->m_rect = item->ellipseRect();

    RectF chordRect;
    double minWidth = item->m_chord->upNote()->width();
    for (const Note* note : item->m_chord->notes()) {
        chordRect |= note->bbox();
        minWidth = std::min(minWidth, note->width());
    }

    item->_offsetFromUpNote = (item->m_rect.height() - chordRect.height()
                               - (item->m_chord->downNote()->pos().y() - item->m_chord->upNote()->pos().y())
                               ) / 2;
    item->_sideOffset = (item->m_rect.width() - minWidth) / 2;

    item->setbbox(item->m_rect.adjusted(-lw, -lw, lw, lw));
}

void TLayout::layout(Glissando* item, LayoutContext&)
{
    double _spatium = item->spatium();

    if (item->score()->isPaletteScore() || !item->startElement() || !item->endElement()) {    // for use in palettes or while dragging
        if (item->spannerSegments().empty()) {
            item->add(item->createLineSegment(item->score()->dummy()->system()));
        }
        LineSegment* s = item->frontSegment();
        s->setPos(PointF(-_spatium * Glissando::GLISS_PALETTE_WIDTH / 2, _spatium * Glissando::GLISS_PALETTE_HEIGHT / 2));
        s->setPos2(PointF(_spatium * Glissando::GLISS_PALETTE_WIDTH, -_spatium * Glissando::GLISS_PALETTE_HEIGHT));
        s->layout();
        return;
    }
    item->SLine::layout();
    if (item->spannerSegments().empty()) {
        LOGD("no segments");
        return;
    }
    item->setPos(0.0, 0.0);

    Note* anchor1     = toNote(item->startElement());
    Note* anchor2     = toNote(item->endElement());
    Chord* cr1         = anchor1->chord();
    Chord* cr2         = anchor2->chord();
    GlissandoSegment* segm1 = toGlissandoSegment(item->frontSegment());
    GlissandoSegment* segm2 = toGlissandoSegment(item->backSegment());

    // Note: line segments are defined by
    // initial point: ipos() (relative to system origin)
    // ending point:  pos2() (relative to initial point)

    // LINE ENDING POINTS TO NOTEHEAD CENTRES

    // assume gliss. line goes from centre of initial note centre to centre of ending note:
    // move first segment origin and last segment ending point from notehead origin to notehead centre
    // For TAB: begin at the right-edge of initial note rather than centre
    PointF offs1 = (cr1->staff()->isTabStaff(cr1->tick()))
                   ? PointF(anchor1->bbox().right(), 0.0)
                   : PointF(anchor1->headWidth() * 0.5, 0.0);

    PointF offs2 = PointF(anchor2->headWidth() * 0.5, 0.0);

    // AVOID HORIZONTAL LINES

    int upDown = (0 < (anchor2->pitch() - anchor1->pitch())) - ((anchor2->pitch() - anchor1->pitch()) < 0);
    // on TAB's, glissando are by necessity on the same string, this gives an horizontal glissando line;
    // make bottom end point lower and top ending point higher
    if (cr1->staff()->isTabStaff(cr1->tick())) {
        double yOff = cr1->staff()->lineDistance(cr1->tick()) * 0.4 * _spatium;
        offs1.ry() += yOff * upDown;
        offs2.ry() -= yOff * upDown;
    }
    // if not TAB, angle glissando between notes on the same line
    else {
        if (anchor1->line() == anchor2->line()) {
            offs1.ry() += _spatium * 0.25 * upDown;
            offs2.ry() -= _spatium * 0.25 * upDown;
        }
    }

    // move initial point of first segment and adjust its length accordingly
    segm1->setPos(segm1->ipos() + offs1);
    segm1->setPos2(segm1->ipos2() - offs1);
    // adjust ending point of last segment
    segm2->setPos2(segm2->ipos2() + offs2);

    // FINAL SYSTEM-INITIAL NOTE
    // if the last gliss. segment attaches to a system-initial note, some extra width has to be added
    if (cr2->segment()->measure()->isFirstInSystem() && cr2->rtick().isZero()
        // but ignore graces after, as they are not the first note of the system,
        // even if their segment is the first segment of the system
        && !(cr2->noteType() == NoteType::GRACE8_AFTER
             || cr2->noteType() == NoteType::GRACE16_AFTER || cr2->noteType() == NoteType::GRACE32_AFTER)
        // also ignore if cr1 is a child of cr2, which means cr1 is a grace-before of cr2
        && !(cr1->explicitParent() == cr2)) {
        // in theory we should be reserving space for the gliss prior to the first note of a system
        // but in practice we are not (and would be difficult to get right in current layout algorithms)
        // so, a compromise is to at least use the available space to the left -
        // the default layout for lines left a margin after the header
        segm2->movePosX(-_spatium);
        segm2->rxpos2()+= _spatium;
    }

    // INTERPOLATION OF INTERMEDIATE POINTS
    // This probably belongs to SLine class itself; currently it does not seem
    // to be needed for anything else than Glissando, though

    // get total x-width and total y-height of all segments
    double xTot = 0.0;
    for (SpannerSegment* segm : item->spannerSegments()) {
        xTot += segm->ipos2().x();
    }
    double y0   = segm1->ipos().y();
    double yTot = segm2->ipos().y() + segm2->ipos2().y() - y0;
    yTot -= yStaffDifference(segm2->system(), segm2->staffIdx(), segm1->system(), segm1->staffIdx());
    double ratio = yTot / xTot;
    // interpolate y-coord of intermediate points across total width and height
    double xCurr = 0.0;
    double yCurr;
    for (unsigned i = 0; i + 1 < item->spannerSegments().size(); i++) {
        SpannerSegment* segm = item->segmentAt(i);
        xCurr += segm->ipos2().x();
        yCurr = y0 + ratio * xCurr;
        segm->rypos2() = yCurr - segm->ipos().y();           // position segm. end point at yCurr
        // next segment shall start where this segment stopped, corrected for the staff y-difference
        SpannerSegment* nextSeg = item->segmentAt(i + 1);
        yCurr += yStaffDifference(nextSeg->system(), nextSeg->staffIdx(), segm->system(), segm->staffIdx());
        segm = nextSeg;
        segm->rypos2() += segm->ipos().y() - yCurr;          // adjust next segm. vertical length
        segm->setPosY(yCurr);                                // position next segm. start point at yCurr
    }

    // KEEP CLEAR OF ALL ELEMENTS OF THE CHORD
    // Remove offset already applied
    offs1 *= -1.0;
    offs2 *= -1.0;
    // Look at chord shapes (but don't consider lyrics)
    Shape cr1shape = cr1->shape();
    mu::remove_if(cr1shape, [](ShapeElement& s) {
        if (!s.toItem || s.toItem->isLyrics()) {
            return true;
        } else {
            return false;
        }
    });
    offs1.rx() += cr1shape.right() - anchor1->pos().x();
    if (!cr2->staff()->isTabStaff(cr2->tick())) {
        offs2.rx() -= cr2->shape().left() + anchor2->pos().x();
    }
    // Add note distance
    const double glissNoteDist = 0.25 * item->spatium(); // TODO: style
    offs1.rx() += glissNoteDist;
    offs2.rx() -= glissNoteDist;

    // apply offsets: shorten first segment by x1 (and proportionally y) and adjust its length accordingly
    offs1.ry() = segm1->ipos2().y() * offs1.x() / segm1->ipos2().x();
    segm1->setPos(segm1->ipos() + offs1);
    segm1->setPos2(segm1->ipos2() - offs1);
    // adjust last segment length by x2 (and proportionally y)
    offs2.ry() = segm2->ipos2().y() * offs2.x() / segm2->ipos2().x();
    segm2->setPos2(segm2->ipos2() + offs2);

    for (SpannerSegment* segm : item->spannerSegments()) {
        segm->layout();
    }

    // compute glissando bbox as the bbox of the last segment, relative to the end anchor note
    PointF anchor2PagePos = anchor2->pagePos();
    PointF system2PagePos;
    IF_ASSERT_FAILED(cr2->segment()->system()) {
        system2PagePos = segm2->pos();
    } else {
        system2PagePos = cr2->segment()->system()->pagePos();
    }

    PointF anchor2SystPos = anchor2PagePos - system2PagePos;
    RectF r = RectF(anchor2SystPos - segm2->pos(), anchor2SystPos - segm2->pos() - segm2->pos2()).normalized();
    double lw = item->lineWidth() * .5;
    item->setbbox(r.adjusted(-lw, -lw, lw, lw));

    item->addLineAttachPoints();
}

void TLayout::layout(GlissandoSegment* item, LayoutContext&)
{
    if (item->pos2().x() <= 0) {
        item->setbbox(RectF());
        return;
    }

    if (item->staff()) {
        item->setMag(item->staff()->staffMag(item->tick()));
    }
    RectF r = RectF(0.0, 0.0, item->pos2().x(), item->pos2().y()).normalized();
    double lw = item->glissando()->lineWidth() * .5;
    item->setbbox(r.adjusted(-lw, -lw, lw, lw));
}

void TLayout::layout(GraceNotesGroup* item, LayoutContext&)
{
    Shape _shape;
    for (size_t i = item->size() - 1; i != mu::nidx; --i) {
        Chord* grace = item->at(i);
        Shape graceShape = grace->shape();
        Shape groupShape = _shape;
        mu::remove_if(groupShape, [grace](ShapeElement& s) {
            if (!s.toItem || (s.toItem->isStem() && s.toItem->vStaffIdx() != grace->vStaffIdx())) {
                return true;
            }
            return false;
        });
        double offset;
        offset = -std::max(graceShape.minHorizontalDistance(groupShape), 0.0);
        // Adjust spacing for cross-beam situations
        if (i < item->size() - 1) {
            Chord* prevGrace = item->at(i + 1);
            if (prevGrace->up() != grace->up()) {
                double crossCorrection = grace->notes().front()->headWidth() - grace->stem()->width();
                if (prevGrace->up() && !grace->up()) {
                    offset += crossCorrection;
                } else {
                    offset -= crossCorrection;
                }
            }
        }
        _shape.add(graceShape.translated(mu::PointF(offset, 0.0)));
        double xpos = offset - item->parent()->rxoffset() - item->parent()->xpos();
        grace->setPos(xpos, 0.0);
    }
    double xPos = -_shape.minHorizontalDistance(item->appendedSegment()->staffShape(item->parent()->staffIdx()));
    // If the parent chord is cross-staff, also check against shape in the other staff and take the minimum
    if (item->parent()->staffMove() != 0) {
        double xPosCross = -_shape.minHorizontalDistance(item->appendedSegment()->staffShape(item->parent()->vStaffIdx()));
        xPos = std::min(xPos, xPosCross);
    }
    // Same if the grace note itself is cross-staff
    Chord* firstGN = item->back();
    if (firstGN->staffMove() != 0) {
        double xPosCross = -_shape.minHorizontalDistance(item->appendedSegment()->staffShape(firstGN->vStaffIdx()));
        xPos = std::min(xPos, xPosCross);
    }
    // Safety net in case the shape checks don't succeed
    xPos = std::min(xPos, -double(item->score()->styleMM(Sid::graceToMainNoteDist) + firstGN->notes().front()->headWidth() / 2));
    item->setPos(xPos, 0.0);
}

void TLayout::layout(GradualTempoChangeSegment* item, LayoutContext&)
{
    item->TextLineBaseSegment::layout();
    if (item->isStyled(Pid::OFFSET)) {
        item->roffset() = item->tempoChange()->propertyDefault(Pid::OFFSET).value<PointF>();
    }
    item->autoplaceSpannerSegment();
}

void TLayout::layout(HairpinSegment* item, LayoutContext&)
{
    const StaffType* stType = item->staffType();

    item->setSkipDraw(false);
    if (stType && stType->isHiddenElementOnTab(item->score(), Sid::hairpinShowTabCommon, Sid::hairpinShowTabSimple)) {
        item->setSkipDraw(true);
        return;
    }

    const double _spatium = item->spatium();
    const track_idx_t _trck = item->track();
    Dynamic* sd = nullptr;
    Dynamic* ed = nullptr;
    double dymax = item->hairpin()->placeBelow() ? -10000.0 : 10000.0;
    if (item->autoplace() && !item->score()->isPaletteScore()) {
        Segment* start = item->hairpin()->startSegment();
        Segment* end = item->hairpin()->endSegment();
        // Try to fit between adjacent dynamics
        double minDynamicsDistance = item->score()->styleMM(Sid::autoplaceHairpinDynamicsDistance) * item->staff()->staffMag(item->tick());
        const System* sys = item->system();
        if (item->isSingleType() || item->isBeginType()) {
            if (start && start->system() == sys) {
                sd = toDynamic(start->findAnnotation(ElementType::DYNAMIC, _trck, _trck));
                if (!sd) {
                    // Dynamics might have been added to the previous
                    // segment rather than exactly to hairpin start,
                    // search in that segment too.
                    start = start->prev(SegmentType::ChordRest);
                    if (start && start->system() == sys) {
                        sd = toDynamic(start->findAnnotation(ElementType::DYNAMIC, _trck, _trck));
                    }
                }
            }
            if (sd && sd->addToSkyline() && sd->placement() == item->hairpin()->placement()) {
                const double sdRight = sd->bbox().right() + sd->pos().x()
                                       + sd->segment()->pos().x() + sd->measure()->pos().x();
                const double dist    = std::max(sdRight - item->pos().x() + minDynamicsDistance, 0.0);
                item->movePosX(dist);
                item->rxpos2() -= dist;
                // prepare to align vertically
                dymax = sd->pos().y();
            }
        }
        if (item->isSingleType() || item->isEndType()) {
            if (end && end->tick() < sys->endTick() && start != end) {
                // checking ticks rather than systems
                // systems may be unknown at layout stage.
                ed = toDynamic(end->findAnnotation(ElementType::DYNAMIC, _trck, _trck));
            }
            if (ed && ed->addToSkyline() && ed->placement() == item->hairpin()->placement()) {
                const double edLeft  = ed->bbox().left() + ed->pos().x()
                                       + ed->segment()->pos().x() + ed->measure()->pos().x();
                const double dist    = edLeft - item->pos2().x() - item->pos().x() - minDynamicsDistance;
                const double extendThreshold = 3.0 * _spatium;           // TODO: style setting
                if (dist < 0.0) {
                    item->rxpos2() += dist;                 // always shorten
                } else if (dist >= extendThreshold && item->hairpin()->endText().isEmpty() && minDynamicsDistance > 0.0) {
                    item->rxpos2() += dist;                 // lengthen only if appropriate
                }
                // prepare to align vertically
                if (item->hairpin()->placeBelow()) {
                    dymax = std::max(dymax, ed->pos().y());
                } else {
                    dymax = std::min(dymax, ed->pos().y());
                }
            }
        }
    }

    HairpinType type = item->hairpin()->hairpinType();
    if (item->hairpin()->isLineType()) {
        item->m_twoLines = false;
        item->TextLineBaseSegment::layout();
        item->m_drawCircledTip   = false;
        item->m_circledTipRadius = 0.0;
    } else {
        item->m_twoLines  = true;

        item->hairpin()->setBeginTextAlign({ AlignH::LEFT, AlignV::VCENTER });
        item->hairpin()->setEndTextAlign({ AlignH::RIGHT, AlignV::VCENTER });

        double x1 = 0.0;
        item->TextLineBaseSegment::layout();
        if (!item->m_text->empty()) {
            x1 = item->m_text->width() + _spatium * .5;
        }

        Transform t;
        double h1 = item->hairpin()->hairpinHeight().val() * _spatium * .5;
        double h2 = item->hairpin()->hairpinContHeight().val() * _spatium * .5;

        double x = item->pos2().x();
        if (!item->m_endText->empty()) {
            x -= (item->m_endText->width() + _spatium * .5);             // 0.5 spatium distance
        }
        if (x < _spatium) {               // minimum size of hairpin
            x = _spatium;
        }
        double y = item->pos2().y();
        double len = sqrt(x * x + y * y);
        t.rotateRadians(asin(y / len));

        item->m_drawCircledTip   = item->hairpin()->hairpinCircledTip();
        item->m_circledTipRadius = item->m_drawCircledTip ? 0.6 * _spatium * .5 : 0.0;

        LineF l1, l2;

        switch (type) {
        case HairpinType::CRESC_HAIRPIN: {
            switch (item->spannerSegmentType()) {
            case SpannerSegmentType::SINGLE:
            case SpannerSegmentType::BEGIN:
                l1.setLine(x1 + item->m_circledTipRadius * 2.0, 0.0, len, h1);
                l2.setLine(x1 + item->m_circledTipRadius * 2.0, 0.0, len, -h1);
                item->m_circledTip.setX(x1 + item->m_circledTipRadius);
                item->m_circledTip.setY(0.0);
                break;

            case SpannerSegmentType::MIDDLE:
            case SpannerSegmentType::END:
                item->m_drawCircledTip = false;
                l1.setLine(x1,  h2, len, h1);
                l2.setLine(x1, -h2, len, -h1);
                break;
            }
        }
        break;
        case HairpinType::DECRESC_HAIRPIN: {
            switch (item->spannerSegmentType()) {
            case SpannerSegmentType::SINGLE:
            case SpannerSegmentType::END:
                l1.setLine(x1,  h1, len - item->m_circledTipRadius * 2, 0.0);
                l2.setLine(x1, -h1, len - item->m_circledTipRadius * 2, 0.0);
                item->m_circledTip.setX(len - item->m_circledTipRadius);
                item->m_circledTip.setY(0.0);
                break;
            case SpannerSegmentType::BEGIN:
            case SpannerSegmentType::MIDDLE:
                item->m_drawCircledTip = false;
                l1.setLine(x1,  h1, len, +h2);
                l2.setLine(x1, -h1, len, -h2);
                break;
            }
        }
        break;
        default:
            break;
        }

        // Do Coord rotation
        l1 = t.map(l1);
        l2 = t.map(l2);
        if (item->m_drawCircledTip) {
            item->m_circledTip = t.map(item->m_circledTip);
        }

        item->m_points[0] = l1.p1();
        item->m_points[1] = l1.p2();
        item->m_points[2] = l2.p1();
        item->m_points[3] = l2.p2();
        item->m_npoints   = 4;

        RectF r = RectF(l1.p1(), l1.p2()).normalized().united(RectF(l2.p1(), l2.p2()).normalized());
        if (!item->m_text->empty()) {
            r.unite(item->m_text->bbox());
        }
        if (!item->m_endText->empty()) {
            r.unite(item->m_endText->bbox().translated(x + item->m_endText->bbox().width(), 0.0));
        }
        double w  = item->point(item->score()->styleS(Sid::hairpinLineWidth));
        item->setbbox(r.adjusted(-w * .5, -w * .5, w, w));
    }

    if (!item->explicitParent()) {
        item->setPos(PointF());
        item->roffset() = PointF();
        return;
    }

    if (item->isStyled(Pid::OFFSET)) {
        item->roffset() = item->hairpin()->propertyDefault(Pid::OFFSET).value<PointF>();
    }

    // rebase vertical offset on drag
    double rebase = 0.0;
    if (item->offsetChanged() != OffsetChange::NONE) {
        rebase = item->rebaseOffset();
    }

    if (item->autoplace()) {
        double ymax = item->pos().y();
        double d;
        double ddiff = item->hairpin()->isLineType() ? 0.0 : _spatium * 0.5;

        double sp = item->spatium();

        // TODO: in the future, there should be a minDistance style setting for hairpinLines as well as hairpins.
        double minDist = item->m_twoLines ? item->minDistance().val() : item->score()->styleS(Sid::dynamicsMinDistance).val();
        double md = minDist * sp;

        bool above = item->spanner()->placeAbove();
        SkylineLine sl(!above);
        Shape sh = item->shape();
        sl.add(sh.translated(item->pos()));
        if (above) {
            d  = item->system()->topDistance(item->staffIdx(), sl);
            if (d > -md) {
                ymax -= d + md;
            }
            // align hairpin with dynamics
            if (!item->hairpin()->diagonal()) {
                ymax = std::min(ymax, dymax - ddiff);
            }
        } else {
            d  = item->system()->bottomDistance(item->staffIdx(), sl);
            if (d > -md) {
                ymax += d + md;
            }
            // align hairpin with dynamics
            if (!item->hairpin()->diagonal()) {
                ymax = std::max(ymax, dymax - ddiff);
            }
        }
        double yd = ymax - item->pos().y();
        if (yd != 0.0) {
            if (item->offsetChanged() != OffsetChange::NONE) {
                // user moved element within the skyline
                // we may need to adjust minDistance, yd, and/or offset
                double adj = item->pos().y() + rebase;
                bool inStaff = above ? sh.bottom() + adj > 0.0 : sh.top() + adj < item->staff()->height();
                item->rebaseMinDistance(md, yd, sp, rebase, above, inStaff);
            }
            item->movePosY(yd);
        }

        if (item->hairpin()->addToSkyline() && !item->hairpin()->diagonal()) {
            // align dynamics with hairpin
            if (sd && sd->autoplace() && sd->placement() == item->hairpin()->placement()) {
                double ny = item->y() + ddiff - sd->offset().y();
                if (sd->placeAbove()) {
                    ny = std::min(ny, sd->ipos().y());
                } else {
                    ny = std::max(ny, sd->ipos().y());
                }
                if (sd->ipos().y() != ny) {
                    sd->setPosY(ny);
                    if (sd->addToSkyline()) {
                        Segment* s = sd->segment();
                        Measure* m = s->measure();
                        RectF r = sd->bbox().translated(sd->pos());
                        s->staffShape(sd->staffIdx()).add(r);
                        r = sd->bbox().translated(sd->pos() + s->pos() + m->pos());
                        m->system()->staff(sd->staffIdx())->skyline().add(r);
                    }
                }
            }
            if (ed && ed->autoplace() && ed->placement() == item->hairpin()->placement()) {
                double ny = item->y() + ddiff - ed->offset().y();
                if (ed->placeAbove()) {
                    ny = std::min(ny, ed->ipos().y());
                } else {
                    ny = std::max(ny, ed->ipos().y());
                }
                if (ed->ipos().y() != ny) {
                    ed->setPosY(ny);
                    if (ed->addToSkyline()) {
                        Segment* s = ed->segment();
                        Measure* m = s->measure();
                        RectF r = ed->bbox().translated(ed->pos());
                        s->staffShape(ed->staffIdx()).add(r);
                        r = ed->bbox().translated(ed->pos() + s->pos() + m->pos());
                        m->system()->staff(ed->staffIdx())->skyline().add(r);
                    }
                }
            }
        }
    }
    item->setOffsetChanged(false);
}

void TLayout::layout(Hairpin* item, LayoutContext&)
{
    item->setPos(0.0, 0.0);
    item->TextLineBase::layout();
}

void TLayout::layout(HarmonicMarkSegment* item, LayoutContext&)
{
    const StaffType* stType = item->staffType();

    item->setSkipDraw(false);
    if (stType
        && (!stType->isTabStaff()
            || stType->isHiddenElementOnTab(item->score(), Sid::harmonicMarkShowTabCommon, Sid::harmonicMarkShowTabSimple))) {
        item->setSkipDraw(true);
        return;
    }

    item->TextLineBaseSegment::layout();
    item->autoplaceSpannerSegment();
}

void TLayout::layout(Harmony* item, LayoutContext&)
{
    if (!item->explicitParent()) {
        item->setPos(0.0, 0.0);
        item->setOffset(0.0, 0.0);
        item->layout1();
        return;
    }
    //if (isStyled(Pid::OFFSET))
    //      setOffset(propertyDefault(Pid::OFFSET).value<PointF>());

    item->layout1();
    item->setPos(calculateBoundingRect(item));
}

void TLayout::layout1(Harmony* item, LayoutContext&)
{
    if (item->isLayoutInvalid()) {
        item->createLayout();
    }

    if (item->textBlockList().empty()) {
        item->textBlockList().push_back(TextBlock());
    }

    calculateBoundingRect(item);

    if (item->hasFrame()) {
        item->layoutFrame();
    }

    item->score()->addRefresh(item->canvasBoundingRect());
}

PointF TLayout::calculateBoundingRect(Harmony* item)
{
    const double ypos = (item->placeBelow() && item->staff()) ? item->staff()->height() : 0.0;
    const FretDiagram* fd = (item->explicitParent() && item->explicitParent()->isFretDiagram())
                            ? toFretDiagram(item->explicitParent())
                            : nullptr;

    const double cw = item->symWidth(SymId::noteheadBlack);

    double newPosX = 0.0;
    double newPosY = 0.0;

    if (item->textList.empty()) {
        item->TextBase::layout1();

        if (fd) {
            newPosY = item->ypos();
        } else {
            newPosY = ypos - ((item->align() == AlignV::BOTTOM) ? item->_harmonyHeight - item->bbox().height() : 0.0);
        }
    } else {
        RectF bb;
        for (TextSegment* ts : item->textList) {
            bb.unite(ts->tightBoundingRect().translated(ts->x, ts->y));
        }

        double xx = 0.0;
        switch (item->align().horizontal) {
        case AlignH::LEFT:
            xx = -bb.left();
            break;
        case AlignH::HCENTER:
            xx = -(bb.center().x());
            break;
        case AlignH::RIGHT:
            xx = -bb.right();
            break;
        }

        double yy = -bb.y();      // Align::TOP
        if (item->align() == AlignV::VCENTER) {
            yy = -bb.y() / 2.0;
        } else if (item->align() == AlignV::BASELINE) {
            yy = 0.0;
        } else if (item->align() == AlignV::BOTTOM) {
            yy = -bb.height() - bb.y();
        }

        if (fd) {
            newPosY = ypos - yy - item->score()->styleMM(Sid::harmonyFretDist);
        } else {
            newPosY = ypos;
        }

        for (TextSegment* ts : item->textList) {
            ts->offset = PointF(xx, yy);
        }

        item->setbbox(bb.translated(xx, yy));
        item->_harmonyHeight = item->bbox().height();
    }

    if (fd) {
        switch (item->align().horizontal) {
        case AlignH::LEFT:
            newPosX = 0.0;
            break;
        case AlignH::HCENTER:
            newPosX = fd->centerX();
            break;
        case AlignH::RIGHT:
            newPosX = fd->rightX();
            break;
        }
    } else {
        switch (item->align().horizontal) {
        case AlignH::LEFT:
            newPosX = 0.0;
            break;
        case AlignH::HCENTER:
            newPosX = cw * 0.5;
            break;
        case AlignH::RIGHT:
            newPosX = cw;
            break;
        }
    }

    return PointF(newPosX, newPosY);
}

void TLayout::layout(Hook* item, LayoutContext&)
{
    item->setbbox(item->symBbox(item->sym()));
}

void TLayout::layout(Image* item, LayoutContext&)
{
    item->setPos(0.0, 0.0);
    item->init();

    SizeF imageSize = item->size();

    // if autoscale && inside a box, scale to box relevant size
    if (item->autoScale()
        && item->explicitParent()
        && ((item->explicitParent()->isHBox() || item->explicitParent()->isVBox()))) {
        if (item->lockAspectRatio()) {
            double f = item->sizeIsSpatium() ? item->spatium() : DPMM;
            SizeF size(item->imageSize());
            double ratio = size.width() / size.height();
            double w = item->parentItem()->width();
            double h = item->parentItem()->height();
            if ((w / h) < ratio) {
                imageSize.setWidth(w / f);
                imageSize.setHeight((w / ratio) / f);
            } else {
                imageSize.setHeight(h / f);
                imageSize.setWidth(h * ratio / f);
            }
        } else {
            imageSize = item->pixel2size(item->parentItem()->bbox().size());
        }
    }

    item->setSize(imageSize);

    // in any case, adjust position relative to parent
    item->setbbox(RectF(PointF(), item->size2pixel(imageSize)));
}

void TLayout::layout(InstrumentChange* item, LayoutContext&)
{
    item->TextBase::layout();
    item->autoplaceSegmentElement();
}

void TLayout::layout(Jump* item, LayoutContext&)
{
    item->TextBase::layout();
    item->autoplaceMeasureElement();
}

void TLayout::layout(KeySig* item, LayoutContext&)
{
    double _spatium = item->spatium();
    double step = _spatium * (item->staff() ? item->staff()->staffTypeForElement(item)->lineDistance().val() * 0.5 : 0.5);

    item->setbbox(RectF());

    item->_sig.keySymbols().clear();
    if (item->staff() && !item->staff()->staffType(item->tick())->genKeysig()) {
        return;
    }

    // determine current clef for this staff
    ClefType clef = ClefType::G;
    if (item->staff()) {
        // Look for a clef before the key signature at the same tick
        Clef* c = nullptr;
        if (item->segment()) {
            for (Segment* seg = item->segment()->prev1(); !c && seg && seg->tick() == item->tick(); seg = seg->prev1()) {
                if (seg->isClefType() || seg->isHeaderClefType()) {
                    c = toClef(seg->element(item->track()));
                }
            }
        }
        if (c) {
            clef = c->clefType();
        } else {
            // no clef found, so get the clef type from the clefs list, using the previous tick
            clef = item->staff()->clef(item->tick() - Fraction::fromTicks(1));
        }
    }

    int t1 = int(item->_sig.key());

    if (item->isCustom() && !item->isAtonal()) {
        double accidentalGap = item->score()->styleS(Sid::keysigAccidentalDistance).val();
        // add standard key accidentals first, if necessary
        for (int i = 1; i <= abs(t1) && abs(t1) <= 7; ++i) {
            bool drop = false;
            for (CustDef& cd: item->_sig.customKeyDefs()) {
                int degree = item->_sig.degInKey(cd.degree);
                // if custom keysig accidental takes place, don't create tonal accidental
                if ((degree * 2 + 2) % 7 == (t1 < 0 ? 8 - i : i) % 7) {
                    drop = true;
                    break;
                }
            }
            if (!drop) {
                KeySym ks;
                int lineIndexOffset = t1 > 0 ? -1 : 6;
                ks.sym = t1 > 0 ? SymId::accidentalSharp : SymId::accidentalFlat;
                ks.line = ClefInfo::lines(clef)[lineIndexOffset + i];
                if (item->_sig.keySymbols().size() > 0) {
                    KeySym& previous = item->_sig.keySymbols().back();
                    double previousWidth = item->symWidth(previous.sym) / _spatium;
                    ks.xPos = previous.xPos + previousWidth + accidentalGap;
                } else {
                    ks.xPos = 0;
                }
                // TODO octave metters?
                item->_sig.keySymbols().push_back(ks);
            }
        }
        for (CustDef& cd : item->_sig.customKeyDefs()) {
            SymId sym = item->_sig.symInKey(cd.sym, cd.degree);
            int degree = item->_sig.degInKey(cd.degree);
            bool flat = std::string(SymNames::nameForSymId(sym).ascii()).find("Flat") != std::string::npos;
            int accIdx = (degree * 2 + 1) % 7; // C D E F ... index to F C G D index
            accIdx = flat ? 13 - accIdx : accIdx;
            int line = ClefInfo::lines(clef)[accIdx] + cd.octAlt * 7;
            double xpos = cd.xAlt;
            if (item->_sig.keySymbols().size() > 0) {
                KeySym& previous = item->_sig.keySymbols().back();
                double previousWidth = item->symWidth(previous.sym) / _spatium;
                xpos += previous.xPos + previousWidth + accidentalGap;
            }
            // if translated symbol if out of range, add key accidental followed by untranslated symbol
            if (sym == SymId::noSym) {
                KeySym ks;
                ks.line = line;
                ks.xPos = xpos;
                // for quadruple sharp use two double sharps
                if (cd.sym == SymId::accidentalTripleSharp) {
                    ks.sym = SymId::accidentalDoubleSharp;
                    sym = SymId::accidentalDoubleSharp;
                } else {
                    ks.sym = t1 > 0 ? SymId::accidentalSharp : SymId::accidentalFlat;
                    sym = cd.sym;
                }
                item->_sig.keySymbols().push_back(ks);
                xpos += t1 < 0 ? 0.7 : 1; // flats closer
            }
            // create symbol; natural only if is user defined
            if (sym != SymId::accidentalNatural || sym == cd.sym) {
                KeySym ks;
                ks.sym = sym;
                ks.line = line;
                ks.xPos = xpos;
                item->_sig.keySymbols().push_back(ks);
            }
        }
    } else {
        int accidentals = 0, naturals = 0;
        switch (std::abs(t1)) {
        case 7: accidentals = 0x7f;
            break;
        case 6: accidentals = 0x3f;
            break;
        case 5: accidentals = 0x1f;
            break;
        case 4: accidentals = 0xf;
            break;
        case 3: accidentals = 0x7;
            break;
        case 2: accidentals = 0x3;
            break;
        case 1: accidentals = 0x1;
            break;
        case 0: accidentals = 0;
            break;
        default:
            LOGD("illegal t1 key %d", t1);
            break;
        }

        // manage display of naturals:
        // naturals are shown if there is some natural AND prev. measure has no section break
        // AND style says they are not off
        // OR key sig is CMaj/Amin (in which case they are always shown)

        bool naturalsOn = false;
        Measure* prevMeasure = item->measure() ? item->measure()->prevMeasure() : 0;

        // If we're not force hiding naturals (Continuous panel), use score style settings
        if (!item->_hideNaturals) {
            const bool newSection = (!item->segment()
                                     || (item->segment()->rtick().isZero() && (!prevMeasure || prevMeasure->sectionBreak()))
                                     );
            naturalsOn = !newSection && (item->score()->styleI(Sid::keySigNaturals) != int(KeySigNatural::NONE) || (t1 == 0));
        }

        // Don't repeat naturals if shown in courtesy
        if (item->measure() && item->measure()->system() && item->measure()->isFirstInSystem()
            && prevMeasure && prevMeasure->findSegment(SegmentType::KeySigAnnounce, item->tick())
            && !item->segment()->isKeySigAnnounceType()) {
            naturalsOn = false;
        }
        if (item->track() == mu::nidx) {
            naturalsOn = false;
        }

        int coffset = 0;
        Key t2      = Key::C;
        if (naturalsOn) {
            if (item->staff()) {
                t2 = item->staff()->key(item->tick() - Fraction(1, 480 * 4));
            }
            if (t2 == Key::C) {
                naturalsOn = false;
            } else {
                switch (std::abs(int(t2))) {
                case 7: naturals = 0x7f;
                    break;
                case 6: naturals = 0x3f;
                    break;
                case 5: naturals = 0x1f;
                    break;
                case 4: naturals = 0xf;
                    break;
                case 3: naturals = 0x7;
                    break;
                case 2: naturals = 0x3;
                    break;
                case 1: naturals = 0x1;
                    break;
                case 0: naturals = 0;
                    break;
                default:
                    LOGD("illegal t2 key %d", int(t2));
                    break;
                }
                // remove redundant naturals
                if (!((t1 > 0) ^ (t2 > 0))) {
                    naturals &= ~accidentals;
                }
                if (t2 < 0) {
                    coffset = 7;
                }
            }
        }

        // naturals should go BEFORE accidentals if style says so
        // OR going from sharps to flats or vice versa (i.e. t1 & t2 have opposite signs)

        bool prefixNaturals = naturalsOn
                              && (item->score()->styleI(Sid::keySigNaturals) == int(KeySigNatural::BEFORE)
                                  || t1 * int(t2) < 0);

        // naturals should go AFTER accidentals if they should not go before!
        bool suffixNaturals = naturalsOn && !prefixNaturals;

        const signed char* lines = ClefInfo::lines(clef);

        if (prefixNaturals) {
            for (int i = 0; i < 7; ++i) {
                if (naturals & (1 << i)) {
                    keySigAddLayout(item, SymId::accidentalNatural, lines[i + coffset]);
                }
            }
        }
        if (abs(t1) <= 7) {
            SymId symbol = t1 > 0 ? SymId::accidentalSharp : SymId::accidentalFlat;
            int lineIndexOffset = t1 > 0 ? 0 : 7;
            for (int i = 0; i < abs(t1); ++i) {
                keySigAddLayout(item, symbol, lines[lineIndexOffset + i]);
            }
        } else {
            LOGD("illegal t1 key %d", t1);
        }

        // add suffixed naturals, if any
        if (suffixNaturals) {
            for (int i = 0; i < 7; ++i) {
                if (naturals & (1 << i)) {
                    keySigAddLayout(item, SymId::accidentalNatural, lines[i + coffset]);
                }
            }
        }

        // Follow stepOffset
        if (item->staffType()) {
            item->setPosY(item->staffType()->stepOffset() * 0.5 * _spatium);
        }
    }

    // compute bbox
    for (KeySym& ks : item->_sig.keySymbols()) {
        double x = ks.xPos * _spatium;
        double y = ks.line * step;
        item->addbbox(item->symBbox(ks.sym).translated(x, y));
    }
}

void TLayout::keySigAddLayout(KeySig* item, SymId sym, int line)
{
    double _spatium = item->spatium();
    double step = _spatium * (item->staff() ? item->staff()->staffTypeForElement(item)->lineDistance().val() * 0.5 : 0.5);
    KeySym ks;
    ks.sym = sym;
    double x = 0.0;
    if (item->_sig.keySymbols().size() > 0) {
        KeySym& previous = item->_sig.keySymbols().back();
        double accidentalGap = item->score()->styleS(Sid::keysigAccidentalDistance).val();
        if (previous.sym != sym) {
            accidentalGap *= 2;
        } else if (previous.sym == SymId::accidentalNatural && sym == SymId::accidentalNatural) {
            accidentalGap = item->score()->styleS(Sid::keysigNaturalDistance).val();
        }
        double previousWidth = item->symWidth(previous.sym) / _spatium;
        x = previous.xPos + previousWidth + accidentalGap;
        bool isAscending = line < previous.line;
        SmuflAnchorId currentCutout = isAscending ? SmuflAnchorId::cutOutSW : SmuflAnchorId::cutOutNW;
        SmuflAnchorId previousCutout = isAscending ? SmuflAnchorId::cutOutNE : SmuflAnchorId::cutOutSE;
        PointF cutout = item->symSmuflAnchor(sym, currentCutout);
        double currentCutoutY = line * step + cutout.y();
        double previousCutoutY = previous.line * step + item->symSmuflAnchor(previous.sym, previousCutout).y();
        if ((isAscending && currentCutoutY < previousCutoutY) || (!isAscending && currentCutoutY > previousCutoutY)) {
            x -= cutout.x() / _spatium;
        }
    }
    ks.xPos = x;
    ks.line = line;
    item->_sig.keySymbols().push_back(ks);
}
