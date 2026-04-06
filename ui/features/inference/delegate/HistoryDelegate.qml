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
        anchors.margins: 8

        RowLayout {
            id: rowLayout
            anchors.fill: parent
            spacing: 10

            // Disease Name
            Text {
                id: diseaseNameLabel
                Layout.fillWidth: true
                text: {
                    if (diseaseName.length > 8) {
                        return diseaseName.substring(0, 10) + "............"
                    }
                    return diseaseName
                }
                font.pixelSize: 16
                verticalAlignment: Text.AlignVCenter
                color: "#333"
            }

            // Delete icon
            RoundButton {
                width: 28
                height: 28
                flat: true
                Image {
                    id: deleteIcon
                    width: 26
                    height: width
                    anchors.centerIn: parent
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
