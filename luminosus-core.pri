# ------------------ General configurations -----------------------

QT += qml quick svg websockets

CONFIG += c++17

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# use QtQuick Compiler (only available under commercial license)
CONFIG += qtquickcompiler


# ------------------ Resources (.cpp, .h, .qrc, icons) -----------------------

RESOURCES += \
    $$PWD/data-core.qrc \
    $$PWD/ui-core.qrc \
    $$PWD/ui-images-core.qrc

HEADERS += \
    $$PWD/CoreController.h \
    $$PWD/block_basics/ConnectionCycleBlock.h \
    $$PWD/block_basics/GroupBlock.h \
    $$PWD/conversation/Command.h \
    $$PWD/conversation/ConversationActionInterface.h \
    $$PWD/conversation/SystemOutput.h \
    $$PWD/conversation/UserInput.h \
    $$PWD/helpers/ObjectWithAttributes.h \
    $$PWD/helpers/constants.h \
    $$PWD/helpers/qstring_literal.h \
    $$PWD/helpers/utils.h \
    $$PWD/manager/StatusManager.h \
    $$PWD/qtquick_items/BarGraphItem.h \
    $$PWD/qtquick_items/BezierCurve.h \
    $$PWD/qtquick_items/ColoredPointsItem.h \
    $$PWD/qtquick_items/CustomImagePainter.h \
    $$PWD/qtquick_items/IrregularCircleItem.h \
    $$PWD/qtquick_items/IrregularCircleOutline.h \
    $$PWD/qtquick_items/IrregularCircleOutlineTwoColored.h \
    $$PWD/qtquick_items/KineticEffect.h \
    $$PWD/qtquick_items/KineticEffect2D.h \
    $$PWD/qtquick_items/LineItem.h \
    $$PWD/qtquick_items/NodeConnectionLines.h \
    $$PWD/qtquick_items/PointsItem.h \
    $$PWD/qtquick_items/ShadowedRectangle.h \
    $$PWD/qtquick_items/ShadowedTexture.h \
    $$PWD/qtquick_items/SpectrumItem.h \
    $$PWD/qtquick_items/StretchLayouts.h \
    $$PWD/qtquick_items/TouchArea.h \
    $$PWD/block_basics/BlockBase.h \
    $$PWD/block_basics/BlockInterface.h \
    $$PWD/block_basics/InOutBlock.h \
    $$PWD/block_basics/OneInputBlock.h \
    $$PWD/block_basics/OneOutputBlock.h \
    $$PWD/connections/Matrix.h \
    $$PWD/connections/NodeData.h \
    $$PWD/connections/Nodes.h \
    $$PWD/helpers/AsyncWebSocket.h \
    $$PWD/helpers/QCircularBuffer.h \
    $$PWD/helpers/SmartAttribute.h \
    $$PWD/helpers/application_setup.h \
    $$PWD/manager/AnchorManager.h \
    $$PWD/manager/BlockList.h \
    $$PWD/manager/BlockManager.h \
    $$PWD/manager/Engine.h \
    $$PWD/manager/FileSystemManager.h \
    $$PWD/manager/GuiManager.h \
    $$PWD/manager/HandoffManager.h \
    $$PWD/manager/KeyboardEmulator.h \
    $$PWD/manager/LogManager.h \
    $$PWD/manager/ProjectManager.h \
    $$PWD/manager/WebsocketConnection.h \
    $$PWD/qtquick_items/scenegraph/paintedrectangleitem.h \
    $$PWD/qtquick_items/scenegraph/shadowedborderrectanglematerial.h \
    $$PWD/qtquick_items/scenegraph/shadowedbordertexturematerial.h \
    $$PWD/qtquick_items/scenegraph/shadowedrectanglematerial.h \
    $$PWD/qtquick_items/scenegraph/shadowedrectanglenode.h \
    $$PWD/qtquick_items/scenegraph/shadowedtexturematerial.h \
    $$PWD/qtquick_items/scenegraph/shadowedtexturenode.h \
    $$PWD/version.h

