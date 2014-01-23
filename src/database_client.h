#ifndef DATABASE_CLIENT_H
#define DATABASE_CLIENT_H

#include <map>
#include "schema.h"

class DatabaseClient {
public:
	inline const Database &database_schema() { return database; }
	inline const Table &table_by_name(const string &table_name) { return *tables_by_name.at(table_name); } // throws out_of_range if not present in the map

	string retrieve_rows_sql(const Table &table, const ColumnValues &prev_key, size_t row_count, char quote_identifiers_with = 0);
	string retrieve_rows_sql(const Table &table, const ColumnValues &prev_key, const ColumnValues &last_key, char quote_identifiers_with = 0);

protected:
	void index_database_tables();

protected:
	Database database;
	map<string, Tables::const_iterator> tables_by_name;
};

#endif
