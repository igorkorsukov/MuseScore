import QtQuick 2.9
import QtQuick.Controls 2.2
import MuseScore.Ui 1.0
import MuseScore.Dock 1.0
import MuseScore.NotationScene 1.0

DockPage {
    id: notationPage
    objectName: "notation"

    property var color: ui.theme.window

    QtObject {
        id: observer

        property var _callbacks: ({})
        function register(target, callback) {
            _callbacks[target] = callback;
        }

        function call(target, method, args) {
            _callbacks[target](method, args)
        }
    }

    toolbar: DockToolBar {
        id: notationToolBar
        objectName: "notationToolBar"

        height: 40
        width: 400
        color: notationPage.color

        NotationToolBar {
            onClicked: observer.call("view", name)
        }
    }

    panels: [
        DockPanel {

            id: palettePanel
            objectName: "palettePanel"

            title: "Palette"
            width: 200
            color: notationPage.color

            Rectangle {

                Text {
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: "Palette"
                }
            }
        },

        DockPanel {

            id: inspectorPanel
            objectName: "inspectorPanel"

            title: "Inspector"
            width: 200
            color: notationPage.color

            //area: Qt.RightDockWidgetArea
            tabifyObjectName: "palettePanel"

            Rectangle {

                Text {
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: "Inspector"
                }
            }
        }
    ]

    central: DockCentral {
        id: notationCentral
        objectName: "notationCentral"

        NotationView {
            id: notationView

            function onCall(method, args) {
                console.info("[view] onCall: " + method + "(" + args + ")")
                switch (method) {
                case "open": notationView.open()
                }
            }

            Component.onCompleted: {
                observer.register("view", onCall)
            }
        }
    }

    statusbar: DockStatusBar {

        id: notationStatusBar
        objectName: "notationStatusBar"

        width: notationPage.width
        color: notationPage.color

        Rectangle {
            color: notationStatusBar.color

            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                text: "Notation status bar"
            }
        }
    }
}
