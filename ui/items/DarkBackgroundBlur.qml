import QtQuick 2.5
import Qt5Compat.GraphicalEffects

Item {
	id: root
	property Item blurSource: parent
	property rect blurArea: Qt.rect(x, y, width, height)
    property real brightness: -0.4
	property real contrast: -0.3
	property color alternativeColor: Qt.rgba(0.15, 0.15, 0.15, 1)

    Loader {
        id: blur
        anchors.fill: parent
        active: GRAPHICAL_EFFECTS_LEVEL >= 3
        sourceComponent: FastBlur {
            radius: 32*dp

            source: ShaderEffectSource {
                sourceItem: blurSource
                sourceRect: blurArea
            }
            visible: false
        }
    }


    Loader {
        anchors.fill: parent
        active: GRAPHICAL_EFFECTS_LEVEL >= 3
        sourceComponent: BrightnessContrast {
            source: blur.item
            brightness: root.brightness
            contrast: root.contrast

        }
    }

	Rectangle {
		// blur alternative on low graphics
		anchors.fill: parent
		color: alternativeColor
		visible: GRAPHICAL_EFFECTS_LEVEL < 3
	}
}
