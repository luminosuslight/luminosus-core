#include "qstring_literal.h"

#include <string>

QString operator"" _q(const char* str, std::size_t len) {
    return QString::fromStdString(std::string(str, len));
}
