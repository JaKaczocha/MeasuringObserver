import QtQuick

Item {
    ListModel {
        ListElement {
            channel: "123"

        }
        ListElement {
            channel: "456"

        }
        ListElement {
            channel: "789"

        }
    }
    ListView {
        width: 180; height: 200

        model: ContactModel {}
        delegate: Text {
            text: name + ": " + number
        }
    }
}
