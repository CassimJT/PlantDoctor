import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../utils/Utils.js" as Utills
import "../features/home"
Item {
    id: root
    signal switchClicked()
    signal themeChanged()
    signal menuClicked()
    property real h_height: 90
    Rectangle {
        id: header
        width: parent.width
        height: root.h_height
        color: Material.primary
        ToolBar {
            id: tooBar
            anchors {
                right: parent.right
                left: parent.left
                bottom: parent.bottom
                bottomMargin: 2
            }
            //Material.elevation: 7
            RowLayout{
                id: row
                visible: header.height === root.h_height
                width: parent.width
                height: parent.height
                RoundButton {
                    id: menu
                    Layout.leftMargin: 10
                    icon.source: "qrc:/assets/home/menu.svg"
                    //icon.color: "#333"
                    Material.background: "#f5f5f5"
                    icon.width: 26
                    icon.height: 26
                    //flat: true
                    onClicked: {
                        menuClicked()
                    }
                }
                Label {
                    id: tittle
                    text: qsTr("PlantDoctor")
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    Material.elevation: 2
                }

                RoundButton {
                    id: dotedmenu
                    Layout.rightMargin: 10
                    icon.source: "qrc:/assets/home/doted-menu.svg"
                    Material.background: "#f5f5f5"
                    //icon.color: "#333"
                    icon.width: 26
                    icon.height: 26
                    Layout.alignment: Qt.AlignRight
                    //flat: true
                    onClicked: {
                        //
                    }
                }
            }

        }

        //-----------the tabBar visible only when the header is expandede
        AppBarItemes {
            id: items
            visible: header.height === root.parent.height * 0.3  ? true : false

        }
    }

}

