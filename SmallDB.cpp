#include "SmallDB.hpp"
#include "Query.hpp"

#include <memory>
#include <utility>
#include <fstream>

SmallDB::SmallDB(std::vector<std::pair<std::string, Datatype>> columns) : columns(std::move(columns)), database(), strings() {}

template <class T>
inline T SmallDB::read(std::ifstream& file) {
	char value[sizeof(T)];
	file.read(value, sizeof(T));
	return *reinterpret_cast<T*>(value);
}


SmallDB::SmallDB(const std::string& fileName) : columns(), database(), strings() {
	std::ifstream file(fileName, std::ios_base::in | std::ios_base::binary);

	auto rowNumber = read<uint32_t>(file);
	auto headerLength = read<uint32_t>(file);

	do {
		std::string columnName;
		getline(file, columnName);

		headerLength -= columnName.size() + sizeof(uint8_t);//TODO test unicode

		auto typeLetter = read<uint8_t>(file);

		headerLength -= sizeof(uint8_t);

		if(typeLetter == Datatype::Letter::StringL) {
			auto columnLength = read<uint32_t>(file);

			headerLength -= sizeof(uint32_t);

			columns.emplace_back(columnName, Datatype::String(columnLength));
		} else {
			switch(typeLetter) {
				case Datatype::Letter::DoubleL:
					columns.emplace_back(columnName, Datatype::Double());
					break;
				case Datatype::Letter::LongL:
					columns.emplace_back(columnName, Datatype::Long());
					break;
				case Datatype::Letter::DatetimeL:
					columns.emplace_back(columnName, Datatype::Datetime());
					break;
				default:
					throw std::exception();
			}
		}
	} while(headerLength > sizeof(uint32_t)); // > sizeof(uint32_t) because rowLength is part of header

	[[maybe_unused]] const auto rowLength = read<uint32_t>(file);

	database.reserve(rowNumber);
	for (unsigned int i = 0; i < rowNumber; ++i) {
		Row row;
		row.reserve(columns.size());

		for (const auto& [name, type] : columns) {
			switch(type.letter()) {
				case Datatype::Letter::DoubleL: {
					auto value = read<uint64_t>(file);

					row.emplace_back(name, Value{ .doubleValue = *reinterpret_cast<double *>(&value) });
				}
					break;
				case Datatype::Letter::LongL: {
					auto value = read<uint64_t>(file);

					row.emplace_back(name, Value{ .longValue = *reinterpret_cast<long *>(&value) });
				}
					break;
				case Datatype::Letter::StringL:{
					auto rawString = new uint8_t[type.size()];

					for (unsigned int j = 0; j < type.size(); ++j) {
						rawString[j] = read<uint8_t>(file);
					}

					const std::string& savedValue = strings.emplace_back(reinterpret_cast<const char*>(&rawString));

					delete [] rawString;

					row.emplace_back(name, Value{ .stringValue = std::string_view(savedValue) });

				}
					break;
				case Datatype::Letter::DatetimeL:{
					Datetime value {read<uint32_t>(file), read<uint32_t>(file), read<uint32_t>(file),
					                read<uint32_t>(file), read<uint32_t>(file), read<uint32_t>(file)};

					row.emplace_back(name, Value{ .datetimeValue = value });
				}
					break;
			}
		}

		database.emplace_back(std::move(row));
	}

	file.close();
}

db_id_t SmallDB::insertRow() {
	auto& row = database.emplace_back();
	for (const auto & [name, _] : columns) {
		row.emplace_back(name, Value {});
	}
	return database.size()-1;
}

void SmallDB::deleteRow(db_id_t id) {
	database.erase(database.cbegin() + id);
}

std::vector<db_id_t> SmallDB::find(const Query& query) const {
	return query.runOn(database);
}

double SmallDB::readDouble(db_id_t id, const std::string_view& name) const {
	return iterator(id, name).doubleValue;
}

void SmallDB::writeDouble(db_id_t id, const std::string_view& name, double value) {
	iterator(id, name).doubleValue = value;
}

long SmallDB::readLong(db_id_t id, const std::string_view& name) const {
	return iterator(id, name).longValue;
}

void SmallDB::writeLong(db_id_t id, const std::string_view& name, long value) {
	iterator(id, name).longValue = value;
}

const std::string_view& SmallDB::readString(db_id_t id, const std::string_view& name) const {
	return iterator(id, name).stringValue;
}

void SmallDB::writeString(db_id_t id, const std::string_view& name, std::string_view value) {
	const auto cmp = [name](const std::pair<const std::string&, Datatype>& p) { return p.first == name;};
	const auto position =  std::find_if(columns.begin(), columns.end(), cmp);

	if(value.size() > position->second.size()) {
		throw StringTooLong();
	}

	const std::string& savedValue = strings.emplace_back(value);
	iterator(id, name).stringValue = std::string_view(savedValue);
}

Datetime SmallDB::readDatetime(db_id_t id, const std::string_view& name) const {
	return iterator(id, name).datetimeValue;
}

void SmallDB::writeDatetime(db_id_t id, const std::string_view& name, Datetime value) {
	iterator(id, name).datetimeValue = value;
}

template <class T>
inline void SmallDB::write(std::ofstream& file, const T value) {
	file.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

void SmallDB::save(const std::string& fileName) const {
	std::ofstream file(fileName, std::ios_base::out | std::ios_base::binary);

	write<uint32_t>(file, static_cast<uint32_t>(database.size()));

	unsigned int headerLength = sizeof(uint32_t); //starts at sizeof(uint32_t) to account for row length field
	for (const auto& [name, type] : columns) {
		headerLength += name.size() + sizeof(uint8_t);
		headerLength += sizeof(uint8_t);
		if(type.letter() == Datatype::StringL) {
			headerLength += sizeof(uint32_t);
		}
	}
	write<uint32_t>(file, headerLength);

	for (const auto& [name, type] : columns) {
		file << name << '\n';
		write<uint8_t>(file, type.letter());
		if(type.letter() == Datatype::StringL) {
			write<uint32_t>(file, type.size());
		}
	}

	unsigned int rowLength = 0;
	for (const auto& [name, type] : columns) {
		rowLength += type.size();
	}
	write<uint32_t>(file, rowLength);

	for (const auto& row : database) {
		for (const auto& [name, type] : columns) {
			const auto cmp = [&name](const std::pair<std::string_view, Value> &p) { return p.first == name; };
			auto position = std::find_if(row.begin(), row.end(), cmp);
			const Value& value = position->second;
			switch (type.letter()) {
				case Datatype::DoubleL:
					write<uint64_t>(file, *reinterpret_cast<const uint64_t*>(&value.doubleValue));
					break;
				case Datatype::LongL:
					write<uint64_t>(file, *reinterpret_cast<const uint64_t*>(&value.longValue));
					break;
				case Datatype::StringL:
					for (uint8_t c : value.stringValue) {
						write<uint8_t>(file, c);
					}

					for(unsigned int i = value.stringValue.size(); i < type.size(); i++) {
						file << '\0';
					}
					break;
				case Datatype::DatetimeL:
					write<uint32_t>(file, value.datetimeValue.year);
					write<uint32_t>(file, value.datetimeValue.month);
					write<uint32_t>(file, value.datetimeValue.day);
					write<uint32_t>(file, value.datetimeValue.hour);
					write<uint32_t>(file, value.datetimeValue.minute);
					write<uint32_t>(file, value.datetimeValue.second);
					break;
			}
		}
	}

	file.close();
}