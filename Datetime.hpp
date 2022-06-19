#ifndef SMALLDB_DATETIME_HPP
#define SMALLDB_DATETIME_HPP

#include <cstdint>
#include <compare>

struct Datetime {
	auto operator <=>(const Datetime &b) const = default;

	// KEEP IN ORDER (most significant first) for default operator <=>
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
};


#endif //SMALLDB_DATETIME_HPP
