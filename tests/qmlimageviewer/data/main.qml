import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3

/*
Rectangle {
    width: 200
    height: 100
    color: "red"

    Text {
        anchors.centerIn: parent
        text: "Hello, World!"
    }
}*/

Window {
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")


    Rectangle {
        width: 200
        height: 100
        color: "red"

        Text {
            anchors.centerIn: parent
            text: "Hello, World!"
        }
    }
}
