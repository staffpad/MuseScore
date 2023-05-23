/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2022 MuseScore BVBA and others
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

#include "harppedalpopupmodel.h"
#include "libmscore/stafflines.h"

#include "log.h"

using namespace mu::notation;

HarpPedalPopupModel::HarpPedalPopupModel(QObject* parent)
    : AbstractElementPopupModel(parent)
{
    setModelType(PopupModelType::TYPE_HARP_DIAGRAM);
    setTitle("Harp pedal");
}

void HarpPedalPopupModel::init()
{
    AbstractElementPopupModel::init();

    connect(this, &AbstractElementPopupModel::dataChanged, [this]() {
        load();
    });

    load();
}

void HarpPedalPopupModel::load()
{
    EngravingItem* element = getElement();
    m_diagram = element && element->isHarpPedalDiagram() ? toHarpPedalDiagram(getElement()) : nullptr;
    if (!m_diagram) {
        return;
    }

    m_isDiagram = m_diagram->isDiagram();
    emit isDiagramChanged(isDiagram());

    setPopupPedalState(m_diagram->getPedalState());
    emit pedalStateChanged(pedalState());
}

std::array<mu::engraving::PedalPosition, mu::engraving::HARP_STRING_NO> HarpPedalPopupModel::getPopupPedalState()
{
    std::array<mu::engraving::PedalPosition, mu::engraving::HARP_STRING_NO> posArr;
    for (int i = 0; i < 7; i++) {
        switch (m_pedalState.at(i)) {
        case HarpPedalPopupModel::Position::FLAT:
            posArr[i] = mu::engraving::PedalPosition::FLAT;
            break;
        case HarpPedalPopupModel::Position::NATURAL:
            posArr[i] = mu::engraving::PedalPosition::NATURAL;
            break;
        case HarpPedalPopupModel::Position::SHARP:
            posArr[i] = mu::engraving::PedalPosition::SHARP;
            break;
        case HarpPedalPopupModel::Position::UNSET:
            posArr[i] = mu::engraving::PedalPosition::UNSET;
            break;
        default:
            posArr[i] = mu::engraving::PedalPosition::UNSET;
            break;
        }
    }

    return posArr;
}

void HarpPedalPopupModel::setPopupPedalState(std::array<HarpPedalPopupModel::Position, mu::engraving::HARP_STRING_NO> pos)
{
    m_pedalState = pos;
}

void HarpPedalPopupModel::setPopupPedalState(std::array<mu::engraving::PedalPosition, mu::engraving::HARP_STRING_NO> pos)
{
    std::array<HarpPedalPopupModel::Position, mu::engraving::HARP_STRING_NO> posArr;
    for (int i = 0; i < 7; i++) {
        switch (pos.at(i)) {
        case mu::engraving::PedalPosition::FLAT:
            posArr[i] = HarpPedalPopupModel::Position::FLAT;
            break;
        case mu::engraving::PedalPosition::NATURAL:
            posArr[i] = HarpPedalPopupModel::Position::NATURAL;
            break;
        case mu::engraving::PedalPosition::SHARP:
            posArr[i] = HarpPedalPopupModel::Position::SHARP;
            break;
        case mu::engraving::PedalPosition::UNSET:
            posArr[i] = HarpPedalPopupModel::Position::UNSET;
            break;
        default:
            posArr[i] = HarpPedalPopupModel::Position::UNSET;
            break;
        }
    }

    m_pedalState = posArr;
}

bool HarpPedalPopupModel::isDiagram() const
{
    return m_isDiagram;
}

QPointF HarpPedalPopupModel::pos() const
{
    return fromLogical(m_diagram->canvasPos()).toQPointF();
}

QPointF HarpPedalPopupModel::size() const
{
    RectF elemRect = fromLogical(m_diagram->canvasBoundingRect());
    return QPointF(elemRect.width(), elemRect.height());
}

bool HarpPedalPopupModel::belowStave() const
{
    if (m_diagram != nullptr && (m_diagram->getProperty(mu::engraving::Pid::PLACEMENT) == mu::engraving::PlacementV::BELOW)) {
        return true;
    }
    return false;
}

QRectF HarpPedalPopupModel::staffPos() const
{
    // Just need top & bottom y.  Don't need x pos
    Measure* measure = m_diagram->measure();
    auto harpIdxList = m_diagram->part()->staveIdxList();
    std::list<engraving::StaffLines*> staves;
    for (auto idx : harpIdxList) {
        staves.push_back(measure->staffLines(idx));
    }

    if (staves.size() > 0) {
        engraving::StaffLines* topStaff = staves.front();
        engraving::StaffLines* bottomStaff = staves.back();
        RectF staffRect
            = RectF(measure->canvasBoundingRect().x(),
                    topStaff->canvasBoundingRect().y(),
                    measure->canvasBoundingRect().width(),
                    bottomStaff->canvasBoundingRect().bottomLeft().y() - topStaff->canvasBoundingRect().topLeft().y());

        return fromLogical(staffRect).toQRectF();
    }
    return QRectF();
}

QVector<HarpPedalPopupModel::Position> HarpPedalPopupModel::pedalState() const
{
    return QVector<HarpPedalPopupModel::Position>(m_pedalState.begin(), m_pedalState.end());
}

void HarpPedalPopupModel::setIsDiagram(bool isDiagram)
{
    if (m_isDiagram == isDiagram) {
        return;
    }

    beginCommand();

    m_isDiagram = isDiagram;
    m_diagram->undoChangeProperty(mu::engraving::Pid::HARP_IS_DIAGRAM, m_isDiagram, mu::engraving::PropertyFlags::STYLED);
    updateNotation();
    endCommand();
    emit isDiagramChanged(m_isDiagram);
}

void HarpPedalPopupModel::setDiagramPedalState(QVector<Position> pedalState)
{
    std::array<HarpPedalPopupModel::Position, mu::engraving::HARP_STRING_NO> stdPedalState;
    for (int i = 0; i < 7; i++) {
        stdPedalState[i] = pedalState.at(i);
    }

    if (stdPedalState == m_pedalState) {
        return;
    }

    beginCommand();
    setPopupPedalState(stdPedalState);
    m_diagram->score()->undo(new mu::engraving::ChangeHarpPedalState(m_diagram, getPopupPedalState()));
    updateNotation();
    endCommand();
    emit pedalStateChanged(pedalState);
}
