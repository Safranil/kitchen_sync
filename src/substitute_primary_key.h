#ifndef SUBSTITUTE_PRIMARY_KEY_H
#define SUBSTITUTE_PRIMARY_KEY_H

#include <set>

inline bool any_column_nullable(const Table &table) {
	for (const Column &column : table.columns) {
		if (column.nullable) return true;
	}
	return false;
}

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

	// if there's no usable key, we want to treat the whole row as if it were the primary key (and group and count to
	// spot duplicates).  that's only possible if there are no nullable columns, though; otherwise we can't query based
	// on key ranges, since the comparison operators like > and <= will return NULL for any comparisons involving NULL
	// values, so we can't query based on even the entire row values for anything other than a point (equality) comparison.
	table.primary_key_type = any_column_nullable(table) ? no_available_key : entire_row_as_key;

	// tables with no explicit or suitable substitute primary key are potentially very slow to query because the database
	// may not have any good way to sort the rows, and we can't assume that it will happen to serve them up in the same
	// order at both ends; try to find an index with all the columns in it, and if found use that order; take the longest
	// index available if none covers all columns.
	for (const Key &key : table.keys) {
		if (key.columns.size() > table.primary_key_columns.size()) {
			table.primary_key_columns = key.columns;
		}
	}

	// if no key was found, just use the columns in the order that they are - and accept that the database is going to run
	// some awfully slow queries.  if a partial key was found, add on any missing columns.
	if (table.primary_key_columns.size() < table.columns.size()) {
		set<size_t> columns_in_key(table.primary_key_columns.begin(), table.primary_key_columns.end());

		for (size_t column = 0; column < table.columns.size(); ++column) {
			if (!columns_in_key.count(column)) {
				table.primary_key_columns.push_back(column);
			}
		}
	}
}

#endif
