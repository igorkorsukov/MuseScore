import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.12

ToolBar {
    id: root

    signal clicked(string name)

    RowLayout {
        anchors.fill: parent

        ToolButton {
            text: "Open"
            onClicked: root.clicked("open")
        }

        ToolButton {
            text: "N"
            onClicked: root.clicked("noteinput")
        }

        ToolButton {
            text: "n16"
            onClicked: root.clicked("note16")
        }

        ToolButton {
            text: "n8"
            onClicked: root.clicked("note8")
        }

        ToolButton {
            text: "n4"
            onClicked: root.clicked("note4")
        }

        ToolButton {
            text: "n2"
            onClicked: root.clicked("note2")
        }

        ToolButton {
            text: "n0"
            onClicked: root.clicked("note0")
        }
    }
}
