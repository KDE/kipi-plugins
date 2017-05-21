import QtQuick 2.6
import QtQuick.Layouts 1.3

Rectangle {
	property var target : null
        property var title : "Unnamed"
        //border.width: 4
        //border.color: "green"
        height: childrenRect.height

        Text {
            id: titleObject
            text: title
            anchors.top: parent.top
            anchors.horizontalCenter : parent.horizontalCenter
            height: contentHeight
            font.pointSize: 12
        }
	Column {
            id: grid
            width: parent.width
            anchors.top: titleObject.bottom
            height: childrenRect.height

            Repeater {
                id: propertiesRepeater
                model: propertiesModel
                delegate: propertyDelegate
            }

	}

        ListModel {
            id: propertiesModel
        }

	Component {
		id: propertyDelegate
		
                Rectangle {
                    //border.color: "black"
                    //border.width: 2
                    width: grid.width
                    //height: childrenRect.height
                    height: 40
                    readonly property int margin : Math.max( width / 10, 5)
                    Text {
                        id: propertyNameText
                        width: parent.width / 2
                        height: contentHeight
                        anchors.left : parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.leftMargin : margin
                        anchors.rightMargin : margin
                        text: model.name
                    }
                    Text {
                        id: propertyValueText
                        width: parent.width / 2
                        height: contentHeight
                        anchors.right : parent.right
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.leftMargin : margin
                        anchors.rightMargin : margin
                        text : "\"" + model.value + "\""
                    }
                }
	}

	Component.onCompleted : updateTargetProperties();
	
	onTargetChanged : updateTargetProperties();

	function updateTargetProperties() {
            propertiesModel.clear();
            if( target !== null && target !== undefined ) {
                for( var p in target ) {
                    if( target.hasOwnProperty(p) && typeof(target[p]) != 'function' ) {
                        console.log("OI: adding property", p, " of type: ", typeof(target[pName]) );
                        var pName = p;
                        if( pName === "images" ) {
                            continue; // images is concatination of all urls, it's waay to long
                        }
                        var pValue = "".concat(target[pName]);
                        var element = { "name" : pName, "value" : pValue }
                        propertiesModel.append(element)
                        console.log("OI: \tadded property", p, " value: ", pValue);
                    }
                }
            }
            console.log( "Properties count in model: ", propertiesModel.count )
            propertiesRepeater.model = propertiesModel
	}
}

