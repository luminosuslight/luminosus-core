#include "Command.h"

#include "ConversationActionInterface.h"

#include <QDateTime>


Command::Command(QObject* parent, QString origin)
    : QObject(parent)
    , m_timestamp(QDateTime::currentMSecsSinceEpoch())
    , m_origin(origin)
    , m_nextAction(nullptr)
    , m_finished(false)
    , m_successful(false)
{

}

UserInput& Command::newInput() {
    m_inputs.append(UserInput());
    return m_inputs.last();
}

void Command::addInput(const UserInput& input) {
    m_inputs.append(input);
}

UserInput& Command::currentInput() {
    if (m_inputs.isEmpty()) newInput();
    return m_inputs.last();
}

const UserInput& Command::currentInput() const {
    Q_ASSERT_X(!m_inputs.isEmpty(), "Command::currentInput() const", "Command has no input.");
    return m_inputs.last();
}

SystemOutput& Command::newOutput() {
    m_outputs.append(SystemOutput());
    return m_outputs.last();
}

SystemOutput& Command::currentOutput() {
    if (m_outputs.isEmpty()) newOutput();
    return m_outputs.last();
}

void Command::setParameter(QString key, QString value) {
    if (value.isEmpty()) return;
    m_parameters[key] = value;
}

QString Command::getParameter(QString key) const {
    return m_parameters.value(key);
}

bool Command::hasParameter(QString key) const {
    return m_parameters.contains(key);
}

bool Command::hasDuration() const {
    if (hasParameter("durationInSeconds")) return true;
    for (const UserInput& input: m_inputs) {
        if (input.hasDuration()) {
            return true;
        }
    }
    return false;
}

double Command::getDuration() const {
    if (hasParameter("durationInSeconds")) return getParameter("durationInSeconds").toDouble();
    for (const UserInput& input: m_inputs) {
        if (input.hasDuration()) {
            return input.getDuration();
        }
    }
    return -1;
}

bool Command::hasPercentage() const {
    if (hasParameter("percentage")) return true;
    for (const UserInput& input: m_inputs) {
        if (input.hasPercentage()) {
            return true;
        }
    }
    return false;
}

double Command::getPercentage() const {
    if (hasParameter("percentage")) return getParameter("percentage").toDouble();
    for (const UserInput& input: m_inputs) {
        if (input.hasPercentage()) {
            return input.getPercentage();
        }
    }
    return -1;
}
