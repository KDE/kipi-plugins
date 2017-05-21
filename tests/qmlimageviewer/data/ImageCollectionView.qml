import QtQuick 2.6
import QtQuick.Layouts 1.3
import com.kde.kipiplugins 0.1 

Rectangle {
    id: imageCollectionView
    property var imageCollection : null
    border.width: 2
    border.color: "black"

    ImageCollectionModel {
        id: imagesModel
        imageCollection : imageCollectionView.imageCollection
    }

/*
    onImageCollectionChanged : {
        imagesModel.clear();
        if( imageCollection !== null && imageCollection !== undefined ) {
            for( var i = 0; i < Math.min(10, imageCollection.images.length); i++ ) {
                console.log(i)
                var u = imageCollection.images[i];
                imagesModel.append( { "url" : u } );
            }
        }
    }
*/


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
            property int mode : 0
            id: thumbnailsView
            orientation : ListView.Horizontal
            delegate: PhotoDelegate {}
            model : imagesModel
            Layout.preferredWidth : imageCollectionView.width
            Layout.preferredHeight : imageCollectionView.height / 3
        }
        ListView {
            id: previewView
            property int mode : 1
            orientation : ListView.Horizontal
            Layout.preferredWidth : imageCollectionView.width
            Layout.preferredHeight : imageCollectionView.height / 3
            delegate: PhotoDelegate {}
            model : imagesModel
        }
        ListView {
            property int mode : 2
            orientation : ListView.Horizontal
            Layout.preferredWidth : imageCollectionView.width
            Layout.preferredHeight : imageCollectionView.height / 3
            delegate: PhotoDelegate {}
            model : imagesModel
        }
    }
}

