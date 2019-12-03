#ifndef COREUTILS_H
#define COREUTILS_H

#include <QtMath>
#include <QDataStream>
#include <QByteArray>
#include <QCborMap>

#include <algorithm>
#include <chrono>


// This file includes some often used utility functions.

// Limits / saturates a value with min and max bounds:
template<typename T1, typename T2, typename T3>
T2 limit(T1 min, T2 value, T3 max) {
	return qMax(T2(min), qMin(value, T2(max)));
}

template<typename T>
T limitToOne(T value) {
    return std::max(0.0, std::min(value, 1.0));
}

template<typename T, typename E>
bool contains(const T& list, const E& el) {
    return list.end() != std::find(list.begin(), list.end(), el);
}

template<typename T, typename E>
bool mapContains(const T& list, const E& el) {
    return list.end() != list.find(el);
}

template<typename T, typename E>
bool appendUnique(T& vec, E& el) {
    if (!contains(vec, el)) {
        vec.push_back(el);
        return true;
    }
    return false;
}

template<typename T, typename E>
void removeUnique(T& vec, E& el) {
    auto pos = std::find(vec.begin(), vec.end(), el);
    if (pos != vec.end()) {
        vec.erase(pos);
    }
}

namespace HighResTime {  // -------------------

typedef std::chrono::time_point<std::chrono::system_clock> time_point_t;

inline time_point_t now() {
    return std::chrono::system_clock::now();
}

inline double elapsedSecSince(time_point_t start) {
    time_point_t now = HighResTime::now();
    std::chrono::duration<double> elapsed_seconds_duration = now - start;
    double elapsedSeconds = elapsed_seconds_duration.count();
    return elapsedSeconds;
}

inline double diff(time_point_t end, time_point_t start) {
    std::chrono::duration<double> elapsed_seconds_duration = end - start;
    return elapsed_seconds_duration.count();
}

inline double getElapsedSecAndUpdate(time_point_t& lastTime) {
    time_point_t now = HighResTime::now();
    double elapsedSeconds = diff(now, lastTime);
    lastTime = now;
    return elapsedSeconds;
}

}  // end namespace HighResTime -----------------

template<typename T>
QString serialize(const T& x)
{
	QByteArray ba;
	QDataStream s(&ba, QIODevice::WriteOnly);
	s << x;
	QString str = QString::fromLatin1(ba.toBase64());
	return str;
}

template<typename T>
T deserialize(const QString& str)
{
	QByteArray ba = QByteArray::fromBase64(str.toLatin1());
	QDataStream s(&ba, QIODevice::ReadOnly);
	T res; s >> res;
	return res;
}

template<typename T>
QByteArray serializeBinary(const T& x)
{
    QByteArray ba;
    QDataStream s(&ba, QIODevice::WriteOnly);
    s << x;
    return ba;
}

template<typename T>
T deserializeBinary(const QByteArray& x)
{
    QByteArray ba = x;
    QDataStream s(&ba, QIODevice::ReadOnly);
    T res;
    s >> res;
    return res;
}

inline bool isEqualInsensitive(const QString& str1, const QString& str2) {
	int result = str1.compare(str2, Qt::CaseInsensitive);
	return result == 0;
}

// ((a % base) + base) % base;

template<typename T>
T realMod(T value, T base) {
    return std::fmod(std::fmod(value, base) + base, base);
}

template <typename T>
T intMod(T value, T base) {
  return ((value % base) + base) % base;
}


template <typename T>
T almostMedian(QVector<T> v) {
    // the median would be the average of the middle two, if the size is even
    // this just returns the first one in this case
    size_t n = v.size() / 2;
    std::nth_element(v.begin(), v.begin() + n, v.end());
    return v[n];
}

inline double stringToDouble(QString text) {
    if (text.toDouble() > 0.0) {
        return text.toDouble();
    } else if (text == "one" || text == "eins") {
        return 1.0;
    } else if (text == "two" || text == "zwei") {
        return 2.0;
    } else if (text == "three" || text == "drei") {
        return 3.0;
    } else if (text == "four" || text == "vier") {
        return 4.0;
    } else if (text == "five" || text == "fünf") {
        return 5.0;
    } else if (text == "six" || text == "sechs") {
        return 6.0;
    } else if (text == "seven" || text == "sieben") {
        return 7.0;
    } else if (text == "eight" || text == "acht") {
        return 8.0;
    } else if (text == "nine" || text == "neun") {
        return 9.0;
    } else if (text == "ten" || text == "zehn") {
        return 10.0;
    } else if (text == "eleven" || text == "elf") {
        return 11.0;
    } else if (text == "twelve" || text == "zwölf") {
        return 12.0;
    } else {
        return 0.0;
    }
}

inline QDataStream& operator<<(QDataStream& out, const QCborMap& obj) {
    out << obj.toCborValue().toCbor();
    return out;
}

inline QDataStream& operator>>(QDataStream& in, QCborMap& obj) {
    QByteArray cbor;
    in >> cbor;
    obj = QCborValue::fromCbor(cbor).toMap();
    return in;
}

// Examples for user-defined literals:

//long double operator "" _²(long double v) {
//    return std::pow(v, 2);
//}

//std::string operator"" _upper(const char * str, size_t len) {
//    auto s = std::string{str, len};
//    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
//    return s;
//}

#endif // COREUTILS_H

