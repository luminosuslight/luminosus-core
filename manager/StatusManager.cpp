#include "StatusManager.h"

#include "core/CoreController.h"

Status::Status(StatusManager* statusManager, const QString& uid)
    : QObject(nullptr)
    , ObjectWithAttributes(this)
    , m_manager(statusManager)
    , m_uid(this, "uid", uid)
    , m_color(this, "color", {0.2, 0.2, 1})
    , m_title(this, "title")
    , m_text(this, "text")
    , m_progress(this, "progress", 0.0)
    , m_running(this, "running", false)
    , m_hidden(this, "hidden", false)
{
}

QObject* Status::attr(QString name) {
    return ObjectWithAttributes::attr(name);
}

void Status::closeIn(int ms) {
    QTimer::singleShot(ms, this, [this]() {
        m_manager->removeStatus(m_uid);
    });
}

StatusManager::StatusManager(CoreController* controller)
    : QObject(controller)
    , ObjectWithAttributes(this)
    , m_controller(controller)
{
    qmlRegisterAnonymousType<Status>("Luminosus", 1);
}

QObject* StatusManager::attr(QString name) {
    return ObjectWithAttributes::attr(name);
}

bool StatusManager::hasStatus(const QString& uid) const {
    return m_statusMap.contains(uid);
}

Status* StatusManager::getStatus(const QString& uid) {
    if (auto it = m_statusMap.find(uid); it != m_statusMap.end()) {
        return *it;
    }
    Status* status = new Status(this, uid);
    status->moveToThread(thread());
    m_statusMap[uid] = status;
    emit statusListChanged();
    return status;
}

void StatusManager::removeStatus(const QString& uid) {
    if (!m_statusMap.contains(uid)) return;
    Status* status = m_statusMap.value(uid);
    m_statusMap.remove(uid);
    status->deleteLater();
    emit statusListChanged();
}

QList<Status*> StatusManager::statusList() {
    // if no parent is set, the QML engine may take ownership
    // of the objects and delete them at any time
    updateParents();
    return m_statusMap.values();
}

void StatusManager::updateParents() {
    for (Status* status: m_statusMap) {
        if (!status->QObject::parent()) {
            status->setParent(this);
        }
    }
}
