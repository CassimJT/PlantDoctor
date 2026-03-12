import QtQuick 2.15
import QtQuick.Controls
import "../../inference/delegate"
import QtCore
import QtQml.Models
import HistoryModel
Drawer {
    id: drawer
    width: parent.width * 0.85
    height: parent.height
    padding: 10
    //----SearchField
    SearchField {
        id: searchField
        suggestionModel: filterModel
        textRole: "diseaseName"
        anchors {
            top: parent.top
            right: parent.right
            left: parent.left
            margins: 10
        }
        onTextChanged: {
            filterModel.invalidate()
        }
    }
    //-----SortFilterProxyModel
    SortFilterProxyModel {
        id: filterModel
        model: HistoryModel.listmodel
        filters: [
            FunctionFilter {
                component RoleData: QtObject {
                    property string diseaseName
                }
                function filter(data: RoleData) : bool {

                    if (searchField.text === "")
                        return true
                    return data.diseaseName && data.diseaseName.toLowerCase().includes(searchField.text.toLowerCase())
                }
            }
        ]
    }
    Label {
        id: hisoryLable
        text: qsTr("History")
        font.pointSize: 20
        anchors {
            top: searchField.bottom
            left: parent.left
            margins: 10
        }
    }
    Rectangle {
        id: frame
        width: 1
        color: "#9E9E9E"
        anchors {
            top: hisoryLable.bottom
            left: parent.left
            bottom: parent.bottom
            leftMargin: 25
            topMargin: 10
            bottomMargin: 10
        }
    }
    ListView {
        id: view
        clip: true
        model: filterModel
        delegate: HistoryDelegate{}

        section {
            property: "date"
            criteria: ViewSection.FullString
            labelPositioning: ViewSection.InlineLabels
            delegate: ItemDelegate {
                width: parent.width
                Label {
                    id: dateLabel
                    text: section
                    font.pixelSize: 14
                    color: "#666"
                    anchors.centerIn: parent
                }
            }
        }

        anchors {
            top: hisoryLable.bottom
            right: parent.right
            bottom: parent.bottom
            left: frame.right
        }
    }
    RoundButton {
        id: roundBtn
        width: 56
        height: width
        z: 10
        radius: width / 2
        Material.elevation: 8
        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: 25
        }
        Image {
            id: clear
            source: "qrc:/assets/home/clear.png"
            width: 28
            height: width
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
        }
        onClicked: {
            clearAll.open()
        }
    }
    Component.onCompleted: {
        HistoryModel.loadHistory()
        filterModel.invalidate()

    }
    Dialog {
        id: clearAll
        title: "clear all history?"
        standardButtons: Dialog.Yes | Dialog.No
        anchors.centerIn: parent
        onAccepted:  {
            HistoryModel.clearModel()
           // filterModel.invalidate()
        }
    }
}
