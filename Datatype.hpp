#ifndef SMALLDB_DATATYPE_HPP
#define SMALLDB_DATATYPE_HPP

#include "TypeDB.hpp"
#include "Exceptions.hpp"

class Datatype {
public:
	enum Letter {
		DoubleL = 'D',
		LongL = 'L',
		StringL = 'S',
		DatetimeL = 'T',
	};

private:
	constexpr Datatype(Datatype::Letter letterF, uint32_t sizeF) : letterF(letterF), sizeF(sizeF) {}

	Datatype::Letter letterF;
	uint32_t sizeF;

public:
	[[nodiscard]] Datatype::Letter letter() const { return letterF; }
	[[nodiscard]] uint32_t size() const { return sizeF; }

	[[nodiscard]] static constexpr Datatype Double();
	[[nodiscard]] static constexpr Datatype Long();
	[[nodiscard]] static constexpr Datatype String(uint32_t size);
	[[nodiscard]] static constexpr Datatype Datetime();
};

constexpr Datatype Datatype::Double() {
	return {Datatype::Letter::DoubleL, 64};
}

constexpr Datatype Datatype::Long() {
	return {Datatype::Letter::LongL, 64};
}

constexpr Datatype Datatype::String(uint32_t size) {
	return {Datatype::Letter::StringL, size};
}

constexpr Datatype Datatype::Datetime() {
	return {Datatype::Letter::DatetimeL, 6*32};
}

#endif //SMALLDB_DATATYPE_HPP
