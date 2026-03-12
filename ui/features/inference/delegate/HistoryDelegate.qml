import QtQuick
import QtQuick.Controls
import HistoryModel

ItemDelegate {
    id: itemDelegate
    width: parent.width
    height: 50
    Row {
        anchors.fill: parent
        anchors {
            margins: 10
            rightMargin:15
        }
        spacing: 15
        // Disease Name
        Text {
            id: diseaseNameLabel
            width: parent.width * 0.50
            text: diseaseName
            elide: Text.ElideRight
            font.pixelSize: 16
            anchors.verticalCenter: parent.verticalCenter
            color: "#ffffff"
        }

        // Date
        Label {
            id: dateLabel
            width: parent.width * 0.29
            text: date
            font.pixelSize: 14
            color: "#666"
             horizontalAlignment: Text.AlignLeft
            anchors.verticalCenter: parent.verticalCenter
        }

        // Delete icon
        Image {
            id: deleteIcon
            source: "qrc:/assets/com/delete.png"
            width: 24
            height: 24
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    HistoryModel.deleteHistory(index)
                }
            }
        }
    }
    onClicked: {
        //go to infareceHistor
        mainLoader.item.mainStackView.push("../Pages/InfarenceHistoryPage.qml",{
                                                "classIndex":classIndex,
                                                "diseaseName":diseaseName,
                                                "confidence":confidence
                                   })
        mainRoot.drawer.close()
    }
}
