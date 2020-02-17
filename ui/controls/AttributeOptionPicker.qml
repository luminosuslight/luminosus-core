import QtQuick 2.12
import QtQuick.Window 2.12
import CustomStyle 1.0
import CustomElements 1.0
import "qrc:/core/ui/items"


CustomTouchArea {
    id: root
    implicitWidth: -1
    mouseOverEnabled: true
    property QtObject attr
    property var optionListGetter: function () { return ["A", "B", "C"] }
    property alias align: displayedText.horizontalAlignment
    property bool openToLeft: false

    property Item dropDownItem

    Rectangle {
        anchors.fill: parent
        color: "white"
        opacity: 0.1
        visible: parent.mouseOver
    }

    Text {  // text
        id: displayedText
        anchors.fill: parent
        anchors.rightMargin: parent.width > 45*dp ? 5*dp : 0
        anchors.leftMargin: parent.width > 45*dp ? 5*dp : 0
        font.pixelSize: 16*dp
        font.bold: true
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: root.enabled ? "lightgrey" : "#555"
        wrapMode: Text.Wrap
        elide: Text.ElideRight
        fontSizeMode: Text.Fit
        text: attr.val
    }

    Rectangle {  // thin blue line under text
        color: dropDownItem ? Style.primaryHighlightColor : (root.enabled ? Style.primaryActionColor : "#555")
        height: 1*dp
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 6*dp
        anchors.rightMargin: 6*dp
        anchors.verticalCenter: root.verticalCenter
        anchors.verticalCenterOffset: 12*dp
    }

    // -------------- Touch Handling --------------------

    onClick: {
        if (dropDownItem) {
            controller.playClickUpSound()
            dropDownItem.destroy()
            return
        }
        if (!enabled) return

        controller.playClickSound()
        var window = Window.contentItem
        var openPosition = openToLeft ? mapToItem(window, -205*dp, height / 2) : mapToItem(window, width + 5*dp, height / 2)
        dropDownItem = dropDownComponent.createObject(window, {x: openPosition.x, rootY: openPosition.y})
        dropDownItem.forceActiveFocus()
    }

    // ---------------------- DropDown Component ----------------

    Component {
        id: dropDownComponent

        Rectangle {
            id: dropDown
            property int rootY
            y: rootY - height / 2
            width: 200*dp
            height: Math.min(optionListColumn.contentHeight, 200*dp)
            color: "#333"
            radius: 8*dp
            antialiasing: dp < 2

            onActiveFocusChanged: {
                if (!activeFocus) {
                    dropDown.destroy()
                }
            }

            Keys.onEscapePressed: dropDown.destroy()

            Rectangle {
                rotation: 45
                color: parent.color
                width: 15*dp
                height: width
                anchors.horizontalCenter: root.openToLeft ? parent.right : parent.left
                anchors.verticalCenter: parent.verticalCenter
            }

            CustomTouchArea {
                width: 3000*dp
                height: 3000*dp
                anchors.centerIn: parent

                onScrollEvent: scrollEventWasAccepted()
                onTouchDown: {
                    dropDown.destroy()
                    if (!root.contains(root.mapFromItem(this, touch.itemX, touch.itemY))) {
                        touch.accepted = false
                    }
                }
            }

            ListView {
                id: optionListColumn
                anchors.fill: parent
                model: root.optionListGetter()
                clip: true

                delegate: CustomTouchArea {
                    width: optionListColumn.width
                    height: Math.max(30*dp, optionLabel.contentHeight)
                    mouseOverEnabled: true
                    onClick: {
                        controller.playClickSound()
                        attr.val = modelData
                        dropDown.destroy()
                    }
                    Rectangle {
                        anchors.fill: parent
                        color: "white"
                        opacity: 0.1
                        visible: parent.mouseOver
                        radius: 8*dp
                        antialiasing: false
                    }

                    Text {
                        id: optionLabel
                        anchors.fill: parent
                        font.pixelSize: 16*dp
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: displayedText.horizontalAlignment
                        color: modelData === attr.val ? Style.primaryHighlightColor :  "#fff"
                        wrapMode: Text.Wrap
                        text: modelData
                    }
                }
            }  // end option list
        }
    }
}
