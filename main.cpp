#include <iostream>
#include <vector>
#include <utility>

#include "SmallDB.hpp"
#include "Query.hpp"

constexpr auto name = "databaseTest.db";

void testDb() {
	const auto m = std::vector<std::pair<std::string, Datatype>> {
		{"bar", Datatype::Double()},
		{"world", Datatype::String(10)},
		{"me", Datatype::Datetime()},
		{"mea", Datatype::Long()},
		{"meff", Datatype::Long()},
	};
	SmallDB db(m);

	auto id = db.insertRow();

	db.writeLong(id, "mea", 10);
	db.writeString(id, "world", "你好");
	db.writeDatetime(id, "me", {1, 1, 1, 1, 1, 1});

	std::cout << db.readLong(id, "mea") << "\n";

	const auto query = Query::Builder(Query::Builder::LiteralLongColumn { Query::Builder::Operation::EQUALS, 10L, "mea"}).build();

	std::cout << db.find(query).size() << "\n";
	db.save(name);
}

void reopenDb() {
	SmallDB db(name);

	const auto query = Query::Builder(Query::Builder::LiteralLongColumn { Query::Builder::Operation::EQUALS, 10L, "mea"}).build();

	std::cout << db.find(query).size() << "\n";

	db.deleteRow(db.find(query).front());

	std::cout << db.find(query).size() << "\n";
}

int main() {
	testDb();
	reopenDb();

	return 0;
}
