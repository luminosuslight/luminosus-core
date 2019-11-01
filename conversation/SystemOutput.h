#ifndef SYSTEMOUTPUT_H
#define SYSTEMOUTPUT_H

#include <QString>

namespace ConversationConstants {
    // don't use enum because ints are comparable (always > auto > never)
    static const int CACHE_NEVER = -1;
    static const int CACHE_AUTO = 0;
    static const int CACHE_ALWAYS = 1;
}


struct SystemOutput {

    SystemOutput();

    void setSsml(QString ssml);

    QString m_plainText;
    QString m_ssml;
    QString m_language;
    QString m_voice;
    double m_pitch;
    double m_speed;
    int m_cacheHint;
    long m_timestamp;
};

#endif // SYSTEMOUTPUT_H
