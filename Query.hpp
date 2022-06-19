#ifndef SMALLDB_QUERY_HPP
#define SMALLDB_QUERY_HPP

#include <vector>
#include <functional>
#include <string>
#include "TypeDB.hpp"
#include "Datatype.hpp"

typedef std::function<bool(const Row&)> Predicate;

class Query {
	Predicate predicate;

public:
	class Builder;

	explicit Query(Predicate predicate);

	[[nodiscard]] std::vector<db_id_t> runOn(const Database& database) const;
};

class Query::Builder {
public:
	enum Operation { EQUALS, NOT_EQUALS, LESS_THAN, GREATER_THAN };

private:
	Predicate currentFunction;

	static std::function<bool(Value a, Value b)> operatorFromOperation(Datatype::Letter type, Operation operation) {
		switch (operation) {
			case EQUALS:
				switch (type) {
					case Datatype::Letter::DoubleL:
						return [](Value a, Value b) { return a.doubleValue == b.doubleValue; };
					case Datatype::Letter::LongL:
						return [](Value a, Value b) { return a.longValue == b.longValue; };
					case Datatype::Letter::StringL:
						return [](Value a, Value b) { return a.stringValue == b.stringValue; };
					case Datatype::Letter::DatetimeL:
						return [](Value a, Value b) { return a.datetimeValue == b.datetimeValue; };
				}
			case NOT_EQUALS:
				switch (type) {
					case Datatype::Letter::DoubleL:
						return [](Value a, Value b) { return a.doubleValue != b.doubleValue; };
					case Datatype::Letter::LongL:
						return [](Value a, Value b) { return a.longValue != b.longValue; };
					case Datatype::Letter::StringL:
						return [](Value a, Value b) { return a.stringValue != b.stringValue; };
					case Datatype::Letter::DatetimeL:
						return [](Value a, Value b) { return a.datetimeValue != b.datetimeValue; };
				}
			case LESS_THAN:
				switch (type) {
					case Datatype::Letter::DoubleL:
						return [](Value a, Value b) { return a.doubleValue < b.doubleValue; };
					case Datatype::Letter::LongL:
						return [](Value a, Value b) { return a.longValue < b.longValue; };
					case Datatype::Letter::StringL:
						return [](Value a, Value b) { return a.stringValue < b.stringValue; };
					case Datatype::Letter::DatetimeL:
						return [](Value a, Value b) { return a.datetimeValue < b.datetimeValue; };
				}
			case GREATER_THAN:
				switch (type) {
					case Datatype::Letter::DoubleL:
						return [](Value a, Value b) { return a.doubleValue > b.doubleValue; };
					case Datatype::Letter::LongL:
						return [](Value a, Value b) { return a.longValue > b.longValue; };
					case Datatype::Letter::StringL:
						return [](Value a, Value b) { return a.stringValue > b.stringValue; };
					case Datatype::Letter::DatetimeL:
						return [](Value a, Value b) { return a.datetimeValue > b.datetimeValue; };
				}
		}

		throw std::exception();
	}

	static Value valueForColumn(std::string_view name, const Row& row) {
		const auto cmp = [name](const std::pair<std::string_view, Value>& p) { return p.first == name;};
		const auto position =  std::find_if(row.begin(), row.end(), cmp);
		return position->second;
	}

public:
	struct ColumnColumn { Operation operation; Datatype::Letter type; std::string column1; std::string column2; };

	struct LiteralDoubleColumn { Operation operation; double value; std::string column2; };
	struct LiteralLongColumn { Operation operation; long value; std::string column2; };
	struct LiteralStringColumn { Operation operation; std::string value; std::string column2; };
	struct LiteralDatetimeColumn { Operation operation; Datetime value; std::string column2; };

	explicit Builder(const ColumnColumn& operation) : currentFunction([operation](const Row& row){
		const auto f = operatorFromOperation(operation.type, operation.operation);
		return f(valueForColumn(operation.column1, row), valueForColumn(operation.column2, row));
	}) {}

	explicit Builder(const LiteralDoubleColumn& operation) : currentFunction([operation](const Row& row){
		const auto f = operatorFromOperation(Datatype::Letter::DoubleL, operation.operation);
		return f(Value { .doubleValue = operation.value }, valueForColumn(operation.column2, row));
	}) {}

	explicit Builder(const LiteralLongColumn& operation) : currentFunction([operation](const Row& row){
		const auto f = operatorFromOperation(Datatype::Letter::LongL, operation.operation);
		return f(Value { .longValue = operation.value }, valueForColumn(operation.column2, row));
	}) {}

	explicit Builder(const LiteralStringColumn& operation) : currentFunction([operation](const Row& row){
		const auto f = operatorFromOperation(Datatype::Letter::StringL, operation.operation);
		return f(Value { .stringValue = operation.value }, valueForColumn(operation.column2, row));
	}) {}

	explicit Builder(const LiteralDatetimeColumn& operation) : currentFunction([operation](const Row& row){
		const auto f = operatorFromOperation(Datatype::Letter::DatetimeL, operation.operation);
		return f(Value { .datetimeValue = operation.value }, valueForColumn(operation.column2, row));
	}) {}

	Query::Builder& andB(const Predicate& value) {
		const Predicate oldFunction = currentFunction;
		currentFunction = [oldFunction, value](const Row& row) {
			return oldFunction(row) && value(row);
		};

		return *this;
	};

	Query::Builder& orB(const Predicate& value) {
		const Predicate oldFunction = currentFunction;
		currentFunction = [oldFunction, value](const Row& row) {
			return oldFunction(row) || value(row);
		};

		return *this;
	};

	[[nodiscard]] Query build() const {
		return Query {currentFunction};
	}
};

#endif //SMALLDB_QUERY_HPP
