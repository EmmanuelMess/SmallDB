#ifndef SMALLDB_TYPEDB_HPP
#define SMALLDB_TYPEDB_HPP

#include <cstdint>
#include <utility>
#include <vector>
#include <string_view>
#include "Datetime.hpp"

typedef uint64_t db_id_t;//TODO this very probably needs to change to an iterator of std::vector<Row>

union Value {
	double doubleValue;
	long longValue;
	std::string_view stringValue;
	Datetime datetimeValue;
};

typedef std::vector<std::pair<const std::string_view, Value>> Row;

typedef std::vector<Row> Database;

#endif //SMALLDB_TYPEDB_HPP
