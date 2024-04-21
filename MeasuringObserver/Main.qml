import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtCharts 2.15

ApplicationWindow {
    id: root
    Material.theme: Material.Dark
    Material.accent: Material.Orange
    width: 800
    height: 600
    visible: true
    title: qsTr("IoT Air")



    Frame {
        id: frameText
        height: 60
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        topPadding: 5;
        bottomPadding: 5;
        leftPadding: 5;

        ComboBox {

            id: comboBoxChannels
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.horizontalCenter;
            anchors.rightMargin: parent.width / 4

            currentIndex: 0;
            displayText: model.get(currentIndex).text
            model: ListModel {
                //add channels id
            }
            onCurrentIndexChanged:  {
                appcore.channel = model.get(currentIndex).text
            }



            font.pixelSize: 14
        }

        Button {
            anchors.left: comboBoxChannels.right
            anchors.leftMargin: 2;
            width: comboBoxChannels.width / 4
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            background: Rectangle {
                color: "#202020"
                border.color: "#707070"
                border.width: 1
                radius: 4
            }

            Text {
                id: buttonText
                anchors.centerIn: parent;
                color: "#ffffff"
                text: "Add.."
            }
            onPressed: {
                channelDialog.open()
            }
        }


    }

    Frame {
        id: frameGauge
        height: width/3
        anchors.top: frameText.bottom
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10

        Gauge {
            id: gaugePressure
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: -0.3*parent.width
            size: 0.8*parent.height
            colorCircle: "#00C0ff"
            colorBackground: "#202020"
            lineWidth: 0.1*width
            showGlow: true
            quantity: "Pressure"
            unit: "hPa"
            from:800
            to:1200
            value: from
        }
        Gauge {
            id: gaugeTemperature
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: 0
            size: parent.height
            colorCircle: "#ff8000"
            colorBackground: "#202020"
            lineWidth: 0.1*width
            showGlow: true
            quantity: "Temperature"
            unit: "\xB0C"
            from:0
            to:85
            value: from
        }
        Gauge {
            id: gaugeHumidity
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: 0.3*parent.width
            size: 0.8*parent.height
            colorCircle: "#00ffc0"
            colorBackground: "#202020"
            lineWidth: 0.1*width
            showBackground: true
            showGlow: true
            quantity: "Humidity"
            unit: "%"
            from:10
            to:100
            value: from
        }
    }

    ChartView {
        id: line
        anchors.top: frameGauge.bottom
        anchors.topMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.bottom: parent.bottom
        theme: ChartView.ChartThemeDark
        ValuesAxis {
            id: axisX
            min: 1
            max: 10
            tickCount: max
        }
        ValuesAxis {
            id: axisY
            min: 0
            max: 120
            tickCount: 7
        }
        LineSeries {
            id: series1
            name: "Pressure x10 [hPa]"
            axisX: axisX
            axisY: axisY
            color: "#00C0ff"
        }
        LineSeries {
            id: series2
            name: "Temperature [\xB0C]"
            axisX: axisX
            axisY: axisY
            color: "#ff8000"
        }
        LineSeries {
            id: series3
            name: "Humidity [%]"
            axisX: axisX
            axisY: axisY
            color: "#00ffc0"
        }
    }

    Connections {
        target: appcore
        function onNewData(v) {
            var channel = appcore.channel
            //console.log(v, v.length)
            if (v.length > 0) {
                axisX.max = v.length
                axisX. tickCount = axisX.max

                gaugeTemperature.value = v[v.length-1][0]
                gaugePressure.value = v[v.length-1][1]
                gaugeHumidity.value = v[v.length-1][2]
                series1.clear();
                series2.clear();
                series3.clear();
                for (var i = 0; i < v.length; i++) {
                    series2.append(v.length-i, v[i][0])
                    series1.append(v.length-i, v[i][1]/10)
                    series3.append(v.length-i, v[i][2])
                }
            }
        }
    }

    DialogChannelChange {
        id: channelDialog
        onNewChannelAccepted: {
            comboBoxChannels.model.append({ "text": newChannelName })
            comboBoxChannels.currentIndex = comboBoxChannels.count - 1
        }
    }
}
