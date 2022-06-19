#ifndef SMALLDB_SMALLDB_HPP
#define SMALLDB_SMALLDB_HPP

#include <string_view>
#include <vector>
#include <utility>
#include <string>
#include <unordered_map>
#include <cassert>

#include "Datatype.hpp"
#include "Exceptions.hpp"
#include "TypeDB.hpp"
#include "Datetime.hpp"

class Query;

class SmallDB {
	std::vector<std::pair<std::string, Datatype>> columns;
	Database database;
	std::vector<std::string> strings; //Place to store all strings, but not column names

	[[nodiscard]] const Value& iterator(db_id_t id, std::string_view name) const {
		const auto& row = database.at(id);
		const auto cmp = [name](const std::pair<std::string_view, Value>& p) { return p.first == name;};
		const auto position =  std::find_if(row.begin(), row.end(), cmp);
		if(position == row.end()) {
			throw ColumnNotFound();
		}
		return position->second;
	}

	[[nodiscard]] Value& iterator(db_id_t id, std::string_view name) {
		auto& row = database.at(id);
		const auto cmp = [name](const std::pair<std::string_view, Value>& p) { return p.first == name;};
		auto position = std::find_if(row.begin(), row.end(), cmp);
		if(position == row.end()) {
			throw ColumnNotFound();
		}
		return position->second;
	}

	template <class T>
	static inline T read(std::ifstream& file);
	template <class T>
	static inline void write(std::ofstream& file, T value);
public:
	explicit SmallDB(std::vector<std::pair<std::string, Datatype>> columns);
	explicit SmallDB(const std::string& fileName);

	[[nodiscard]] db_id_t insertRow();
	void deleteRow(db_id_t id);
	[[nodiscard]] std::vector<db_id_t> find(const Query& query) const;

	[[nodiscard]] double readDouble(db_id_t id, const std::string_view& name) const;
	void writeDouble(db_id_t id, const std::string_view& name, double value);
	[[nodiscard]] long readLong(db_id_t id, const std::string_view& name) const;
	void writeLong(db_id_t id, const std::string_view& name, long value);
	[[nodiscard]] const std::string_view& readString(db_id_t id, const std::string_view& name) const;
	void writeString(db_id_t id, const std::string_view& name, std::string_view value);
	[[nodiscard]] Datetime readDatetime(db_id_t id, const std::string_view& name) const;
	void writeDatetime(db_id_t id, const std::string_view& name, Datetime value);

	void save(const std::string& fileName) const;
};


#endif //SMALLDB_SMALLDB_HPP
