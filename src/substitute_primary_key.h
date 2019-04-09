#ifndef SUBSTITUTE_PRIMARY_KEY_H
#define SUBSTITUTE_PRIMARY_KEY_H

#include <set>

inline bool any_column_nullable(const Table &table, const ColumnIndices &columns) {
	for (size_t column : columns) {
		if (table.columns[column].nullable) return true;
	}
	return false;
}

inline void choose_primary_key_for(Table &table) {
	// generally we expect most tables to have a real primary key
	if (table.primary_key_type == explicit_primary_key) return;

	// if not, we want to find a unique key with no nullable columns to act as a surrogate primary key
	for (const Key &key : table.keys) {
		if (key.unique && !any_column_nullable(table, key.columns)) {
			table.primary_key_columns = key.columns;
			table.primary_key_type = suitable_unique_key;
			return;
		}
	}

	// if there's no unique key usable as a pseudo-primary key, we can try to treat the whole row as
	// if it were the primary key and group and count to spot duplicates.
	for (const Column &column : table.columns) {
		// that's only possible if there are no nullable columns, though; otherwise we can't query
		// based on key ranges, since the comparison operators like > and <= will return NULL for any
		// comparisons involving NULL values
		if (column.nullable) return;
	}

	// ok, no nullable columns, so we can use the whole row as its own primary key.  but tables like
	// that are potentially very slow to query because the database may not have any good way to sort
	// the rows, and we can't assume that it will happen to serve them up in the same order at both
	// ends; look for an index with all the columns in it.
	for (const Key &key : table.keys) {
		if (key.columns.size() == table.columns.size()) {
			table.primary_key_columns = key.columns;
			table.primary_key_type = entire_row_as_key;
			return;
		}
	}
}

#endif
