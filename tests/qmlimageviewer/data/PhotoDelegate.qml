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
        width: implicitWidth
        asynchronous : true
        fillMode: Image.PreserveAspectFit
        sourceSize.height : parent.height
        source: {
                if( root.ListView.view.modeThumbnail) {
                    return imageInfo.thumbnailUrl;
                } else if( root.ListView.view.modePreview) {
                    return imageInfo.previewUrl;
                } else if( root.ListView.view.modePhoto) {
                    return imageInfo.url
                } else {
                    console.error("Please select photo display mode");
                }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: { console.log("source size: ", image.sourceSize, " width,height: ",
                image.width, ",", image.height, " implicit: ", image.implicitWidth, ",",
                image.implicitHeight ); mouse.accepted = false; }
    }
}
