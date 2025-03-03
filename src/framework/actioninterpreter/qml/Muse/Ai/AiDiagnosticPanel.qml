/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2024 MuseScore BVBA and others
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

import Muse.Ui 1.0
import Muse.UiComponents 1.0
import Muse.Ai 1.0

Rectangle {

    id: root

    objectName: "DiagnosticAiPanel"
    color: ui.theme.backgroundPrimaryColor

    AiDiagnosticViewModel {
        id: aimodel
    }


    Item {
        id: topPanel
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 40
        height: 60

        FlatToggleButton {
            anchors.centerIn: parent
            icon: IconCode.STAR
            implicitHeight: 60
            implicitWidth: 60
            checked: aimodel.isRecording
            onToggled: aimodel.toogleRecord()
        }
    }

    Item {
        id: textPanel
        anchors.top: topPanel.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.right: parent.right
        height: (root.height - topPanel.height - statusPanel.height) / 2

        StyledListView {
            anchors.fill: parent
            model: aimodel.textList
            delegate: StyledTextLabel {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 16
                horizontalAlignment: Text.AlignLeft
                text: modelData
            }
        }
    }

    Item {
        id: statusPanel
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 40

        StyledTextLabel {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            text: aimodel.status
        }
    }
}
