#ifndef LEGACY_SCHEMA_SERIALIZATION
#define LEGACY_SCHEMA_SERIALIZATION

template <typename OutputStream>
void legacy_serialize(Packer<OutputStream> &packer, const Column &column) {
	int fields = 2;
	if (column.size) fields++;
	if (column.scale) fields++;
	if (!column.nullable) fields++;
	if (!column.db_type_def.empty()) fields++;
	if (column.default_type != DefaultType::no_default) fields++;
	if (column.flags.mysql_timestamp) fields++;
	if (column.flags.mysql_on_update_timestamp) fields++;
	if (column.flags.time_zone) fields++;
	pack_map_length(packer, fields);
	packer << string("name");
	packer << column.name;
	packer << string("column_type");
	packer << column.column_type;
	if (column.size) {
		packer << string("size");
		packer << column.size;
	}
	if (column.scale) {
		packer << string("scale");
		packer << column.scale;
	}
	if (!column.nullable) {
		packer << string("nullable");
		packer << column.nullable;
	}
	if (!column.db_type_def.empty()) {
		packer << string("db_type_def");
		packer << column.db_type_def;
	}
	switch (column.default_type) {
		case DefaultType::no_default:
			break;

		case DefaultType::sequence:
			packer << string("sequence");
			packer << column.default_value; // currently unused, but allowed for forward compatibility
			break;

		case DefaultType::default_value:
			packer << string("default_value");
			packer << column.default_value;
			break;

		case DefaultType::default_expression:
			packer << string("default_function");
			packer << column.default_value;
			break;
	}
	if (column.flags.mysql_timestamp) {
		packer << string("mysql_timestamp");
		packer << true;
	}
	if (column.flags.mysql_on_update_timestamp) {
		packer << string("mysql_on_update_timestamp");
		packer << true;
	}
	if (column.flags.time_zone) {
		packer << string("time_zone");
		packer << true;
	}
}

template <typename VersionedFDWriteStream>
void legacy_serialize(Packer<VersionedFDWriteStream> &packer, const Key &key) {
	pack_map_length(packer, 3);
	packer << string("name");
	packer << key.name;
	packer << string("unique");
	packer << key.unique();
	packer << string("columns");
	packer << key.columns;
}

#endif
