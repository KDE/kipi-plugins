import QtQuick 2.6
import QtQuick.Layouts 1.3
import com.kde.kipiplugins 0.1 

Rectangle {
    id: imageCollectionView
    property var imageCollection : null
    border.width: 2
    border.color: "black"

    ListModel {
        id: imagesModel
    }

    onImageCollectionChanged : {
        imagesModel.clear();
        if( imageCollection !== null && imageCollection !== undefined ) {
            for( var i = 0; i < 10/*imageCollection.images.length*/; i++ ) {
                console.log(i)
                var u = imageCollection.images[i];
                imagesModel.append( { "url" : u } );
            }
        }
    }


/*
    MouseArea {
        anchors.fill: parent
        onClicked : { console.log( "Collection: ", imageCollection, " images count: ", imageCollection.images.length); }
    }
*/
    // Three flickables - thumbnails, preview, photo
    ColumnLayout {
        anchors.fill : parent
        ListView {
            property bool modeThumbnail : true
            id: thumbnailsView
            orientation : ListView.Horizontal
            delegate: PhotoDelegate {}
            model : imagesModel
            Layout.preferredWidth : imageCollectionView.width
            Layout.preferredHeight : imageCollectionView.height / 3
        }
        ListView {
            id: previewView
            property bool modePreview : true
            orientation : ListView.Horizontal
            Layout.preferredWidth : imageCollectionView.width
            Layout.preferredHeight : imageCollectionView.height / 3
            delegate: PhotoDelegate {}
            model : imagesModel
        }
        ListView {
            property bool modePhoto : true
            orientation : ListView.Horizontal
            Layout.preferredWidth : imageCollectionView.width
            Layout.preferredHeight : imageCollectionView.height / 3
            delegate: PhotoDelegate {}
            model : imagesModel
        }
    }
}

