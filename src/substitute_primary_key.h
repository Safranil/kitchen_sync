#ifndef SUBSTITUTE_PRIMARY_KEY_H
#define SUBSTITUTE_PRIMARY_KEY_H

#include <algorithm>

struct ColumnNullableOrReplaced {
	ColumnNullableOrReplaced(const Table &table): table(table) {}

	bool operator()(size_t column) {
		return table.columns[column].nullable || !table.columns[column].filter_expression.empty();
	}

	const Table &table;
};

inline void choose_primary_key_for(Table &table) {
	// generally we expect most tables to have a real primary key
	if (table.primary_key_type == PrimaryKeyType::explicit_primary_key) return;

	ColumnNullableOrReplaced usable(table);

	// if not, we need to find a unique key with no nullable columns to act as a surrogate primary key
	for (const Key &key : table.keys) {
		if (key.unique() && none_of(key.columns.begin(), key.columns.end(), usable)) {
			table.primary_key_columns = key.columns;
			table.primary_key_type = PrimaryKeyType::suitable_unique_key;
			return;
		}
	}
}

#endif
