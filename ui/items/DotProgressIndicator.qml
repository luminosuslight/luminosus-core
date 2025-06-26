import QtQuick 2.12

Item {
    id: root
    width: 30*dp
    height: 30*dp
    property real progress: 0.0
    property color color: "lightgreen"

    Behavior on progress {
        NumberAnimation {
            duration: 300
        }
    }

    ShaderEffect {
        visible: progress > 0.0
        anchors.centerIn: parent
        width: 26*dp
        height: 26*dp
        property variant lineWidth: (2.5*dp) / width
        property variant smoothness: 1.0 / width
        property color color: root.color
        property color backgroundColor: "#333"
        property variant value: progress
        fragmentShader: "qrc:/core/ui/items/ring_shader.frag.qsb"
    }
}
