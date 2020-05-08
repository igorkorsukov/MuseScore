import QtQuick 2.9
import MuseScore.NotationScene 1.0

Rectangle {

    id: root

    function cmd(name, args) {
        console.info("cmd: " + name)
        view.cmd(name);
    }

    NotationPaintView {
        id: view
        anchors.fill: parent
    }
}
