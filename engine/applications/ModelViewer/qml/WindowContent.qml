import QtQuick 6.3
import QtQuick.Controls 6.3
import QtQuick.Layouts 6.3
import ModelViewer

Item {
    id: root

    anchors.leftMargin: 8

    GridLayout {
        id: main_layout

        anchors.fill: parent

        Item {
            Layout.fillHeight: true
            Layout.minimumWidth: 200

            ColumnLayout {
                anchors.fill: parent

                Button {
                    text: "Test"

                    Layout.fillWidth: true
                }
            }
        }

        Frame {
            id: viewer_frame

            Layout.fillHeight: true
            Layout.fillWidth: true

            clip: true

            StormKitView {
                x: 1
                y: 1
                width: parent.width - 2
                height: parent.height - 2

                clip: true
            }
        }
    }
}
