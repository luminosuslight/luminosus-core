import QtQuick 2.5
import "qrc:/core/ui/items"
import "../."

Button {

    property bool isPlaying: false
    property string iconName: block.isPlaying ? "pause_icon": "play_icon"

    // ------------------------ Icon Image -------------------------

    Image {
        id: icon
        source: "qrc:/core/ui/images/" + iconName + ".png"
        width: Math.min(parent.height, parent.width) - 16*dp
        height: width
        anchors.centerIn: parent
        smooth: true
    }

    // ------------------- Touch Feedback Effect ------------------------------

//    RippleEffect { -> disabled due to CPU consumption bug
//        id: rippleEffect
//        anchors.fill: parent
//    }
//    onTouchDown: rippleEffect.touchStart(touch.itemX, touch.itemY)
//    onTouchUp: rippleEffect.touchEnd()
}
