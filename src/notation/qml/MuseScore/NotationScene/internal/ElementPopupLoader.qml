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
import QtQuick 2.15
import QtQuick.Controls 2.15

import Muse.Ui 1.0
import Muse.UiComponents 1.0
import MuseScore.NotationScene 1.0

import MuseScore.Playback 1.0

Item {
    id: container

    property var popup: loader.item
    property bool isPopupOpened: Boolean(popup) && popup.isOpened


    property NavigationSection notationViewNavigationSection: null
    property int navigationOrderStart: 0
    property int navigationOrderEnd: Boolean(loader.item)
                                        ? loader.item.navigationOrderEnd
                                        : navigationOrderStart

    signal opened()
    signal closed()

    QtObject {
        id: prv

        function componentByType(type) {
            switch (type) {
            case Notation.TYPE_HARP_DIAGRAM: return harpPedalComp
            case Notation.TYPE_CAPO: return capoComp
            case Notation.TYPE_STRING_TUNINGS: return stringTuningsComp
            case Notation.TYPE_SOUND_FLAG: return soundFlagComp
            }

            return null
        }

        function loadPopup() {
            loader.active = true
        }

        function unloadPopup() {
            loader.sourceComponent = undefined
            loader.active = false

            Qt.callLater(container.closed)
        }

        function updateContainerPosition(elementRect) {
            container.x = elementRect.x
            container.y = elementRect.y
            container.height = elementRect.height
            container.width = elementRect.width

            loader.item.updatePosition()
        }
    }

    function show(elementType, elementRect) {
        prv.unloadPopup()
        prv.loadPopup()

        var popup = loader.createPopup(prv.componentByType(elementType), elementRect)
        popup.open()

        popup.opened.connect(function() {
            container.opened()
        })

        popup.closed.connect(function() {
            prv.unloadPopup()
        })
    }

    function close() {
        if (Boolean(container.popup) && container.popup.isOpened) {
            container.popup.close()
        }
    }

    Loader {
        id: loader

        anchors.fill: parent
        active: false

        function createPopup(comp, elementRect) {
            loader.sourceComponent = comp
            loader.item.parent = container

            prv.updateContainerPosition(elementRect)
            loader.item.elementRectChanged.connect(function(elementRect) {
                prv.updateContainerPosition(elementRect)
            })

            //! NOTE: All navigation panels in popups must be in the notation view section.
            //        This is necessary so that popups do not activate navigation in the new section,
            //        but at the same time, when clicking on the component (text input), the focus in popup's window should be activated
            loader.item.navigationSection = null

            loader.item.notationViewNavigationSection = container.notationViewNavigationSection
            loader.item.navigationOrderStart = container.navigationOrderStart

            return loader.item
        }
    }

    Component {
        id: harpPedalComp
        HarpPedalPopup {
        }
    }

    Component {
        id: capoComp
        CapoPopup {
        }
    }

    Component {
        id: stringTuningsComp
        StringTuningsPopup {
        }
    }

    Component {
        id: soundFlagComp
        SoundFlagPopup {
        }
    }
}
