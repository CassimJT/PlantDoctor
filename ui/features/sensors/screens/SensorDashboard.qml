import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../pages"

Page {
    id: sensorDashboard
    SwipeView{
        id: view
        currentIndex: 0
        anchors.fill: parent

        TempeatureHumidity{

        }
        AirQuality{

        }

    }

    Timer {
            id: autoSwipe
            interval: 3000
            running: true
            repeat: true
            onTriggered: {
                if (view.currentIndex < view.count - 1) {
                    view.currentIndex++
                } else {
                    view.currentIndex = 0
                }
            }
        }


    PageIndicator {
        id: indicator

        count: view.count
        currentIndex: view.currentIndex

        anchors.bottom: view.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        delegate: Rectangle {
                   width: index === view.currentIndex ? 16 : 8
                   height: 8
                   radius: 4
                   color: index === view.currentIndex ? "#1A2E1F" : "#8FAF8F"
                   anchors.verticalCenter: parent.verticalCenter

                   Behavior on width { NumberAnimation { duration: 200 } }
                   Behavior on color { ColorAnimation { duration: 200 } }
               }
    }
    //to do
    /*
        add swip animation
        addswip conroll
        add a live monitobuttun
    */
}
