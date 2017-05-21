import QtQuick 2.6
import QtQuick.Layouts 1.3
import com.kde.kipiplugins 0.1 

Rectangle {
    id: root
    border.width: 3
    border.color: "green"
    height: ListView.view.height
    width: image.width

    ImageInfo {
        id: imageInfo
        url: model.url
    }

    Image {
        id: image
        x: 0
        y: 0
        height: parent.height
        width: 40 // We need to have non-zero width at all times, otherwise views overloads
        asynchronous : true
        fillMode: Image.PreserveAspectFit
        sourceSize.height : parent.height
        sourceSize.width : 0
        source: {
                if( root.ListView.view.mode === 0) {
                    return model.thumbnailUrl;
                } else if( root.ListView.view.mode === 1) {
                    console.log("requesting preview", model.previewUrl);
                    return model.previewUrl;
                } else if( root.ListView.view.mode === 2) {
                    return model.url
                } else {
                    console.error("Please select photo display mode");
                }
        }
        states : [
            State {
                name: "unloaded"
                when: image.status !== Image.Ready || image.implicitWidth === 0
                PropertyChanges {
                    target: image
                    width: image.height
                }
            },
            State {
                name: "loaded"
                when: image.status === Image.Ready && image.implicitWidth !== 0
                PropertyChanges {
                    target: image
                    width: implicitWidth
                }
                /* TODO: REMOVE
                StateChangeScript {
                    name: "dbgScript"
                    script : { console.log("Implicit width: ", image.implicitWidth);}
                }*/
            }
        ]
    }

    MouseArea {
        anchors.fill: parent
        onClicked: { console.log(
                        "state: ", image.state,
                        "source size: ", image.sourceSize, " width,height: ",
                        image.width, ",", image.height, " implicit: ", image.implicitWidth, ",",
                        image.implicitHeight );
                        mouse.accepted = false; }
    }
}
