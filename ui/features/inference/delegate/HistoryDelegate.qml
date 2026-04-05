import QtQuick
import QtQuick.Controls
import HistoryModel
import QtQuick.Layouts

ItemDelegate {
    id: itemDelegate
    width: parent.width
    height: 50

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        anchors.margins: 6

        RowLayout {
            id: rowLayout
            anchors.fill: parent
            spacing: 10

            // Disease Name
            Text {
                id: diseaseNameLabel
                Layout.fillWidth: true
                text: diseaseName
                elide: Text.ElideRight
                font.pixelSize: 16
                verticalAlignment: Text.AlignVCenter
                color: "#333"
            }

            // Delete icon
            Item {
                width: 26
                height: 26

                Image {
                    id: deleteIcon
                    anchors.fill: parent
                    source: "qrc:/assets/infarence/delete.png"
                    fillMode: Image.PreserveAspectFit
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        HistoryModel.deleteHistory(index)
                    }
                }
            }
        }
    }

    onClicked: {
        /*mainLoader.item.mainStackView.push("../Pages/InfarenceHistoryPage.qml",{
                                               "classIndex": classIndex,
                                               "diseaseName": diseaseName,
                                               "confidence": confidence
                                           })
        mainRoot.drawer.close()*/
    }
}
