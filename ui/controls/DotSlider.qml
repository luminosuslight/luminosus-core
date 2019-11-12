import QtQuick 2.5
import QtGraphicalEffects 1.12  // for canonical gradient
import CustomStyle 1.0
import CustomElements 1.0
import "qrc:/core/ui/items"

CustomTouchArea {
    id: root

    // public attributes:
    property real value: 0.0

    // --------------------------- Dot ------------------------

    ShaderEffect {
        anchors.fill: parent
        anchors.margins: 1*dp
        property variant lineWidth: (2.5*dp) / width
        property variant smoothness: 1.0 / width
        property variant color: Style.primaryActionColor
        property variant backgroundColor: "#333"
        property variant value: root.value
        fragmentShader: "qrc:/core/ui/items/circle_shader.frag"
    }

    Text {
        anchors.centerIn: parent
        text: value < 1 ? (value * 100).toFixed(0) : "FL"
        font.pixelSize: 14*dp
        font.bold: true
        visible: !root.pressed
    }

    // ----------------------- Touch Logic ----------------------

    property real sliderStartVal: 0.0
    property real defaultSliderHeight: 200*dp
    property real sliderHeight: defaultSliderHeight
    property Item overlayItem

    onTouchDown: {
        // don't change anything if not enabled:
        if (!root.enabled) return

        controller.midiMapping().guiControlHasBeenTouched(mappingID)

        sliderStartVal = root.value
        sliderHeight = defaultSliderHeight

        // preferred overlay position is in the center left of the DotSlider item:
        overlay.show(root.width / 2, touch.itemY);
    }

    onTouchMove: {
        var dy = touch.originY - touch.y
        var newValue = Math.max(0.0, Math.min(dy / sliderHeight + sliderStartVal, 1.0))
        guiManager.setPropertyWithoutChangingBindings(root, "value", newValue)

        var dx = touch.x - touch.originX - 60*dp
        if (dx > 0) {
            sliderHeight = 200*dp + dx
        }
    }

    onTouchUp: {
        overlay.destroyItem()
    }

    onTouchCanceled: {
        overlay.destroyItem()
    }

    onClick: {
        if (root.value < 0.9) {
            guiManager.setPropertyWithoutChangingBindings(root, "value", 1)
        } else {
            guiManager.setPropertyWithoutChangingBindings(root, "value", 0)
        }
    }

    // ------------------------- Slider Overlay -----------------------

    Overlay {
        id: overlay
        sourceComponent: Component {
            Item {

                Rectangle {
                    id: sliderBg
                    width: 30*dp
                    height: sliderHeight
                    x: width / 2 * -1.0
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: sliderHeight * sliderStartVal * -1.0
                    color: "#333"
                    radius: 6*dp

                    Rectangle {
                        width: 30*dp
                        height: sliderHeight * root.value
                        anchors.bottom: parent.bottom
                        color: Style.primaryActionColor
                        radius: 6*dp
                    }
                }

                Rectangle {
                    width: 60*dp
                    height: 30*dp
                    x: -100*dp
                    anchors.verticalCenter: sliderBg.bottom
                    anchors.verticalCenterOffset: sliderHeight * root.value * -1.0
                    radius: 4*dp
                    color: "#333"

                    Text {
                        anchors.centerIn: parent
                        text: (value * 100).toFixed(0) + "%"
                        font.pixelSize: 18*dp
                        font.bold: true
                    }
                }
            }  // end Item
        }  // end Overlay Component

    }// end Overlay

    // ---------------------- External Input Handling ----------------------

    property string mappingID: ""
    property real externalInput: -1
    Component.onCompleted: controller.midiMapping().registerGuiControl(this, mappingID)
    Component.onDestruction: if (controller) controller.midiMapping().unregisterGuiControl(mappingID)
    onExternalInputChanged: {
        guiManager.setPropertyWithoutChangingBindings(this, "value", externalInput)
    }
    onValueChanged: controller.midiMapping().sendFeedback(mappingID, value)
}
