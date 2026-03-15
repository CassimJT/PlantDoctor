import QtQuick 2.15
import QtQuick.Controls 2.15
import "../features/home/screens"

Item {
    id: root
    property alias mainStackView: mainStackView

    StackView {
        id: mainStackView
        anchors.fill: parent
        initialItem: HomeScreen {}  // Make sure HomeScreen is defined
    }
}
