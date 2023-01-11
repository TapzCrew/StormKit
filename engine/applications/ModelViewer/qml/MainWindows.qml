import QtQuick 6.3
import QtQuick.Controls 6.3
import QtQuick.Layouts 6.3

import WinUI3Style 1.0
import WinUI3Style.Addons 1.0

MicaApplicationWindow {
    id: window

    width: 640
    height: 480

    minimumWidth: 500
    minimumHeight: 200

    visible: true
    title: qsTr("Model Viewer")

    WinUI3Style.theme: WinUI3Style.Theme.Dark

    titleBarExpanded: true

    titleBar: Row {
        spacing: 6

        leftPadding: 4
        topPadding: 4

        ToolButton {
            font.family: WinUI3Style.iconFont
            text: String.fromCodePoint(0xe8e5)

            ToolTip {
                text: qsTr("Open")
            }
        }

        Label {
            text: window.title

            font.pixelSize: 12
            font.weight: 450
            font.family: "Segoe UI Semibold"

            anchors.verticalCenter: parent.verticalCenter
        }
    }

    WindowContent {
        id: window_content

        anchors.fill: parent
    }
}
