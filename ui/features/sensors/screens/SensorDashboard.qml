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
        interactive: true
       //swipe control
        contentItem: ListView {
                    model: view.contentModel
                    interactive: view.interactive
                    currentIndex: view.currentIndex
                    focus: view.focus

                    spacing: view.spacing
                    orientation: view.orientation
                    snapMode: ListView.SnapOneItem
                    boundsBehavior: Flickable.StopAtBounds

                    highlightRangeMode: ListView.StrictlyEnforceRange
                    preferredHighlightBegin: 0
                    preferredHighlightEnd: 0
                    highlightMoveDuration: 400
                    flickDeceleration: 2500
                    maximumFlickVelocity: 2500

                    // pause auto swipe when user is swiping
                    onDraggingChanged: {
                        if (dragging) {
                            autoSwipe.stop()
                        } else {
                            autoSwipe.restart()
                        }
                    }
                }

        TempeatureHumidity{

        }
        AirQuality{

        }

    }



   // swipe timer
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

    Rectangle {
        z: 10
        anchors {
            bottom: indicator.top
            bottomMargin: 16
            horizontalCenter: parent.horizontalCenter
        }
        width: 220
        height: 60
        radius: 15
        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop { position: 0.0; color: "#34c45a" }
            GradientStop { position: 1.0; color: "#5dde7a" }
        }
        Rectangle {
            anchors { top: parent.top; left: parent.left; right: parent.right }
            height: parent.height / 2
            radius: parent.radius
            color: "#1affffff"
        }

        MouseArea {
              id: liveMA
              anchors.fill: parent
              onClicked: {
                  mainStackView.push("LiveMonitorScreen.qml")
              }
          }


        Row {
            anchors.centerIn: parent
            spacing: 8
            Image {
                 width: 20; height: 20
                 source: "qrc:/assets/appbar/pest.svg"
                 fillMode: Image.PreserveAspectFit
                 anchors.verticalCenter: parent.verticalCenter
             }


            Text {
                text: "Live Monitor"
                font.family: "Georgia"
                font.pixelSize: 15
                font.bold: true
                color: "#FFFFFF"
                anchors.verticalCenter: parent.verticalCenter
            }
        }


    }

        }

    //to do
    /*
        add swip animation
        addswip conroll
        add a live monitobuttun
    */

