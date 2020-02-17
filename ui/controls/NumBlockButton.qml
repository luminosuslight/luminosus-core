import QtQuick 2.5
import CustomElements 1.0

import "qrc:/core/ui/items"

// ------------- Button to use in a NumBlock -----------------
CustomTouchArea {
    id: control
    property bool bold: true
    property string text
    Rectangle {
        anchors.fill: parent
        border.width: 1
        border.color: "#444"
//			gradient: Gradient {
//				GradientStop { position: 0 ; color: control.pressed ? "#444" : "#333" }
//				GradientStop { position: 1 ; color: control.pressed ? "#555" : "#444" }
//			}
        gradient: Gradient {
            GradientStop { position: 0 ; color: control.pressed ? "#444" : "#3D5970" }
            GradientStop { position: 1 ; color: control.pressed ? "#555" : "#334754" }
        }
    }
    Text {
        anchors.fill: parent
        text: control.text
        font.pixelSize: 18*dp
        font.bold: control.bold
        color: "#ccc"
        fontSizeMode: Text.Fit
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

	Rectangle {
		// Trigger Feedback Animation
		// Blinks when trigger gets activated
		anchors.fill: parent
		color: Qt.rgba(1, 1, 1, 0.3)
		opacity: 0
		NumberAnimation on opacity {
			id: triggerFeedback
			running: false
			alwaysRunToEnd: true
			from: 1
			to: 0
			duration: 300
			easing.type: Easing.OutCubic
		}
	}
    onPressedChanged: {
        if (pressed) {
            controller.playClickSound()
            triggerFeedback.restart()
        }
    }

	function simulatePress() {
        control.click(null)
		triggerFeedback.restart()
	}
}
