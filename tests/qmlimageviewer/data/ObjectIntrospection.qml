import QtQuick 2.6
import QtQuick.Layouts 1.3

Rectangle {
	property var target : null
    border.width: 2
    border.color: "black"

	GridLayout {
		id: grid
		anchors.fill: parent
		columns : 2
		flow: GridLayout.LeftToRight
	}

	Component {
		id: textObject
		
		Text {}
	}

	Component.onCreated : updateTargetProperties();
	
	onTargetChanged : updateTargetProperties();

	function updateTargetProperties() {
		grid.children = ""
		
		if( target !== null && target !== undefined ) {
			for( var p in target ) {
				if( target.hasOwnProperty(p) ) {
					var pName = textObject.createObject( grid, { 'text' : JSON.stringify(pName) } );
					var pValue = textObject.createObject( grid, { 'text' : JSON.stringify(target[pName]) } );
				}
			}
		}
	}
}

