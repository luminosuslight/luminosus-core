#include "ObjectWithProperties.h"

#include <core/helpers/SmartAttribute.h>

#include <QDebug>



ObjectWithProperties::ObjectWithProperties(QObject* parent)
    : m_parent(parent)
{

}

QObject* ObjectWithProperties::attr(QString name) {
    if (!m_attributes.contains(name)) {
        qWarning() << "Object has no attribute " << name;
    }
    return static_cast<QObject*>(m_attributes.value(name, nullptr));
}

void ObjectWithProperties::registerAttribute(SmartAttribute* attr) {
    m_attributes[attr->name()] = attr;
    if (attr->persistent()) {
        m_persistentAttributes.append(attr);
    }
}

void ObjectWithProperties::writeAttributesTo(QJsonObject& state) const {
    for (SmartAttribute* attr: m_persistentAttributes) {
        if (!attr) continue;
        attr->writeTo(state);
    }
}

void ObjectWithProperties::readAttributesFrom(const QJsonObject& state) {
    for (SmartAttribute* attr: m_persistentAttributes) {
        if (!attr) continue;
        attr->readFrom(state);
    }
}
