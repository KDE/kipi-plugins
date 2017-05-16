import QtQuick 2.6
import QtQuick.Layouts 1.3

Rectangle {
	property ImageCollection imageCollection : null
    width: 200
    height: 100
    border.width: 2
    border.color: "black"

	// Three flickables - thumbnails, preview, photo

	Component {
		id: thumbnailDelegate
		
		Image {
			height: ListView.view.height
			fillMode: Image.PreserveAspectFit
			source: model.modelData.thumbnailUrl
		}
	}
	Component {
		id: previewDelegate
		
		Image {
			height: ListView.view.height
			fillMode: Image.PreserveAspectFit
			source: model.modelData.previewUrl
		}
	}
	Component {
		id: photoDelegate
		
		Image {
			height: ListView.view.height
			fillMode: Image.PreserveAspectFit
			source: model.modelData.url
		}
	}

	ListView {
		id: thumbnailsView
		orientation : ListView.Horizontal
		anchors.right : parent.right
		anchors.left: parent.left
		anchors.top: parent.top
		height: parent.height / 4
		delegate: photoDelegate
		model : imageCollection
	}
	ListView {
		id: previewView
		orientation : ListView.Horizontal
		anchors.right : parent.right
		anchors.left: parent.left
		anchors.top: thumbnailsView.bottom
		height: parent.height / 4
		delegate: photoDelegate
		model : imageCollection
	}
	ListView {
		orientation : ListView.Horizontal
		anchors.right : parent.right
		anchors.left: parent.left
		anchors.top: previewView.bottom
		height: parent.height / 2
		delegate: photoDelegate
		model : imageCollection
	}
		

}

