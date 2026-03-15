import QtQuick 2.15
import QtQuick.Controls
import "../features//home/screens"


Item {
    id: root
    property alias mainStackView: mainStackView
    StackView {
        id: mainStackView
        anchors.fill: parent
        initialItem: HomeScreen {}
    }
}
