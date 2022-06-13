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

    WinUI3Style.theme: WinUI3Style.Theme.System

    onIsDarkThemeEnabledChanged: window.WinUI3Style.theme = WinUI3Style.System

    ColumnLayout {
        anchors.fill: parent

        id: layout

        RowLayout {
            Layout.fillWidth: true

            ToolButton {
                font.family: WinUI3Style.iconFont
                text: String.fromCodePoint(0xe8e5)

                ToolTip {
                    text: qsTr("Open")
                }

                Layout.rightMargin: 6
            }

            Label {
                text: window.title
                font.pixelSize: 12
                font.weight: 400
                font.family: "Segoe UI Semibold"

                Layout.alignment: Qt.AlignVCenter
            }
        }

        WindowContent {
            id: content

            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
