#include "SystemOutput.h"

#include <QDateTime>
#include <QRegularExpression>

SystemOutput::SystemOutput()
    : m_pitch(1.0)
    , m_speed(1.0)
    , m_cacheHint(ConversationConstants::CACHE_AUTO)
    , m_timestamp(QDateTime::currentMSecsSinceEpoch())
{

}

void SystemOutput::setSsml(QString ssml) {
    m_ssml = ssml;
    m_plainText = ssml.replace(QRegularExpression("<.*?>"), "").simplified();  // ? after * searches for shortest match
}
