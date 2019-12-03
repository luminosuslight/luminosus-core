#include "ObjectWithAttributes.h"

#include <core/helpers/SmartAttribute.h>

#include <QDebug>



ObjectWithAttributes::ObjectWithAttributes(QObject* parent)
    : m_parent(parent)
{

}

QObject* ObjectWithAttributes::attr(QString name) {
    if (!m_attributes.contains(name)) {
        qWarning() << "Object has no attribute " << name;
    }
    return static_cast<QObject*>(m_attributes.value(name, nullptr));
}

void ObjectWithAttributes::registerAttribute(SmartAttribute* attr) {
    m_attributes[attr->name()] = attr;
    if (attr->persistent()) {
        m_persistentAttributes.append(attr);
    }
}

void ObjectWithAttributes::writeAttributesTo(QCborMap& state) const {
    for (SmartAttribute* attr: m_persistentAttributes) {
        if (!attr) continue;
        attr->writeTo(state);
    }
}

void ObjectWithAttributes::readAttributesFrom(const QCborMap& state) {
    for (SmartAttribute* attr: m_persistentAttributes) {
        if (!attr) continue;
        attr->readFrom(state);
    }
}
