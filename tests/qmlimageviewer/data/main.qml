import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4

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

    TabView {
        anchors.fill : parent
        Tab {
            title: "Interface"
            id: introspectionTab
/*
            Grid {
                anchors.fill : parent
                columns : 2
                spacing: 5

                Text {
                    text: "currentAlbum"
                }
                Text {
                    text: KIPIInterface.currentAlbum.name
                } 
            }
*/
            Flickable {
                anchors.fill: parent
                contentWidth: mainColumn.width
                contentHeight: mainColumn.height
                Column {
                    id: mainColumn
                    width: introspectionTab.width
                    height: childrenRect.height
                    ObjectIntrospection {
                        id: kipiInterfaceIntrospection
                        target: KIPIInterface
                        title: "KIPI Interface"
                        width: parent.width
                        //anchors.top : parent.top
                        //anchors.left: parent.left
                        //anchors.right: parent.right
                    }
                    ObjectIntrospection {
                        id: currentAlbumIntrospection
                        target: KIPIInterface.currentAlbum
                        title: "Current Album Collection"
                        width: parent.width
                        //anchors.top : parent.
                        //anchors.left: parent.left
                        //anchors.right: parent.right
                    }
                    ObjectIntrospection {
                        id: currentSelectionIntrospection
                        target: KIPIInterface.currentSelection
                        title: "Current Selection Collection"
                        width: parent.width
                        //anchors.top : parent.
                        //anchors.left: parent.left
                        //anchors.right: parent.right
                    }
                }
            }
        }
        Tab {
            title: "Image list"
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
    }
}
