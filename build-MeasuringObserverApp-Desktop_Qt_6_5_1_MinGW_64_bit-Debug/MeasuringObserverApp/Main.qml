import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
ApplicationWindow {
    id: applicationWindow
    Material.theme: Material.Light
    Material.accent: Material.Blue
    Material.primary: Material.Blue
    width: 400
    height: 800
    visible: true
    title: qsTr("BLE Health Thermometer")
    property bool disconnect: true
    header: ToolBar {
        contentHeight: toolButtonScan.implicitHeight
        Row{
            ToolButton {
                id: toolButtonScan
                text: "\u2630"
                font.pixelSize: Qt.application.font.pixelSize * 1.6
                onClicked: {
                    scanButton.enabled=true;
                    scanButton.text = disconnect ? "Scan" : "Disconnect"
                    drawer.open()
                }
            }
            ToolSeparator {}
            Label{
                id: timeLabel
                y:12
                text: " "
                font.pixelSize: Qt.application.font.pixelSize * 1.5
            }
        }
        Image {
            id: batteryIcon
            source: "batt.png"
            width: 32
            height: 32
            y:8
            anchors.right: parent.right
            anchors.rightMargin: 10
        }
        Label {
            id: battText
            anchors.right: batteryIcon.left
            anchors.rightMargin: 6
            y:16
            text: "---%"
        }
    }
    Drawer {
        id: drawer
        width: 250
        height: applicationWindow.height
        Button {
            id: scanButton
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width-20
            text: "Scan"
            onClicked: {
                listView.enabled=false
                if(disconnect) {
                    text="Scanning..."
                    enabled = false
                    busyIndicator.running=true
                    bledevice.startScan()
                } else {
                    bledevice.disconnectFromDevice()
                }
            }
        }
        ListView {
            id: listView
            anchors.fill: parent
            anchors.topMargin: 50
            anchors.bottomMargin: 50
            width: parent.width
            clip: true
            model: bledevice.deviceListModel
            delegate: RadioDelegate {
                id: radioDelegate
                text: (index+1)+". "+modelData
                width: listView.width
                onCheckedChanged: {
                    console.log("checked", modelData, index)
                    scanButton.enabled=false;
                    scanButton.text="Connecting to "+modelData
                    listView.enabled = false;
                    bledevice.startConnect(index)
                }
            }
        }
        BusyIndicator {
            id: busyIndicator
            Material.accent: "Blue"
            anchors.centerIn: parent
            running: false
        }
    }
    Gauge {
        id: gaugeTemperature
        anchors.centerIn: parent
        size: (parent.width < parent.height ? parent.width : parent.height) * 0.8
        colorCircle: "cyan"
        colorBackground: "#808080"
        lineWidth: 0.1*width
        showGlow: false
        quantity: "Temperature"
        unit: "\xB0C"
        from:30
        to:50
        min: 30
        value: 0
    }
    footer: ToolBar {
        width: parent.width
        Label{
            id: intermediateTemperatureLabel
            anchors.left: parent.left
            anchors.leftMargin: 10
            y:12
            text: " "
            font.pixelSize: Qt.application.font.pixelSize * 1.5
        }
    }
    Connections {
        target: bledevice
        function onBatteryLevel(batt) {
            battText.text=+batt+"%"
        }
        function onNewIntermediateTemperature(data) {
            intermediateTemperatureLabel.text = "Intermediate: "+ data[0].toFixed(1) + " \xB0C ";
            timeLabel.text = data[1]
        }
        function onNewTemperature(data) {
            gaugeTemperature.value = data[0]
            if(data[0] >= 37.5)
                gaugeTemperature.colorCircle = "red"
            else
                gaugeTemperature.colorCircle = "cyan"
            timeLabel.text = data[1]
        }
        function onScanningFinished() {
            listView.enabled = true
            scanButton.enabled = true
            scanButton.text = "Scan"
            listView.enabled = true
            busyIndicator.running = false
            scanButton.enabled = true
            console.log("ScanningFinished")
        }
        function onConnectionStart() {
            disconnect = false
            busyIndicator.running = false
            drawer.close()
            console.log("ConnectionStart")
        }
        function onConnectionEnd() {
            disconnect = true
            scanButton.text = "Connection End - Scan again"
            scanButton.enabled = true
            bledevice.resetDeviceListModel()
            console.log("ConnectionEnd")
        }
    }
}
