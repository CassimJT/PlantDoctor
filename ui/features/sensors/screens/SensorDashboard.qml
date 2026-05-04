import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../pages"

Page {
    id: sensorDashboard
    SwipeView{
        id: view
        currentIndex: 0
        anchors.fill: parent

        TempeatureHumidity{

        }
        AirQuality{

        }

    }

}
