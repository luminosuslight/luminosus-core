import QtQuick 2.5
import CustomStyle 1.0
import CustomElements 1.0
import "qrc:/core/ui/items"

CustomTouchArea {
    id: root
    implicitHeight: -1  // stretch height by default

    // public attributes:
    property int padding: 20*dp
    property real value: 0.0
    property real indicator: 0.0
    property bool useIndicator: false
    property bool midiMappingEnabled: true

    Rectangle {  // grey BG line
        width: 2*dp
        gradient: Gradient {
            GradientStop { position: 0.0; color: "white" }
            GradientStop { position: 1.0; color: "black" }
        }
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: padding
        anchors.bottom: parent.bottom
        anchors.bottomMargin: padding
    }

    Image {  // cursor
        id: cursor
        width: Math.round(32*dp)
        height: Math.round(32*dp)
        source: (dp <= 1) ? "qrc:/core/ui/images/slider_cursor.png" : "qrc:/core/ui/images/slider_cursor@2x.png"
        anchors.horizontalCenter: parent.horizontalCenter
        y: Math.round(padding + (parent.height - 2*padding) * (1 - value) - height / 2)
    }

    // ------------------------- Touch Logic ------------------------------

    KineticEffect {
        id: kineticEffect
        friction: 0.18
        minValue: padding
        maxValue: parent.height - padding
        onMoving: setValueByPosition(position)
    }

    onTouchDown: {
        if (midiMappingEnabled) controller.midiMapping().guiControlHasBeenTouched(mappingID)
        kineticEffect.setValue(touch.itemY)
        kineticEffect.start(touch.itemY)
        setValueByPosition(touch.itemY)
    }

    onTouchMove: {
        kineticEffect.update(Math.max(kineticEffect.minValue, Math.min(touch.itemY, kineticEffect.maxValue)))
    }

    onTouchUp: {
        kineticEffect.stop(Math.max(kineticEffect.minValue, Math.min(touch.itemY, kineticEffect.maxValue)))
    }

    function setValueByPosition(position) {
        var val = 1 - Math.max(0.0, Math.min(1.0, (position - padding) / (height - 2*padding)))
        guiManager.setPropertyWithoutChangingBindings(root, "value", val)
        // "this" as first parameter does not work, bug in QML?
    }

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

