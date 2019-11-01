#ifndef COMMAND_H
#define COMMAND_H

#include "UserInput.h"
#include "SystemOutput.h"

#include <QObject>
#include <QMap>
#include <QSet>

class ConversationActionInterface;

namespace CommandOrigin {
static const QString LOCAL = "LOCAL";
static const QString APP = "APP";
static const QString WEB = "WEB";
}


struct Command : public QObject {

    Q_OBJECT

public:

    explicit Command(QObject* parent, QString origin="");

    UserInput& newInput();
    void addInput(const UserInput& input);
    UserInput& currentInput();
    const UserInput& currentInput() const;

    SystemOutput& newOutput();
    SystemOutput& currentOutput();

    void setParameter(QString key, QString value);
    QString getParameter(QString key) const;
    bool hasParameter(QString key) const;

    bool hasDuration() const;
    double getDuration() const;
    bool hasPercentage() const;
    double getPercentage() const;

    // --------------------------------------------------

    long m_timestamp;
    QString m_origin;
    QVector<UserInput> m_inputs;
    QVector<SystemOutput> m_outputs;
    QMap<QString, QString> m_parameters;
    QSet<ConversationActionInterface*> m_actionRestrictions;
    ConversationActionInterface* m_nextAction;
    bool m_finished;
    bool m_successful;
};

#endif // COMMAND_H
