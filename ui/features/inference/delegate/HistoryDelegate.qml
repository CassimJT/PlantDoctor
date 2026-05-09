import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import HistoryModel

ItemDelegate {
    id: itemDelegate
    width: parent.width
    height: 47
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    property bool isHovered: false

    // Background with hover effect
    Rectangle {
        anchors.fill: parent
        anchors.margins: 4
        radius: 10
        color: isHovered ? "#f0f4ef" : "transparent"

        Behavior on color {
            ColorAnimation { duration: 150 }
        }
    }

    RowLayout {
        id: rowLayout
        anchors {
            fill: parent
            leftMargin: 16
            rightMargin: 12
        }
        spacing: 12

        // Disease Name
        Text {
            id: diseaseNameLabel
            Layout.fillWidth: true
            text: diseaseName || "Unknown Disease"
            font.pixelSize: 15
            font.weight: Font.Medium
            color: "#2c3e2c"
            elide: Text.ElideRight
            maximumLineCount: 1
        }

        // Confidence (convert from 0.xxxxx to percentage)
        Text {
            text: Math.round((confidence || 0) * 100) + "%"
            font.pixelSize: 12
            color: {
                var percent = (confidence || 0) * 100
                if (percent >= 70) return "#4a7c59"
                if (percent >= 40) return "#e8a838"
                return "#d64e3a"
            }
            visible: confidence > 0
        }

        // Delete button - always visible and clickable
        Rectangle {
            width: 32
            height: 32
            radius: 16
            z: 10

            Image {
                id: deleteIcon
                width: 18
                height: 18
                anchors.centerIn: parent
                source: "qrc:/assets/infarence/delete.png"
                fillMode: Image.PreserveAspectFit
                opacity: 0.6
            }

            MouseArea {
                id: deleteArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onEntered: {
                    parent.color = "#ffeeee"
                    deleteIcon.opacity = 1
                }
                onExited: {
                    parent.color = "transparent"
                    deleteIcon.opacity = 0.6
                }

                onClicked: {
                    HistoryModel.deleteHistory(index)
                }
            }
        }
    }


    onClicked: {
        if (mainLoader && mainLoader.item && mainLoader.item.mainStackView) {
            mainLoader.item.mainStackView.push("../screens/InfarenceHistoryScreen.qml", {
                                                   "classIndex": classIndex,
                                                   "confidence": confidence
                                               })
            if (mainRoot && mainRoot.drawer) {
                mainRoot.drawer.close()
            }
        }
    }


    // Separator
    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            leftMargin: 16
            rightMargin: 16
        }
        height: 1
        color: "#e8ece5"
    }
}