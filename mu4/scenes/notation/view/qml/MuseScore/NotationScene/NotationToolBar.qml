import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.12

ToolBar {
    id: root

    signal clicked(string cmd)

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
            onClicked: root.clicked("pad-note-16")
        }

        ToolButton {
            text: "n8"
            onClicked: root.clicked("pad-note-8")
        }

        ToolButton {
            text: "n4"
            onClicked: root.clicked("pad-note-4")
        }

        ToolButton {
            text: "n2"
            onClicked: root.clicked("pad-note-2")
        }

        ToolButton {
            text: "n0"
            onClicked: root.clicked("note0")
        }
    }
}
