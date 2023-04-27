/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2023 MuseScore BVBA and others
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
#include "restbeamsettingsmodel.h"

#include "translation.h"
#include "dataformatter.h"

using namespace mu::inspector;
using namespace mu::engraving;

RestBeamSettingsModel::RestBeamSettingsModel(QObject* parent, IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, repository)
{
    setModelType(InspectorModelType::TYPE_BEAM);
    setTitle(qtrc("inspector", "Beam"));
    setBeamModesModel(new BeamModesModel(this, repository));
}

QObject* RestBeamSettingsModel::beamModesModel() const
{
    return m_beamModesModel;
}

void RestBeamSettingsModel::setBeamModesModel(BeamModesModel* beamModesModel)
{
    m_beamModesModel = beamModesModel;

    connect(m_beamModesModel->mode(), &PropertyItem::propertyModified, this, &AbstractInspectorModel::requestReloadPropertyItems);

    emit beamModesModelChanged(m_beamModesModel);
}

void RestBeamSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(mu::engraving::ElementType::REST);
}
