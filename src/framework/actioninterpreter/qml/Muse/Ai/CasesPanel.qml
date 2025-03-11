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
        width: 160

        FlatButton {
            id: addBtn
            anchors.left: parent.left
            anchors.top: parent.top
            icon: IconCode.PLUS
            onClicked: cmodel.addNewCase()
        }

        StyledListView {
            anchors.top: addBtn.bottom
            anchors.topMargin: 16
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            model: cmodel.cases

            delegate: ListItemBlank {
                property var item: modelData
                StyledTextLabel {
                    width: parent.width
                    horizontalAlignment: Text.AlignLeft
                    maximumLineCount: 1
                    elide: Text.ElideRight
                    text: item.text
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
        anchors.topMargin: addBtn.height + 16
        anchors.leftMargin: 16
        anchors.rightMargin: 16

        Column {
            anchors.fill: parent
            spacing: 16

            TextInputArea {
                id: textArea
                anchors.left: parent.left
                anchors.right: parent.right
                height: 120
                currentText: cmodel.currentCase.text
                onTextEditingFinished: (text) => { cmodel.changeCurrentText(text) }
            }

            TextInputArea {
                id: actionsArea
                anchors.left: parent.left
                anchors.right: parent.right
                height: 220
                inputField.wrapMode: Text.WordWrap
                inputField.width: parent.width - 24
                currentText: cmodel.currentCase.actions
                onTextEditingFinished: (text) => { cmodel.changeCurrentActions(text) }
            }

            FlatButton {
                id: runBtn
                anchors.right: parent.right
                text: "Run"
                onClicked: cmodel.runCase()
            }
        }
    }
}
