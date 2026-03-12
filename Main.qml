import QtQuick
import QtQuick.Controls
import "./ui/components"
import "./ui/features/home/componetes"
import HistoryModel
ApplicationWindow {
    id: mainRoot
    width: 350
    height: 580
    visible: true
    title: qsTr("PlantGPT")
    property bool isDarkTheme: false
    property alias drawer: drawer
    property bool exitConfirmed: false
    // Modern Android-friendly flags (Qt 6.9+)
    flags: {
        if (Qt.platform.os === "android") {
            return Qt.Window | Qt.ExpandedClientAreaHint | Qt.NoTitleBarBackgroundHint
        } else {
            return Qt.Window
        }
    }
    Material.primary : "#edf2e0"
    Material.accent : "#4CAF50"
    Material.background: "#edf2e0"

    //-------------- Header ---------------------------------------------------------
    header: AppBar {
        id: appHeader
        //when meni is menuClicked:
        onMenuClicked: {
            if(mainLoader.item && mainLoader.item.mainStackView.depth > 1) {
                mainLoader.item.mainStackView.pop()
            }else {
                drawer.open();
            }
        }
    }
    //---------- Loader ----------------------------------
    Loader {
        id: mainLoader
        anchors.fill: parent
        source:"./ui/app/MainStackView.qml"
    }
    //--------Drawe ------------------------------
    SideBar{
        id: drawer
    }

    //--------Dialog -------------------------------------------------
    Dialog {
        id: confirmExitDialog
        title: "Closng Application"
        Label{
            anchors.centerIn: parent
            text: "Are sure u want to close this app?"
            color: "red"
            font.weight: Font.Medium
        }

        standardButtons: Dialog.Yes | Dialog.No
        anchors.centerIn: parent
        onAccepted:  {
            HistoryModel.persistHistory()
            mainRoot.exitConfirmed = true
            confirmExitDialog.close()
            mainRoot.close()
        }
    }


    onClosing: (close) => {
                   if (mainLoader.item && mainLoader.item.mainStackView.depth > 1) {
                       close.accepted = false
                       mainLoader.item.mainStackView.pop()
                   } else if (!exitConfirmed) {
                       close.accepted = false
                       confirmExitDialog.open()
                   } else {
                       // Allow the close to proceed
                       close.accepted = true
                   }
               }

    Component.onCompleted: {
        Helper.setStatusBarAppearance(Qt.rgba(0,0,0,0),true)
    }
}
