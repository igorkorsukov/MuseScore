import QtQuick 2.9
import MuseScore.NotationScene 1.0

Rectangle {

    id: root


    function open() {
        view.open()
    }

    NotationPaintView {
        id: view
        anchors.fill: parent
        
        
    }
}
