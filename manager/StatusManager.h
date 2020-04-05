#ifndef STATUSMANAGER_H
#define STATUSMANAGER_H

#include "core/helpers/SmartAttribute.h"
#include "core/helpers/ObjectWithAttributes.h"

#include <QObject>
#include <QMap>

class CoreController;
class StatusManager;


class Status : public QObject, public ObjectWithAttributes {

    Q_OBJECT

public:
    explicit Status(StatusManager* statusManager, const QString& uid);

public slots:
    QObject* attr(QString name);

    void closeIn(int ms);

public:
    StatusManager* m_manager;
    StringAttribute m_uid;
    RgbAttribute m_color;
    StringAttribute m_title;
    StringAttribute m_text;
    DoubleAttribute m_progress;
    BoolAttribute m_running;
    BoolAttribute m_hidden;
};


class StatusManager : public QObject, public ObjectWithAttributes {

    Q_OBJECT

    Q_PROPERTY(QList<Status*> statusList READ statusList NOTIFY statusListChanged)

public:
    explicit StatusManager(CoreController* controller);

signals:
    void statusListChanged();

public slots:
    QObject* attr(QString name);

    bool hasStatus(const QString& uid) const;
    Status* getStatus(const QString& uid);
    void removeStatus(const QString& uid);

    QList<Status*> statusList();

private:
    void updateParents();

protected:
    CoreController* const m_controller;

    QMap<QString, Status*> m_statusMap;
};

#endif // STATUSMANAGER_H
