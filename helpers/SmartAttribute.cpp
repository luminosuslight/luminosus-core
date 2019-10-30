#include "SmartAttribute.h"

#include "core/helpers/ObjectWithProperties.h"
#include "core/helpers/utils.h"


SmartAttribute::SmartAttribute(ObjectWithProperties* owp, QString name, bool persistent)
    : QObject(owp->parent())
    , m_name(name)
    , m_persistent(persistent)
{
    owp->registerAttribute(this);
}

SmartAttribute::SmartAttribute(void*, QObject* parent, QString name, bool persistent)
    : QObject(parent)
    , m_name(name)
    , m_persistent(persistent)
{

}

DoubleAttribute::DoubleAttribute(ObjectWithProperties* block, QString name, double initialValue, double min, double max, bool persistent)
    : SmartAttribute(block, name, persistent)
    , m_value(initialValue)
    , m_min(min)
    , m_max(max)
{

}

DoubleAttribute::DoubleAttribute(void*, QObject* parent, QString name, double initialValue, double min, double max, bool persistent)
    : SmartAttribute(nullptr, parent, name, persistent)
    , m_value(initialValue)
    , m_min(min)
    , m_max(max)
{

}

void DoubleAttribute::writeTo(QJsonObject& state) const {
    state[m_name] = getValue();
}

void DoubleAttribute::readFrom(const QJsonObject& state) {
    setValue(state[m_name].toDouble());
}

void DoubleAttribute::setValue(double value) {
    value = limit(m_min, value, m_max);
    if (value == m_value) return;
    m_value = limit(m_min, value, m_max);
    emit valueChanged();
}


IntegerAttribute::IntegerAttribute(ObjectWithProperties* block, QString name, int initialValue, int min, int max, bool persistent)
    : SmartAttribute(block, name, persistent)
    , m_value(initialValue)
    , m_min(min)
    , m_max(max)
{

}

IntegerAttribute::IntegerAttribute(void*, QObject* parent, QString name, int initialValue, int min, int max, bool persistent)
    : SmartAttribute(nullptr, parent, name, persistent)
    , m_value(initialValue)
    , m_min(min)
    , m_max(max)
{

}

void IntegerAttribute::writeTo(QJsonObject& state) const {
    state[m_name] = getValue();
}

void IntegerAttribute::readFrom(const QJsonObject& state) {
    setValue(state[m_name].toInt());
}

void IntegerAttribute::setValue(int value) {
    // TODO: compare with limited value
    if (value == m_value) return;
    m_value = limit(m_min, value, m_max);
    emit valueChanged();
}


StringAttribute::StringAttribute(ObjectWithProperties* block, QString name, QString initialValue, bool persistent)
    : SmartAttribute(block, name, persistent)
    , m_value(initialValue)
{

}

StringAttribute::StringAttribute(void*, QObject* parent, QString name, QString initialValue, bool persistent)
    : SmartAttribute(nullptr, parent, name, persistent)
    , m_value(initialValue)
{

}

void StringAttribute::writeTo(QJsonObject& state) const {
    state[m_name] = getValue();
}

void StringAttribute::readFrom(const QJsonObject& state) {
    setValue(state[m_name].toString());
}


BoolAttribute::BoolAttribute(ObjectWithProperties* block, QString name, bool initialValue, bool persistent)
    : SmartAttribute(block, name, persistent)
    , m_value(initialValue)
{

}

BoolAttribute::BoolAttribute(void*, QObject* parent, QString name, bool initialValue, bool persistent)
    : SmartAttribute(nullptr, parent, name, persistent)
    , m_value(initialValue)
{

}

void BoolAttribute::writeTo(QJsonObject& state) const {
    state[m_name] = getValue();
}

void BoolAttribute::readFrom(const QJsonObject& state) {
    setValue(state[m_name].toBool());
}

RgbAttribute::RgbAttribute(ObjectWithProperties* block, QString name, const RGB& initialValue, bool persistent)
    : SmartAttribute(block, name, persistent)
    , m_value(initialValue)
{

}

