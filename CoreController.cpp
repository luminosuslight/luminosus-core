#include "core/CoreController.h"

#include "core/manager/AnchorManager.h"
#include "core/manager/BlockList.h"
#include "core/manager/BlockManager.h"
#include "core/manager/Engine.h"
#include "core/manager/FileSystemManager.h"
#include "core/manager/GuiManager.h"
#include "core/manager/HandoffManager.h"
#include "core/manager/KeyboardEmulator.h"
#include "core/manager/LogManager.h"
#include "core/manager/ProjectManager.h"
#include "core/manager/WebsocketConnection.h"

#include "core/helpers/constants.h"
#include "core/helpers/utils.h"
#include "core/helpers/qstring_literal.h"
#include "core/version.h"

#include <QCborMap>
#include <QFileInfo>

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QtCore/private/qjnihelpers_p.h>
#endif

CoreController::CoreController(QQmlApplicationEngine* qmlEngine, QString templateFile, QObject* parent)
    : QObject(parent)
    , m_networkAccessManager(new QNetworkAccessManager())
    , m_anchorManager(new AnchorManager(this))
    , m_blockManager(new BlockManager(this))
    , m_engine(new Engine(this))
    , m_dao(new FileSystemManager())
    , m_guiManager(new GuiManager(this, qmlEngine))
    , m_handoffManager(new HandoffManager(this))
    , m_keyboardEmulator(new KeyboardEmulator(this))
    , m_logManager(new LogManager(this))
    , m_projectManager(new ProjectManager(this))
    , m_websocketConnection(new WebsocketConnection(this))
    , m_developerMode(false)
    , m_clickSounds(false)
    , m_templateFileToImport(templateFile)
{
    // print Qt Version to verify that the right library is loaded:
    qInfo() << "Compiled with Qt Version" << QT_VERSION_STR;
    qInfo() << "Runtime Qt Version:" << qVersion();

    // Register classes which slots should be accessible from QML:
    qmlRegisterAnonymousType<AnchorManager>("Luminosus", 1);
    qmlRegisterAnonymousType<BlockManager>("Luminosus", 1);
    qmlRegisterAnonymousType<Engine>("Luminosus", 1);
    qmlRegisterAnonymousType<FileSystemManager>("Luminosus", 1);
    qmlRegisterAnonymousType<GuiManager>("Luminosus", 1);
    qmlRegisterAnonymousType<HandoffManager>("Luminosus", 1);
    qmlRegisterAnonymousType<KeyboardEmulator>("Luminosus", 1);
    qmlRegisterAnonymousType<LogManager>("Luminosus", 1);
    qmlRegisterAnonymousType<ProjectManager>("Luminosus", 1);
    qmlRegisterAnonymousType<WebsocketConnection>("Luminosus", 1);

    // Tell QML that these objects are owned by C++ and should not be deleted by the JS GC:
    // This is very important because otherwise SEGFAULTS will appear randomly!
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(m_anchorManager.get(), QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(m_blockManager.get(), QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(m_engine.get(), QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(m_dao.get(), QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(m_guiManager.get(), QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(m_handoffManager.get(), QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(m_keyboardEmulator.get(), QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(m_logManager.get(), QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(m_projectManager.get(), QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(m_websocketConnection.get(), QQmlEngine::CppOwnership);
}

void CoreController::finishLoading(QUrl mainQmlFile) {
    m_guiManager->createAndShowWindow(mainQmlFile);
    // restore app settings and last project:
    restoreApp();

    // start App engine (for actions that should be run each frame):
    m_engine->start();

    // start save timer (save each 5s):
    connect(&m_saveTimer, SIGNAL(timeout()), this, SLOT(saveAll()));
    if (qgetenv("ALARM_CLOCK_MODE") == "1") {
        qDebug() << "Alarm clock mode: saving only each 2 minutes.";
        m_saveTimer.start(2*60*1000);
    } else {
        m_saveTimer.start(5000);
    }

#ifdef Q_OS_ANDROID
    QTimer::singleShot(4000, this, SLOT(askForPermissions()));
#endif

    qInfo() << "-------------------------------------------";
}

void CoreController::registerManager(QString name, QObject* manager) {
    m_manager[name] = manager;
    m_guiManager->setQmlContextProperty(name, QVariant::fromValue(manager));
}

QNetworkAccessManager* CoreController::networkAccessManager() const {
    return m_networkAccessManager;
}

CoreController::~CoreController() = default;

void CoreController::saveAll() {
    QCborMap appState;
    appState["version"_q] = 0.3;
    m_guiManager->writeTo(appState);
    appState["currentProject"_q] = m_projectManager->getCurrentProjectName();
    appState["developerMode"_q] = getDeveloperMode();
    appState["clickSounds"_q] = getClickSounds();
    appState["websocketConnection"_q] = m_websocketConnection->getState();

    for (auto managerName: m_manager.keys()) {
        const auto* objectWithAttributes = dynamic_cast<const ObjectWithAttributes*>(m_manager[managerName]);
        if (objectWithAttributes) {
            QCborMap managerState;
            objectWithAttributes->writeAttributesTo(managerState);
            appState[managerName] = managerState;
        }
    }

    m_dao->saveFile("", "autosave.ats", appState);

    m_projectManager->saveCurrentProject();
}

void CoreController::restoreApp() {
    bool lockExisted = m_dao->fileExists("", "luminosus.lock");
    m_dao->saveFile("", "luminosus.lock", QByteArray());

    // TODO: move this to other controller
    QStringList combinations{"PowerPoint Fixture.lbc"};
    for (QString filename: combinations) {
        if (!m_dao->fileExists("combinations", filename)) {
            m_dao->importFile(":/core/data/" + filename, "combinations", true);
        }
    }

    QCborMap appState = m_dao->loadCborMap("", "autosave.ats");
    if (appState.empty()) {
        // app state doesn't exist
        emit managersInitialized();
        onFirstStart();
        return;
    }
    m_guiManager->readFrom(appState);
    setDeveloperMode(appState["developerMode"_q].toBool());
    setClickSounds(appState["clickSounds"_q].toBool());
    m_websocketConnection->setState(appState["websocketConnection"_q].toMap());

    for (auto managerName: m_manager.keys()) {
        auto* objectWithAttributes = dynamic_cast<ObjectWithAttributes*>(m_manager[managerName]);
        if (objectWithAttributes) {
            QCborMap managerState = appState[managerName].toMap();
            objectWithAttributes->readAttributesFrom(managerState);
        }
    }
    emit managersInitialized();

#ifndef Q_OS_ANDROID
    if (lockExisted) {
#else
    if (false) {
#endif
        // -> another instance is running or previously running instance
        // didn't exit successfully (crashed)
        // create 'No Project' and load it (because crashes there are unlikely):
        qWarning() << "Another instance is running or the program crashed on last run!";
        m_projectManager->importProjectFile(":/core/data/No Project (Crashed).lpr", /*load=*/ true);
        return;
    } else if (appState["version"_q].toDouble() < 0.2) {
        m_projectManager->setCurrentProject("empty", /*createIfNotExist*/ true);
    } else if (appState["version"_q].toDouble() < 0.3) {
        onFirstStart();
    } else {
        m_projectManager->setCurrentProject(appState["currentProject"_q].toString());
    }
}

void CoreController::onExit() {
    qInfo() << "-------------------------------------------";
    m_engine->stop();
    // FIXME: move this to PowerMate
    // kill powermate thread (probably in blocking read() function):
    // if (m_powermate.isRunning()) m_powermate.terminate();
    saveAll();
    m_blockManager->deleteAllBlocks();
    m_dao->deleteFile("", "luminosus.lock");
    m_guiManager->qmlEngine()->deleteLater();
    qInfo() << "Luminosus exited normally.";
}

void CoreController::onFirstStart() {
    m_projectManager->importProjectFile(":/core/data/No Project.lpr");
    if (m_projectManager->getCurrentProjectName().isEmpty()) {
        m_projectManager->setCurrentProject(ProjectManagerConstants::defaultProjectName);
    }
}

void CoreController::askForPermissions() {
#ifdef Q_OS_ANDROID
    QAndroidJniEnvironment qjniEnv;

    QStringList permissions;
    permissions << "android.permission.ACCESS_NETWORK_STATE";
    permissions << "android.permission.CAMERA";
    permissions << "android.permission.INTERNET";
    permissions << "android.permission.READ_EXTERNAL_STORAGE";
    permissions << "android.permission.WRITE_EXTERNAL_STORAGE";
    permissions << "android.permission.RECORD_AUDIO";
#ifdef BLUETOOTH_LE_SUPPORTED
    // for Bluetooth Low Energy:
    permissions << "android.permission.ACCESS_COARSE_LOCATION";
#endif

    QtAndroidPrivate::requestPermissions(qjniEnv, permissions, [](const QtAndroidPrivate::PermissionsHash & result){
        qInfo() << "--- Android permissions granted:";
        for (QString key: result.keys()) {
            qInfo() << key << ": " << (result[key] == QtAndroidPrivate::PermissionsResult::Granted);
        }
    });
#endif
}

void CoreController::onControllerRotated(double relativeAmount, double degrees, bool pressed) {
    if (m_blockManager->getFocusedBlock()) {
        m_blockManager->getFocusedBlock()->onControllerRotated(relativeAmount, degrees, pressed);
    }
}

void CoreController::onControllerPressed() {
    if (m_blockManager->getFocusedBlock()) {
        m_blockManager->getFocusedBlock()->onControllerPressed();
    }
}

void CoreController::onControllerReleased(double duration) {
    if (m_blockManager->getFocusedBlock()) {
        m_blockManager->getFocusedBlock()->onControllerReleased(duration);
    }
}

QString CoreController::getTemplateFileBaseName() const {
    return QFileInfo(m_templateFileToImport).fileName();
}

void CoreController::requestTemplateImport(QString filename) {
    m_templateFileToImport = filename;
    emit templateFileToImportChanged();
}

void CoreController::onImportTemplateFileAccepted() {
    if (m_templateFileToImport.isEmpty()) return;
    m_projectManager->importProjectFile(m_templateFileToImport, true);
}

QString CoreController::getVersionString() const {
    return LuminosusVersionInfo::VERSION_STRING;
}

QString CoreController::getBuildDate() const { return __DATE__; }

void CoreController::playClickSound() { m_blockManager->playClickSound(); }

void CoreController::playClickUpSound() { m_blockManager->playClickUpSound(); }
