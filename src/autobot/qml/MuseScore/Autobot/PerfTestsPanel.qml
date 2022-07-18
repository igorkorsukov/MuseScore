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
import QtQuick 2.15
import MuseScore.UiComponents 1.0
import MuseScore.Autobot 1.0

Rectangle {

    color: ui.theme.backgroundPrimaryColor

    objectName: "DiagnosticAutobotPerfTestsPanel"

    AutobotPerfTestsModel {
        id: perftestsModel
    }

    Component.onCompleted: {
        perftestsModel.load()
    }

    Item {
        id: topPanel
        anchors.left: parent.left
        anchors.right: parent.right
        height: 48

        FlatButton {
            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.verticalCenter: parent.verticalCenter
            text: "Run"
            onClicked: {

            }
        }
    }

    ListView {
        anchors.top: topPanel.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        clip: true

        model: perftestsModel

        delegate: ListItemBlank {
            anchors.left: parent ? parent.left : undefined
            anchors.right: parent ? parent.right : undefined
            height: 48

            StyledTextLabel {
                id: titleLabel
                anchors.fill: parent
                anchors.margins: 8
                horizontalAlignment: Text.AlignLeft
                text: titleRole
            }

            onClicked: {

            }
        }
    }
}
