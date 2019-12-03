#ifndef QSTRING_LITERAL_H
#define QSTRING_LITERAL_H
#include <QString>

QString operator"" _q(const char * str, std::size_t len);

#endif // QSTRING_LITERAL_H
