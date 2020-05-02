import QtQuick 2.9
import MuseScore.Dock 1.0

import "./HomePage"

DockWindow {

    id: dockWindow

    title: "MuseScore Craft"

    color: ui.theme.window

    currentPageName: "home"

    toolbar: DockToolBar {

        id: windowToolBar
        objectName: "windowToolBar"

        width: 300
        height: 28
        color: dockWindow.color

        MainToolBar {
            color: windowToolBar.color
            onSelected: {
                dockWindow.currentPageName = item;
            }
        }
    }


    HomePage {

    }

    NotationPage {

    }

    SequencerPage {

    }

    PublishPage {

    }

}
