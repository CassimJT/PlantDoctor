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
                    icon.source: {
                        if(mainLoader.item && mainLoader.item.mainStackView.depth > 1) {
                            return  "qrc:/assets/home/back.svg"
                        }else {
                            return  "qrc:/assets/home/menu.svg"
                        }
                    }

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
                    text:  mainRoot.mainStackView?.currentItem?.objectName || "PlantDoctor"


                    font.pixelSize: 14
                    font.weight: Font.Medium
                    Layout.leftMargin: 30
                }
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                RoundButton {
                    id: dotedmenu
                    Layout.rightMargin: 10
                    icon.source: "qrc:/assets/home/doted-menu.svg"
                    Material.background: "#f5f5f5"
                    icon.width: 26
                    icon.height: 26
                    Layout.alignment: Qt.AlignRight
                    visible: mainLoader.item && mainLoader.item.mainStackView.depth === 1 ? true : false
                    onClicked: {
                        appMenu.open()
                    }
                    Menu {
                        id: appMenu
                        onAboutToShow: {
                            x = dotedmenu.width - width
                            y = dotedmenu.height
                        }

                        MenuItem {
                            icon.source: "qrc:/assets/appbar/dashboard.svg"
                            text: qsTr("Sensors")
                            onClicked: {
                                mainLoader.item && mainLoader.item.mainStackView.push("../features/sensors/screens/SensorDashboard.qml")
                            }
                        }

                        MenuSeparator{}

                        MenuItem {
                            icon.source: "qrc:/assets/appbar/pest.svg"
                            text: qsTr("Live Monitor")
                            onClicked: {
                                mainLoader.item && mainLoader.item.mainStackView.push("../features/sensors/screens/LiveMonitorScreen.qml")

                            }
                        }

                        MenuSeparator{}

                        MenuItem {
                            icon.source: "qrc:/assets/appbar/settings.svg"
                            text: qsTr("Settings")
                            onClicked: {
                                mainLoader.item && mainLoader.item.mainStackView.push("../features/sensors/screens/SettingsScreen.qml")

                            }
                        }
                        MenuSeparator{}

                        MenuItem {
                            icon.source: "qrc:/assets/appbar/hotline.svg"
                            text: qsTr("Hotlines")
                            onClicked: {
                                mainLoader.item && mainLoader.item.mainStackView.push("../features/sensors/screens/Hotlines.qml")

                            }
                        }
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

