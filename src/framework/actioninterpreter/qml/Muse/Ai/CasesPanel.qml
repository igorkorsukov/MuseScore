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

    objectName: "DiagnosticCasesPanel"
    color: ui.theme.backgroundPrimaryColor

    CasesViewModel {
        id: cmodel
    }

    Component.onCompleted: {
        cmodel.init()
    }

    Item {
        id: leftPanel
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 200

        FlatButton {
            id: addBtn
            anchors.left: parent.left
            anchors.top: parent.top
            icon: IconCode.PLUS
            onClicked: cmodel.addNewCase()
        }

        StyledListView {
            anchors.top: addBtn.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            model: cmodel.cases

            delegate: ListItemBlank {
                property var item: modelData
                StyledTextLabel {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignLeft
                    text: item.name
                }

                onClicked: cmodel.selectCase(model.index)
            }
        }
    }

    Item {
        id: mainPanel
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: leftPanel.right
        anchors.right: parent.right
        anchors.margins: 16

        TextInputField {
            anchors.left: parent.left
            anchors.right: parent.right
            currentText: cmodel.currentCase.name
            onTextChanged: (text) => { cmodel.currentCase.name = text }
        }

        Item {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 40

            FlatButton {
                id: saveBtn
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                icon: IconCode.SAVE
                onClicked: cmodel.saveCurrent()
            }
        }
    }
}
