import QtQuick 2.5
import CustomStyle 1.0
import CustomElements 1.0
import "qrc:/core/ui/items"

CustomTouchArea {
    id: root
    mouseOverEnabled: true
	property bool active: false
    property bool light: false

	Rectangle {
		width: 20*dp
		height: 20*dp
		anchors.centerIn: parent
		color: "transparent"
		border.width: 1*dp
        border.color: light ? "#888" : "#555"

		Rectangle {
            color: Style.primaryActionColor
			anchors.fill: parent
            anchors.margins: 3*dp
            scale: active ? (root.mouseOver ? 0.8 : 1) : 0
            Behavior on scale {
                ScaleAnimator {
                    duration: 150
				}
			}
		}
	}

    onClick: {
        controller.playClickSound()
        controller.midiMapping().guiControlHasBeenTouched(mappingID)
        guiManager.setPropertyWithoutChangingBindings(this, "active", !active)
	}

    // ---------------------- External Input Handling ----------------------

    property string mappingID: ""
    property real externalInput: -1
    Component.onCompleted: controller.midiMapping().registerGuiControl(this, mappingID)
    Component.onDestruction: if (controller) controller.midiMapping().unregisterGuiControl(mappingID)
    onExternalInputChanged: {
        if (externalInput > 0.) {
            guiManager.setPropertyWithoutChangingBindings(this, "active", true)
        } else {
            guiManager.setPropertyWithoutChangingBindings(this, "active", false)
        }
    }
    onActiveChanged: controller.midiMapping().sendFeedback(mappingID, active ? 1.0 : 0.0)
}
