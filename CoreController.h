#ifndef CORECONTROLLER_H
#define CORECONTROLLER_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QTimer>
#include <QScopedPointer>
#include <QQuickItem>

class AnchorManager;
class BlockList;
class BlockManager;
class Engine;
class FileSystemManager;
class GuiManager;
class HandoffManager;
class KeyboardEmulator;
class LogManager;
class ProjectManager;

/**
 * @brief The CoreController class initializes all components and
 * makes them available by getter functions.
 */
class CoreController : public QObject {

    Q_OBJECT

    Q_PROPERTY(bool developerMode READ getDeveloperMode WRITE setDeveloperMode NOTIFY developerModeChanged)
    Q_PROPERTY(bool clickSounds READ getClickSounds WRITE setClickSounds NOTIFY clickSoundsChanged)
    Q_PROPERTY(QString templateFileToImport READ getTemplateFileBaseName NOTIFY templateFileToImportChanged)

public:

    /**
     * @brief CoreController creates a CoreController object and initializes
     * all Manager classes
     * @param templateFile if not empty, imports this template on startup
     * @param qmlEngine is the QML engine to use to create the GUI
     * @param parent the QObject parent
     */
    explicit CoreController(QQmlApplicationEngine* qmlEngine, QString templateFile = {}, QObject *parent = nullptr);
    void finishLoading(QUrl mainQmlFile, bool restore=true, bool saveReguarly=true);
    ~CoreController();

signals:
    void managersInitialized();
    void developerModeChanged();
    void clickSoundsChanged();
    void templateFileToImportChanged();

public:

    // ----------------------- Getter Methods for Manager classes:

    void registerManager(QString name, QObject* manager);

    template<typename T>
    T* manager(QString name) const {
        return qobject_cast<T*>(m_manager.value(name));
    }

    QNetworkAccessManager* networkAccessManager() const;

public slots:

    QObject* managerQml(QString name) const { return m_manager.value(name); }

    /**
     * @brief guiManager is a Getter for the only GuiManager instance to use in this application
     * @return a pointer to a GuiManager instance
     */
    GuiManager* guiManager() { return m_guiManager.get(); }
    /**
     * @brief logManager is a Getter for the only LogManager instance to use in this application
     * @return a pointer to a LogManager instance
     */
    LogManager* logManager() { return m_logManager.get(); }
    /**
     * @brief engine is a Getter for the only Engine instance to use in this application
     * @return a pointer to a LogManager instance
     */
    Engine* engine() { return m_engine.get(); }
    /**
     * @brief dao is a Getter for the only FileSystemManager instance to use in this application
     * @return a pointer to a FileSystemManager instance
     */
    FileSystemManager* dao() { return m_dao.get(); }
    /**
     * @brief blockManager is a Getter for the only BlockManager instance to use in this application
     * @return a pointer to a BlockManager instance
     */
    BlockManager* blockManager() { return m_blockManager.get(); }
    /**
     * @brief projectManager is a Getter for the only ProjectManager instance to use in this application
     * @return a pointer to a ProjectManager instance
     */
    ProjectManager* projectManager() { return m_projectManager.get(); }
    /**
     * @brief anchorManager is a Getter for the only AnchorManager instance to use in this application
     * @return  a pointer to an AnchorManager instance
     */
    AnchorManager* anchorManager() { return m_anchorManager.get(); }
    /**
     * @brief handoffManager is a Getter for the only HandoffManager instance to use in this application
     * @return a pointer to a HandoffManager instance
     */
    HandoffManager* handoffManager() { return m_handoffManager.get(); }
    /**
     * @brief keyboardEmulator is a Getter for the only KeyboardEmulator instance to use in this application
     * @return a pointer to a KeyboardEmulator instance
     */
    KeyboardEmulator* keyboardEmulator() { return m_keyboardEmulator.get(); }


    // -----------------------------  State / Projects:
    /**
     * @brief saveAll saves the application state and current project to the filesystem
     */
    void saveAll();
    /**
     * @brief restoreApp restores the application state and last loaded project from the filesystem,
     * to be used at application startup only
     */
    void restoreApp();


