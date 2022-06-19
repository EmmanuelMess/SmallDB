#include "Query.hpp"

#include <utility>

Query::Query(Predicate predicate) : predicate(std::move(predicate)) {}

std::vector<db_id_t> Query::runOn(const Database & database) const {
	std::vector<db_id_t> result;
	for(size_t i = 0; i < database.size(); i++) {
		if(predicate(database[i])) {
			result.push_back(i);
		}
	}
	return result;
}