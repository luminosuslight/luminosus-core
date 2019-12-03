#ifndef SPEECHINPUT_H
#define SPEECHINPUT_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QCborMap>


struct UserInput {

    UserInput();

    void setRawAudio(const QByteArray& value) { m_rawAudio = value; }
    void setLanguage(QString lang) { m_language = lang; }
    QString getLanguage() const { return m_language; }

    void setTranscription(QString value) { m_transcription = value; }
    void setConfidence(double value) { m_confidence = value; }
    QString getTranscription() const { return m_transcription; }

    // ------------ Natural Languange Processing -------------
    void setNlpResult(const QCborMap& googleNlpResult);
    bool hasNlpResult() const;
    void printNlpResult() const;

    QString subject() const { return m_lemmasByStructure["NSUBJ"].value(0); }
    QString predicate() const { return m_lemmasByStructure["ROOT"].value(0); }
    QString object() const { return m_lemmasByStructure["DOBJ"].value(0, m_lemmasByStructure["POBJ"].value(0)); }

    bool containsNoun(QString lemma) const { return m_lemmasByType["NOUN"].contains(lemma); }
    bool containsVerb(QString lemma) const { return m_lemmasByType["VERB"].contains(lemma); }

    bool contains(QString word) const;

    bool hasNumber() const;
    bool hasDuration() const;
    bool hasWallTime() const { return false; }
    bool hasDate() const { return false; }
    bool hasPercentage() const;
    bool hasAbsoluteNumber() const { return false; }

    // double getNumber() const;
    double getDuration() const;
    // ? getWallTime() const;
    // ? getDate() const;
    double getPercentage() const;
    // double getAbsoluteNumber() const;

    // ----------------------------------------------------------

    long m_timestamp;

    // from Microphone:
    QByteArray m_rawAudio;
    QString m_language;

    // after Speech-to-Text recognition:
    QString m_transcription;
    double m_confidence;

    // from Natural Languange Processing:
    QCborMap m_googleNlpResult;
    QVector<QString> m_words;
    QVector<QString> m_lemmas;
    QVector<QString> m_types;
    QVector<QString> m_structure;
    QMap<QString, QVector<QString>> m_lemmasByType;
    QMap<QString, QVector<QString>> m_lemmasByStructure;

    // after suitable action was found:
    QString m_actionName;
    QString m_matchedCriteria;

    // after action was performed:
    QString m_logicResultId;
};

#endif // SPEECHINPUT_H
