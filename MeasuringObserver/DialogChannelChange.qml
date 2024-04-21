import QtQuick
import QtQuick.Controls

Dialog {

    id: customDialog
    title: "Add new channel"

    anchors.centerIn: parent

    signal newChannelAccepted
    property string newChannelName

    onVisibleChanged: {
        edtInput.text =  "enter channel id.."
    }

    Column {
        anchors.fill: parent
        Rectangle {

            anchors.left: parent.left
            anchors.right: parent.right
            height: 20;
            color:"lightgrey"
            TextInput {
                id: edtInput
                anchors.centerIn: parent
                text: "Podaj id kanału"

            }
            MouseArea {
                id: mouseAreaInput
                anchors.fill: parent

                onClicked: {
                    if("enter channel id.." == edtInput.text) {
                        edtInput.clear()
                        edtInput.focus = true
                    }
                }
            }
        }

        Row {
            id: dialogRow

            spacing: 20
            Button {
                width: 100
                text: "Cancel"

                background: Rectangle {
                    color: "grey"
                    radius: 5
                }

                onClicked: customDialog.close()
             }

            Button {
                width: 100
                text: "Add"

                background: Rectangle {
                    color: "green"
                    radius: 5
                }
                onClicked: {
                    newChannelName = edtInput.text // Przypisanie tekstu z TextInput do newChannelName
                    customDialog.newChannelAccepted()
                    customDialog.close()
                }
            }
        }
    }
}
