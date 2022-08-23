import QtQuick 2.5
import Qt5Compat.GraphicalEffects
import CustomElements 1.0

Item {
	id: root
    property var block
    property alias pressed: touchArea.pressed
    property bool showShadows: true
    property Component settingsComponent: noSettings
    property alias color: background.color
    property bool ignoreKeyboardShortcuts: false

	onXChanged: block.positionChanged()
	onYChanged: block.positionChanged()

    z: block.focused ? 1 : 0
    antialiasing: false

    Component {
        id: noSettings
        Item {
            implicitHeight: 60*dp

            Text {
                anchors.centerIn: parent
                text: "No settings available\nfor " + block.getBlockName() + "."
                color: "#fff"
                horizontalAlignment: Text.AlignHCenter
                font.family: "Quicksand"
                font.pixelSize: 16*dp
            }
        }
    }

    // Help and Settings buttons:
    BlockOptionButton {
        id: helpButton
        anchors.left: parent.left
        anchors.leftMargin: root.width > 80*dp ? 20*dp : 5*dp
        z: -3
        text: "?"
        enabled: !settingsButton.active
        content: Text {
            width: 270*dp
            height: contentHeight
            font.pixelSize: 13*dp
            textFormat: Text.AutoText
            wrapMode: Text.Wrap
            color: "white"
            text: block.getHelpText()
        }
    }
    BlockOptionButton {
        id: settingsButton
        anchors.left: helpButton.right
        anchors.leftMargin: 10*dp
        z: -3
        icon: "gear_icon"
        enabled: !helpButton.active && settingsComponent !== noSettings
        content: settingsComponent
    }

    // shadow behind block:
    RectangularGlow {
        id: effect
        width: parent.width
        height: parent.height
        y: 3*dp
        z: -2
        glowRadius: pressed ? 12*dp : 4*dp
        cornerRadius: glowRadius
        spread: 0.1
        color: Qt.rgba(0, 0, 0, 0.5)
        Behavior on glowRadius {
            NumberAnimation {
                duration: 400
                easing.type: Easing.OutBounce
            }
        }
        visible: GRAPHICAL_EFFECTS_LEVEL >=2 && showShadows
    }

    // focus border:
    RectangularGlow {
        anchors.fill: parent
        z: -1
        glowRadius: 10*dp
        cornerRadius: glowRadius
        spread: 0.1
        color: Qt.rgba(0.9, 0.8, 0.0, 0.5)
        visible: block.focused && showShadows
    }

    Rectangle {
        id: background
        anchors.fill: parent
        // background color of all blocks:
        color: "#444"
    }

	function moveAnimatedTo(newX, newY) {
		xAnimator.from = x
		xAnimator.to = newX
		yAnimator.from = y
        yAnimator.to = newY
        positionAnimation.start()
	}

    ParallelAnimation {
        id: positionAnimation
        NumberAnimation {
            id: xAnimator
            target: root
            property: "x"
            duration: 500
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            id: yAnimator
            target: root
            property: "y"
            easing.type: Easing.OutCubic
            duration: xAnimator.duration
        }
        onStopped: {
            block.positionChangedExternal()
        }
    }

	// ------------------ Keyboard Handling ----------------------

	// Delete Key:
    Keys.onDeletePressed: (event) => {
        if (block.focused && !ignoreKeyboardShortcuts) {
            block.deletedByUser()
			event.accepted = true
		}
	}

    // Ctrl + D, Ctrl + C and Ctrl + X:
    Keys.onPressed: (event) => {
        if (!block.focused || ignoreKeyboardShortcuts) return;
		if (event.key === Qt.Key_D && (event.modifiers & Qt.ControlModifier)) {
			controller.blockManager().duplicateFocusedBlock()
			event.accepted = true
		} else if (event.key === Qt.Key_C && (event.modifiers & Qt.ControlModifier)) {
			controller.blockManager().copyFocusedBlock()
			event.accepted = true
        } else if (event.key === Qt.Key_X && (event.modifiers & Qt.ControlModifier)) {
            controller.blockManager().copyFocusedBlock()
            block.deletedByUser()
            event.accepted = true
        }
	}

    // ------------------ Delete Animation ----------------------

    function startDeleteAnimation() {
        deleteAnimation.start()
    }

    ScaleAnimator {
        id: deleteAnimation
        objectName: "deleteAnimation"
        target: root
        from: 1
        to: 0.01
        running: false
        duration: GRAPHICAL_EFFECTS_LEVEL < 2 ? 0 : 150
        onStopped: {
            block.onDeleteAnimationEnd()
        }
    }

    // -------------------------- Move Logic -----------------------------------

    CustomTouchArea {
        id: touchArea
        anchors.fill: parent
        secondTouchEnabled: false

        KineticEffect2D {
            id: kineticEffect
            friction: 0.1
            onMoving: {
                root.x = position.x
                root.y = position.y
            }
        }

        Connections {
            target: root.block
            function onPositionChangedExternal() {
                kineticEffect.setValue(root.x, root.y)
            }
        }

        onTouchDown: {
            positionAnimation.stop()
            kineticEffect.start(touch.x, touch.y)
        }

        onTouchMove: {
            kineticEffect.update(touch.x, touch.y)
        }

        onTouchUp: {
            kineticEffect.stop(touch.x, touch.y)

            // check if it is released in trash area:
            var isInTrash = guiManager.pointIsInTrashArea(touch.sceneX, touch.sceneY)
            if (isInTrash) {
                block.deletedByUser()
            }

            if (guiManager.snapToGrid) {
                kineticEffect.stopMovement()
                guiManager.moveToGrid(root)
            }
        }

        onClick: {
            // touch was a tap
            block.focus()
        }

        onRightClick: {
            guiManager.openBlockSettings()
        }
    }

}