RgbAttribute::RgbAttribute(void*, QObject* parent, QString name, const RGB& initialValue, bool persistent)
    : SmartAttribute(nullptr, parent, name, persistent)
    , m_value(initialValue)
{

}

void RgbAttribute::writeTo(QJsonObject& state) const {
    state[m_name + "r"] = m_value.r;
    state[m_name + "g"] = m_value.g;
    state[m_name + "b"] = m_value.b;
}

void RgbAttribute::readFrom(const QJsonObject& state) {
    double r = state[m_name + "r"].toDouble();
    double g = state[m_name + "g"].toDouble();
    double b = state[m_name + "b"].toDouble();
    setValue({r, g, b});
}

double RgbAttribute::hue() const {
    if (val() == 0.0 || sat() == 0.0) {
        return m_tempHsv.h;
    } else {
        return HSV(m_value).h;
    }
}

void RgbAttribute::setHue(double value) {
    HSV hsv(m_value);
    hsv.h = value;
    m_value = RGB(hsv);
    m_tempHsv.h = value;
    emit valueChanged();
}

double RgbAttribute::sat() const {
    if (val() == 0.0) {
        return m_tempHsv.s;
    } else {
        return HSV(m_value).s;
    }
}

void RgbAttribute::setSat(double value) {
    HSV hsv(m_value);
    if (sat() == 0.0) {
        hsv = HSV(m_tempHsv.h, m_tempHsv.s, m_value.max());
    }
    hsv.s = value;
    m_value = RGB(hsv);
    m_tempHsv.s = value;
    emit valueChanged();
}

void RgbAttribute::setVal(double value) {
    HSV hsv(m_value);
    if (m_value.max() == 0.0) {
        hsv = HSV(m_tempHsv.h, m_tempHsv.s, 0);
    }
    hsv.v = value;
    m_value = RGB(hsv);
    emit valueChanged();
}

void RgbAttribute::setQColor(QColor value) {
    m_value.r = value.redF();
    m_value.g = value.greenF();
    m_value.b = value.blueF();
    emit valueChanged();
}

QColor RgbAttribute::getGlow() const {
    double max = m_value.max();
    if (max <= 0) {
        return QColor::fromRgbF(0, 0, 0, 1.0);
    } else {
        return QColor::fromRgbF(m_value.r / max, m_value.g / max, m_value.b / max, 1.0);
    }
}

HsvAttribute::HsvAttribute(ObjectWithProperties* block, QString name, const HSV& initialValue, bool persistent)
    : SmartAttribute(block, name, persistent)
    , m_value(initialValue)
{

}

HsvAttribute::HsvAttribute(void*, QObject* parent, QString name, const HSV& initialValue, bool persistent)
    : SmartAttribute(nullptr, parent, name, persistent)
    , m_value(initialValue)
{

}

void HsvAttribute::writeTo(QJsonObject& state) const {
    state[m_name + "h"] = m_value.h;
    state[m_name + "s"] = m_value.s;
    state[m_name + "v"] = m_value.v;

}

void HsvAttribute::readFrom(const QJsonObject& state) {
    double h = state[m_name + "h"].toDouble();
    double s = state[m_name + "s"].toDouble();
    double v = state[m_name + "v"].toDouble();
    setValue({h, s, v});
}

void HsvAttribute::setValue(const HSV& value) {
    if (value == m_value) return;
    m_value = value;
    emit valueChanged();
}

void HsvAttribute::setQColor(QColor value) {
    m_value.h = value.hueF();
    m_value.s = value.saturationF();
    m_value.v = value.valueF();
    emit valueChanged();
}

void HsvAttribute::mixHtp(const HSV& other) {
    QColor col = QColor::fromHsvF(m_value.h, m_value.s, m_value.v).toRgb();
    RGB x(col.redF(), col.greenF(), col.blueF());
    QColor col2 = QColor::fromHsvF(other.h, other.s, other.v).toRgb();
    RGB y(col2.redF(), col2.greenF(), col2.blueF());
    x.mixHtp(y);
    setQColor(QColor::fromRgbF(x.r, x.g, x.b));
}
