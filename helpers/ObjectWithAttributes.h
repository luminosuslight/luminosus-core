#ifndef OBJECTWITHPROPERTIES_H
#define OBJECTWITHPROPERTIES_H

#include <QMap>
#include <QString>
#include <QPointer>

class SmartAttribute;


class ObjectWithAttributes {


public:
    ObjectWithAttributes(QObject* parent);

    QObject* parent() { return m_parent; }

    QObject* attr(QString name);
    /**
     * @brief registerAttribute registers an attribute to be available by attr()
     * and to be persisted if requested
     * @param attr a pointer to the attribute to register
     */
    void registerAttribute(SmartAttribute* attr);

    void writeAttributesTo(QCborMap& state) const;
    void readAttributesFrom(const QCborMap& state);

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
