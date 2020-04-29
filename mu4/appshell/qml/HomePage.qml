import QtQuick 2.9
import MuseScore.Ui 1.0
import MuseScore.Dock 1.0

DockPage {
    id: homePage

    objectName: "home"

    panels: [
        DockPanel {
            id: resourcesPanel
            objectName: "resourcesPanel"

            //title: "Resources"
            width: 200
            color: ui.theme.window

            Rectangle {

                ListView {
                    anchors.fill: parent
                    model: 6
                    delegate: Item {
                        height: 40
                        anchors.left: parent.left
                        anchors.right: parent.right
                        Text {
                            anchors.fill: parent
                            anchors.leftMargin: 20
                            verticalAlignment: Text.AlignVCenter
                            //color: "#ffffff"
                            text: "Resources " + model.index
                        }
                    }
                }
            }
        }
    ]

    central: DockCentral {
        id: homeCentral
        objectName: "homeCentral"

        Rectangle {

            //color: "#1D1E20"

            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                //color: "#ffffff"
                text: "Home"
            }
        }
    }
}
