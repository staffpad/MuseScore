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

#include "notationtoolbarmodel.h"

using namespace mu::notation;
using namespace muse::uicomponents;
using namespace muse::actions;

void NotationToolBarModel::load()
{
    MenuItemList items = {
        makeItem("parts"),
        makeItem("toggle-mixer")
    };

    setItems(items);

    context()->currentMasterNotationChanged().onNotify(this, [this]() {
        load();
    });

    AbstractMenuModel::load();
}

MenuItem* NotationToolBarModel::makeItem(const ActionCode& actionCode)
{
    MenuItem* item = new MenuItem(actionsRegister()->action(actionCode), this);

    muse::ui::UiActionState state;
    state.enabled = context()->currentNotation() != nullptr;
    item->setState(state);

    return item;
}
