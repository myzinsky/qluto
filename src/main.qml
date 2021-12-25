import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.4
import Qt.labs.settings 1.0
import Qt.labs.platform 1.1
import HamTools 1.0

Window {
    width: 4096
    height: 480
    visible: true
    title: qsTr("Hello World")

    Waterfall {
        objectName: "Waterfall"
        id: wf1
        anchors.fill: parent
        dataSource: fft
    }

    GridLayout {
        id: grid
        columns: 2

        Button {
            id: connectButton
            text: qsTr("Connect")
            onClicked: {
                pluto.connect();
            }
        }

        Button {
            id: startButton
            enabled: false;
            text: qsTr("Start")
            onClicked: {
                pluto.start();
            }
        }
    }

    MessageDialog {
        id: messageDialog
        buttons: MessageDialog.Ok
    }

  Connections{
        target: pluto

        function onConnectionError(message) {
            messageDialog.text = message;
            messageDialog.open();
        }

        function onConnected() {
            startButton.enabled = true;
        }
    }
}
