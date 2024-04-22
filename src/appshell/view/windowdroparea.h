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

#ifndef MU_APPSHELL_WINDOWDROPAREA_H
#define MU_APPSHELL_WINDOWDROPAREA_H

#include <QQuickItem>

#include "modularity/ioc.h"
#include "iapplicationactioncontroller.h"

namespace mu::appshell {
class WindowDropArea : public QQuickItem
{
    INJECT(IApplicationActionController, applicationActionController)
public:
    explicit WindowDropArea(QQuickItem* parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent*) override;
};
}

#endif // MU_APPSHELL_WINDOWDROPAREA_H