SOURCES += \
    $$PWD/CoreController.cpp \
    $$PWD/block_basics/ConnectionCycleBlock.cpp \
    $$PWD/block_basics/GroupBlock.cpp \
    $$PWD/conversation/Command.cpp \
    $$PWD/conversation/SystemOutput.cpp \
    $$PWD/conversation/UserInput.cpp \
    $$PWD/helpers/ObjectWithAttributes.cpp \
    $$PWD/helpers/qstring_literal.cpp \
    $$PWD/manager/StatusManager.cpp \
    $$PWD/qtquick_items/BarGraphItem.cpp \
    $$PWD/qtquick_items/BezierCurve.cpp \
    $$PWD/qtquick_items/ColoredPointsItem.cpp \
    $$PWD/qtquick_items/CustomImagePainter.cpp \
    $$PWD/qtquick_items/IrregularCircleItem.cpp \
    $$PWD/qtquick_items/IrregularCircleOutline.cpp \
    $$PWD/qtquick_items/IrregularCircleOutlineTwoColored.cpp \
    $$PWD/qtquick_items/KineticEffect.cpp \
    $$PWD/qtquick_items/KineticEffect2D.cpp \
    $$PWD/qtquick_items/LineItem.cpp \
    $$PWD/qtquick_items/NodeConnectionLines.cpp \
    $$PWD/qtquick_items/PointsItem.cpp \
    $$PWD/qtquick_items/ShadowedRectangle.cpp \
    $$PWD/qtquick_items/ShadowedTexture.cpp \
    $$PWD/qtquick_items/SpectrumItem.cpp \
    $$PWD/qtquick_items/StretchLayouts.cpp \
    $$PWD/qtquick_items/TouchArea.cpp \
    $$PWD/block_basics/BlockBase.cpp \
    $$PWD/block_basics/InOutBlock.cpp \
    $$PWD/block_basics/OneInputBlock.cpp \
    $$PWD/block_basics/OneOutputBlock.cpp \
    $$PWD/connections/Matrix.cpp \
    $$PWD/connections/NodeData.cpp \
    $$PWD/connections/Nodes.cpp \
    $$PWD/helpers/AsyncWebSocket.cpp \
    $$PWD/helpers/SmartAttribute.cpp \
    $$PWD/helpers/application_setup.cpp \
    $$PWD/manager/AnchorManager.cpp \
    $$PWD/manager/BlockList.cpp \
    $$PWD/manager/BlockManager.cpp \
    $$PWD/manager/Engine.cpp \
    $$PWD/manager/FileSystemManager.cpp \
    $$PWD/manager/GuiManager.cpp \
    $$PWD/manager/HandoffManager.cpp \
    $$PWD/manager/KeyboardEmulator.cpp \
    $$PWD/manager/LogManager.cpp \
    $$PWD/manager/ProjectManager.cpp \
    $$PWD/manager/WebsocketConnection.cpp \
    $$PWD/qtquick_items/scenegraph/paintedrectangleitem.cpp \
    $$PWD/qtquick_items/scenegraph/shadowedborderrectanglematerial.cpp \
    $$PWD/qtquick_items/scenegraph/shadowedbordertexturematerial.cpp \
    $$PWD/qtquick_items/scenegraph/shadowedrectanglematerial.cpp \
    $$PWD/qtquick_items/scenegraph/shadowedrectanglenode.cpp \
    $$PWD/qtquick_items/scenegraph/shadowedtexturematerial.cpp \
    $$PWD/qtquick_items/scenegraph/shadowedtexturenode.cpp

DISTFILES += \
    $$PWD/CREDITS.txt \
    $$PWD/LICENSE.txt \
    $$PWD/README.md


# ------------------ OS specific -----------------------

unix:!macx:!android:!ios {
    CONFIG += linux_specific
}

ios {
    CONFIG += mobile_platform
}

android {
    CONFIG += mobile_platform
    QT += core-private  # to ask for permissions
}

linux_specific {

}

mobile_platform {

}

# enable threads on all platforms:
QT += concurrent
DEFINES += THREADS_ENABLED

!emscripten {
    # enable SSL on all platforms but WebAssembly:
    DEFINES += SSL_ENABLED
}

# --------------- Other -------------------

# CONFIG += multimedia_available
multimedia_available {
    QT += multimedia
    DEFINES += MULTIMEDIA_AVAILABLE
}
