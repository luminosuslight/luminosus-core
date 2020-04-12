import QtQuick 2.12
import CustomElements 1.0
import "qrc:/core/ui/items"
import "qrc:/core/ui/controls"

BlockBase {
    id: root
    width: 120*dp
    height: 30*dp

    StretchColumn {
        anchors.fill: parent

        DragArea {
            text: "Cycle"

            InputNode {
                node: block.node("inputNode")
            }

            OutputNode {
                node: block.node("outputNode")
            }

        }
    }  // end main Column
}

