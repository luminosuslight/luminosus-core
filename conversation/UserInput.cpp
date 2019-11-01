#include "UserInput.h"

#include "core/helpers/utils.h"

#include <QJsonArray>
#include <QDebug>
#include <QDateTime>


UserInput::UserInput()
    : m_timestamp(QDateTime::currentMSecsSinceEpoch())
{

}

void UserInput::setNlpResult(const QJsonObject& googleNlpResult) {
    m_googleNlpResult = googleNlpResult;

    // TODO: check for negation

    // NOTE: lemmas are all stored as lowercase

    for (QJsonValueRef tokenRef: googleNlpResult["tokens"].toArray()) {
        QJsonObject token = tokenRef.toObject();
        QJsonObject textObject = token["text"].toObject();
        QJsonObject partOfSpeech = token["partOfSpeech"].toObject();
        QJsonObject dependencyEdge = token["dependencyEdge"].toObject();
        QString lemma = token["lemma"].toString().toLower();
        QString type = partOfSpeech["tag"].toString();
        QString structure = dependencyEdge["label"].toString();

        m_words.append(textObject["content"].toString());
        m_lemmas.append(lemma);
        m_types.append(type);
        m_structure.append(structure);
        m_lemmasByType[type].append(lemma);
        m_lemmasByStructure[structure].append(lemma);
    }
}

bool UserInput::hasNlpResult() const {
    return !m_googleNlpResult.isEmpty();
}

void UserInput::printNlpResult() const {
    for (QJsonValueRef tokenRef: m_googleNlpResult["tokens"].toArray()) {
        QJsonObject token = tokenRef.toObject();
        qDebug()<< "\n---\n" << token;
    }
    qDebug()<< "\n---\n";
    qDebug() << m_words;
    qDebug() << m_lemmas;
    qDebug() << m_types;
    qDebug() << m_structure;
    qDebug() << m_lemmasByType;
    qDebug() << m_lemmasByStructure;
    qDebug() << "Duration: " << getDuration() << " seconds";
    qDebug() << "Percentage: " << getPercentage();
}

bool UserInput::contains(QString word) const {
    if (!m_googleNlpResult.isEmpty()) {
        return m_lemmas.contains(word) || m_words.contains(word);
    }
    return m_transcription.contains(word);
}

bool UserInput::hasNumber() const {
    return m_types.contains("NUM");
}

bool UserInput::hasDuration() const {
    if (m_language == "de-DE") {
        return m_lemmas.contains("sekunde") || m_lemmas.contains("minute") || m_lemmas.contains("stunde")
                || m_lemmas.contains("sekunden") || m_lemmas.contains("minuten") || m_lemmas.contains("stunden");
    } else {
        return m_lemmas.contains("second") || m_lemmas.contains("minute") || m_lemmas.contains("hour")
                || m_lemmas.contains("seconds") || m_lemmas.contains("minutes") || m_lemmas.contains("hours");
    }

}

bool UserInput::hasPercentage() const {
    return m_lemmas.contains("percent") || m_lemmas.contains("%") || m_lemmas.contains("prozent");
}

double UserInput::getDuration() const {
    double seconds = 0.0;
    for (int i=0; i < m_types.size(); ++i) {
        if (m_types[i] == "NUM") {
            QString nextWord = m_lemmas.value(i+1);
            QString wordBefore = m_lemmas.value(i-1);
            if (nextWord.startsWith("second") || nextWord.startsWith("sekunde")) {
                seconds += stringToDouble(m_lemmas[i]);
            } else if (nextWord.startsWith("minute") || nextWord.startsWith("minute")) {
                seconds += stringToDouble(m_lemmas[i]) * 60;
            } else if (nextWord.startsWith("hour") || nextWord.startsWith("stunde")) {
                seconds += stringToDouble(m_lemmas[i]) * 3600;
            } else if (wordBefore.startsWith("minute") || wordBefore.startsWith("minute")) {
                // i.e. "start a 1 minute 30 timer"
                // -> these are seconds:
                seconds += stringToDouble(m_lemmas[i]);
            }
        }
    }
    return seconds;
}

double UserInput::getPercentage() const {
    for (int i=0; i < m_types.size(); ++i) {
        if (m_types[i] == "NUM") {
            QString nextWord = m_lemmas.value(i+1);
            if (nextWord == "percent" || nextWord == "%" || nextWord == "prozent") {
                return stringToDouble(m_lemmas[i]) / 100.0;
            }
        }
    }
    qWarning() << "InputPhrase: percentage requested but not found";
    return 0.0;
}
