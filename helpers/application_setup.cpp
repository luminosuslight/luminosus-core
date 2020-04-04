#include "application_setup.h"

#include "core/qtquick_items/BezierCurve.h"
#include "core/qtquick_items/KineticEffect.h"
#include "core/qtquick_items/KineticEffect2D.h"
#include "core/qtquick_items/StretchLayouts.h"
#include "core/qtquick_items/TouchArea.h"
#include "core/qtquick_items/NodeConnectionLines.h"
#include "core/qtquick_items/SpectrumItem.h"
#include "core/qtquick_items/BarGraphItem.h"
#include "core/qtquick_items/LineItem.h"
#include "core/qtquick_items/CustomImagePainter.h"
#include "core/qtquick_items/PointsItem.h"
#include "core/qtquick_items/ColoredPointsItem.h"
#include "core/qtquick_items/IrregularCircleItem.h"
#include "core/qtquick_items/IrregularCircleOutline.h"
#include "core/qtquick_items/IrregularCircleOutlineTwoColored.h"
#include "core/qtquick_items/ShadowedRectangle.h"
#include "core/qtquick_items/ShadowedTexture.h"

#include "core/CoreController.h"
#include "core/manager/FileSystemManager.h"
#include "core/helpers/constants.h"

#include <QSysInfo>
#include <QFontDatabase>
#include <QCommandLineParser>

namespace Luminosus {

void preQApplicationCreation() {
    // prepare Qt application:
    // (using QApplication instead of smaller QGuiApplication to support QWidget based
    // FileDialogs on Linux)
#ifdef Q_OS_LINUX
    setenv("QSG_RENDER_LOOP", "windows", /*overwrite=*/ 1);
#endif
//#ifdef Q_OS_IOS
//    // disable QML cache on iOS for now because it doesn't work with Qt 5.10 and iOS 11.2 beta
//    // (shader files are not found and black rectangles are visible)
//    qputenv("QML_DISABLE_DISK_CACHE", "1");
//#endif
    //setenv("QSG_INFO", "1", /*overwrite=*/ 1);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, false);
#if !defined Q_OS_ANDROID && !defined Q_OS_IOS
    QApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
#endif
}

void registerQtQuickItems() {
    const char* uri = "CustomElements";

    qmlRegisterSingletonType(QUrl("qrc:/core/ui/DefaultStyle.qml"), "CustomStyle", 1, 0, "Style");
    qmlRegisterType<BezierCurve>("CustomGeometry", 1, 0, "BezierCurve");
    qmlRegisterType<KineticEffect>("CustomElements", 1, 0, "KineticEffect");
    qmlRegisterType<KineticEffect2D>("CustomElements", 1, 0, "KineticEffect2D");
    qmlRegisterType<StretchColumn>("CustomElements", 1, 0, "StretchColumn");
    qmlRegisterType<StretchRow>("CustomElements", 1, 0, "StretchRow");
    qmlRegisterType<NodeConnectionLines>("CustomElements", 1, 0, "NodeConnectionLines");
    qmlRegisterType<SpectrumItem>("CustomElements", 1, 0, "SpectrumItem");
    qmlRegisterType<BarGraphItem>("CustomElements", 1, 0, "BarGraphItem");
    qmlRegisterType<LineItem>("CustomElements", 1, 0, "LineItem");
    qmlRegisterType<CustomImagePainter>("CustomElements", 1, 0, "ImagePainter");
    qmlRegisterType<PointsItem>("CustomElements", 1, 0, "Points");
    qmlRegisterType<ColoredPointsItem>("CustomElements", 1, 0, "ColoredPoints");
    qmlRegisterType<IrregularCircleItem>("CustomElements", 1, 0, "IrregularCircle");
    qmlRegisterType<IrregularCircleOutline>("CustomElements", 1, 0, "IrregularCircleOutline");
    qmlRegisterType<IrregularCircleOutlineTwoColored>("CustomElements", 1, 0, "IrregularCircleOutlineTwoColored");
    qmlRegisterType<ShadowedRectangle>("CustomElements", 1, 0, "ShadowedRectangle");
    qmlRegisterType<ShadowedTexture>("CustomElements", 1, 0, "ShadowedTexture");

    qmlRegisterUncreatableType<BorderGroup>(uri, 2, 12, "BorderGroup", QStringLiteral("Used as grouped property"));
    qmlRegisterUncreatableType<ShadowGroup>(uri, 2, 12, "ShadowGroup", QStringLiteral("Used as grouped property"));
    qmlRegisterUncreatableType<CornersGroup>(uri, 2, 12, "CornersGroup", QStringLiteral("Used as grouped property"));

    qRegisterMetaType<TouchAreaEvent>();
    qmlRegisterAnonymousType<TouchAreaEvent>("Luminosus", 1);
    qmlRegisterType<TouchArea>("CustomElements", 1, 0, "CustomTouchArea");
}

void prepareQmlEngine(QQmlApplicationEngine& engine) {
    engine.addImportPath("qrc:/core/ui/");
    setupDpProperty(engine);
    engine.rootContext()->setContextProperty("GRAPHICAL_EFFECTS_LEVEL", LuminosusConstants::GRAPHICAL_EFFECTS_LEVEL);
}

// checks if additional scaling is needed (i.e. for HiDPI displays)
// and sets the dp QML context property accordingly:
void setupDpProperty(QQmlApplicationEngine& engine) {
    double scaleFactor = 1.0;

    // get scale factor depending on platform:
    if (QSysInfo::productType() == "android" || QSysInfo::productType() == "ios") {
        double normDPI = 160;
        scaleFactor = (int(QGuiApplication::primaryScreen()->physicalDotsPerInch()) / QGuiApplication::primaryScreen()->devicePixelRatio()) / normDPI;
    } else if (QSysInfo::productType() == "osx") {
        double normDPI = 72;
        scaleFactor = (int(QGuiApplication::primaryScreen()->logicalDotsPerInch()) / QGuiApplication::primaryScreen()->devicePixelRatio()) / normDPI;
    } else {
        double normDPI = 96;
        scaleFactor = (int(QGuiApplication::primaryScreen()->logicalDotsPerInch()) / QGuiApplication::primaryScreen()->devicePixelRatio()) / normDPI;
    }

    // don't shrink content:
    if (scaleFactor < 1) {
        scaleFactor = 1;
    }
    // round scale factor to one decimal position:
    scaleFactor = int(scaleFactor * 10 + 0.5) / 10.0;
    // set QML context property "dp" that can be accessed anywhere from QML:
    engine.rootContext()->setContextProperty("dp", scaleFactor);

    qInfo() << "Device Pixel Ratio (provided by Qt): " << QGuiApplication::primaryScreen()->devicePixelRatio();
    qInfo() << "Custom GUI scale factor (dp unit): " << scaleFactor;
}

void preparePauseAndShutdown(QApplication& app, QQmlApplicationEngine& engine, CoreController& controller) {
    QObject::connect(&app, SIGNAL(aboutToQuit()), &controller, SLOT(onExit()));
    QObject::connect(&engine, SIGNAL(quit()), &app, SLOT(quit())); // to make Qt.quit() to work

    QObject::connect(&app, &QApplication::applicationStateChanged, [&controller](Qt::ApplicationState state){
        if (state == Qt::ApplicationState::ApplicationSuspended) {
            controller.saveAll();
            controller.dao()->deleteFile("", "luminosus.lock");
        }
    });
}

}
