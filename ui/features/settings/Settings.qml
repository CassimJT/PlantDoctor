import QtQuick 2.15
import QtQuick.Controls 2.15
import "../devices"
import "./screens"

Page {
    objectName: "Settings"

    TabBar {
        id: tabBar
        width: parent.width
        currentIndex: view.currentIndex
        anchors {
            top: parent.top
            topMargin: 70
        }

        TabButton {
            text: "System"
            icon.source: "qrc:/assets/appbar/settings.svg"
            onClicked: view.currentIndex = 0
        }
        TabButton {
            text: "Devices"
            icon.source: "qrc:/assets/settings/PND.svg"
            onClicked: view.currentIndex = 1
        }
    }

    SwipeView {
        id: view
        currentIndex: tabBar.currentIndex
        anchors {
            top: tabBar.bottom
            right: parent.right
            left: parent.left
            bottom: parent.bottom
        }

        SystemSettings { id: sysSettings }
        DevicesPage { id: d_settings }
    }
}