    // ----------------------------- Start / Exit:
    /**
     * @brief onExit performs actions to be done before a normal application exit
     * (stops the Engine, saves everything and deletes all blocks so that they can perform
     * "on exit" actions)
     */
    void onExit();
    /**
     * @brief onFirstStart is called only on first appliction start and loads the tutorial project
     */
    void onFirstStart();
    /**
     * @brief askForPermissions shows dialogs to ask the user permission for various feautures
     * on Android 6.0+
     */
    void askForPermissions();


    // ---------------------------- Powermate / universal Controller:
    /**
     * @brief onControllerRotated forwards the event to the currently focused block
     * @param relativeAmount amount of rotation typicaly between 1/512 and 1/64 (accelerated)
     * @param degrees amount of rotation in degrees (not accelerated)
     * @param pressed true if powermate is pressed
     */
    void onControllerRotated(double relativeAmount, double degrees, bool pressed);
    /**
     * @brief onControllerPressed forwards the event to the currently focused block
     */
    void onControllerPressed();
    /**
     * @brief onControllerReleased forwards the event to the currently focused block
     * @param duration how long the powermate was pressed
     */
    void onControllerReleased(double duration);


    // --------------------------- Getter / Setter:

    bool getClickSounds() const { return m_clickSounds; }
    void setClickSounds(bool value) { m_clickSounds = value; emit clickSoundsChanged(); }

    QString getTemplateFileBaseName() const;
    void requestTemplateImport(QString filename);
    void onImportTemplateFileAccepted();


    // --------------------------- Used in QML:

    /**
     * @brief getDeveloperMode returns the state of the developer mode
     * @return true if developer mode is active
     */
    bool getDeveloperMode() const { return m_developerMode; }

    /**
     * @brief setDeveloperMode enables or disables the developer mode
     * @param value true to enable it
     */
    void setDeveloperMode(bool value) { m_developerMode = value; emit developerModeChanged(); }

    /**
     * @brief getVersionString returns the version string to be displayed in GUI
     * @return a version string
     */
    QString getVersionString() const;
    QString getBuildDate() const;

    // TODO: move to a better place
    void playClickSound();
    void playClickUpSound();

    // dummy methods for compatibility:
    QObject* midiMapping() { return this; }
    void registerGuiControl(QQuickItem*, QString) {}
    void unregisterGuiControl(QString) {}
    void guiControlHasBeenTouched(QString) {}
    void sendFeedback(QString, double) const {}

protected:
    QNetworkAccessManager* m_networkAccessManager = nullptr;

    // Managers:
    QScopedPointer<AnchorManager> m_anchorManager;  //!< AnchorManager instance
    QScopedPointer<BlockManager> m_blockManager;  //!< BlockManager instance
    QScopedPointer<Engine> m_engine;  //!< Engine instance
    QScopedPointer<FileSystemManager> m_dao;  //!< FileSystemManager instance
    QScopedPointer<GuiManager> m_guiManager;  //!< GuiManager instance
    QScopedPointer<HandoffManager> m_handoffManager;  //!< HandoffManager instance
    QScopedPointer<KeyboardEmulator> m_keyboardEmulator;  //!< KeyboardEmulator instance
    QScopedPointer<LogManager> m_logManager;  //!< LogManager instance
    QScopedPointer<ProjectManager> m_projectManager;  //!< GlobalOscCommands instance

    // Attributes:
    /**
     * @brief m_saveTimer timer to save app state on a regular basis
     */
    QTimer m_saveTimer;
    /**
    * @brief m_developerMode true if developer mode is active
    */
    bool m_developerMode;
    /**
     * @brief m_clickSounds true if click sounds should be played
     */
    bool m_clickSounds;
    /**
     * @brief m_templateFileToImport file provided as commandline argument or via drag'n'drop that should be imported
     */
    QString m_templateFileToImport;

    QHash<QString, QObject*> m_manager;

};

#endif // CORECONTROLLER_H
