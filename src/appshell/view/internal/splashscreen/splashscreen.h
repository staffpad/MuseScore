/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2022 MuseScore Limited
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

#ifndef MU_APPSHELL_SPLASHSCREEN_H
#define MU_APPSHELL_SPLASHSCREEN_H

#include <QWidget>

class QSvgRenderer;

namespace mu::appshell {
class SplashScreen : public QWidget
{
public:
    enum SplashScreenType {
        Default,
        ForNewInstance
    };

    explicit SplashScreen(SplashScreenType type, bool forNewScore = false, const QString& openingFileName = QString());

private:
    bool event(QEvent* event) override;
    void repaint();
    void draw(QPainter* painter);
    void setSize(const QSize& size);

    QWidget* m_view = nullptr;
};
}

#endif // MU_APPSHELL_SPLASHSCREEN_H
