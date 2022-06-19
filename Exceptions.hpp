#ifndef SMALLDB_EXCEPTIONS_HPP
#define SMALLDB_EXCEPTIONS_HPP

#include <exception>

class StringTooLong : std::exception {};

class ColumnNotFound : std::exception {};

#endif //SMALLDB_EXCEPTIONS_HPP
