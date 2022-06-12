import QtQuick 6.3
import QtQuick.Controls 6.3

ApplicationWindow {
    id: window

    width: 640
    height: 480

    minimumWidth: 500
    minimumHeight: 200

    visible: true
    title: qsTr("Model Viewer")

    WindowContent {
        id: content

        anchors.fill: parent
    }
}
