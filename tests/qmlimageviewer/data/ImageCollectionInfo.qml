import QtQuick 2.6
import QtQuick.Layouts 1.3

Rectangle {
	property ImageCollection imageCollection : null
    width: 200
    height: 100
    border.width: 2
    border.color: "black"

	GridLayout {
		anchors.fill: parent
		Text {
			text: "Hello, World!"
		}
	}
}

