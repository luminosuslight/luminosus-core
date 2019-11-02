import QtQuick 2.12
import CustomStyle 1.0
import CustomElements 1.0


TextInput {
	id: root
    property string hintText: ""
    property alias lineUnderTextColor: lineUnderText.color
    property alias hintTextColor: hintTextItem.color
	height: 30*dp
    color: "#fff"
    font.family: "Quicksand"
    font.pixelSize: 18*dp
	verticalAlignment: Text.AlignVCenter
    selectByMouse: true

	Rectangle {  // thin blue line under text
        id: lineUnderText
        color: Style.primaryActionColor
        height: 1*dp
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.leftMargin: 2*dp
		anchors.rightMargin: 2*dp
		anchors.verticalCenter: root.verticalCenter
		anchors.verticalCenterOffset: 12*dp
        visible: !root.readOnly
    }

    Text {
        id: hintTextItem
        height: parent.height
        width: parent.width
        text: hintText
        visible: parent.displayText === ""
        color: "#aaa"
        font.pixelSize: 18*dp
        font.family: root.font.family
		verticalAlignment:  Text.AlignVCenter
        horizontalAlignment: root.horizontalAlignment
    }
}

