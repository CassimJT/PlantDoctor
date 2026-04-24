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
    property var mainStackView

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
                Helper.setIsCamera(true)
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
                       close.accepted = true
                   }
               }

    Component.onCompleted: {
        Helper.setStatusBarAppearance(Qt.rgba(0,0,0,0),true)
        mainStackView = mainLoader.item.mainStackView
    }
    // This stays active and will trigger every time alarm fires
    Connections {
        target: CloudSynch

        // Connect to startSynchInvoked (emitted after notification is shown)
        function onStartSynchInvoked() {
            console.log("=== ALARM FIRED - Starting cloud sync ===")
            var unsyncedEntries = HistoryModel.getUnsyncedEntriesArray()

            if (unsyncedEntries.length > 0) {
                console.log("Syncing", unsyncedEntries.length, "unsynced entries")
                APIClient.createBatchInferencesQml(unsyncedEntries)
            } else {
                console.log("No unsynced entries to sync")
            }
        }

        // Optional: Debug to see when workerInvoked happens
        function onWorkerInvoked() {
            console.log("workerInvoked received - notification shown")
        }
    }

    // Handle batch sync result
    Connections {
        target: APIClient
        function onBatchCreateFinished(success, totalCount, successCount, response) {
            if (success) {
                console.log("Sync successful - Total:", totalCount, "Succeeded:", successCount)
                if (successCount === totalCount) {
                    HistoryModel.markAllAsSynced()
                }
            } else {
                console.error("Sync failed:", response ? response["message"] : "Unknown error")
            }
        }

        function onBatchProgress(current, total, currentDisease) {
            console.log("Sync progress:", current, "/", total)
        }

        function onNetworkError(error) {
            console.error("Network error during sync:", error)
        }
    }
}
