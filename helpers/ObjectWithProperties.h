#ifndef OBJECTWITHPROPERTIES_H
#define OBJECTWITHPROPERTIES_H

#include <QMap>
#include <QString>
#include <QPointer>

class SmartAttribute;


class ObjectWithProperties {


public:
    ObjectWithProperties(QObject* parent);

    QObject* parent() { return m_parent; }

    QObject* attr(QString name);

    void registerAttribute(SmartAttribute* attr);

    void writeAttributesTo(QJsonObject& state) const;
    void readAttributesFrom(const QJsonObject& state);

protected:

    QObject* const m_parent;

    /**
     * @brief m_attributes contains all SmartAttributes of this object
     */
    QMap<QString, QPointer<SmartAttribute>> m_attributes;

    /**
     * @brief m_persistentAttributes contains pointers to all attributes that should be persistet
     */
    QVector<QPointer<SmartAttribute>> m_persistentAttributes;
};

#endif // OBJECTWITHPROPERTIES_H
