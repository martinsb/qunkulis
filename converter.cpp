#include "converter.h"

namespace UnqliteQt
{

QVariant ToVariantConverter::convert(const unqlite_value*& pVal)
{
	if (unqlite_value_is_bool(pVal)) {
		int val = unqlite_value_to_bool(pVal);
		return QVariant(val != 0 ? true : false);
	}
	else if (unqlite_value_is_numeric(pVal)) {
		return QVariant(unqlite_value_to_int(pVal));
	}
	else if (unqlite_value_is_string(pVal)) {
		int nLen;
		const char* zVal = unqlite_value_to_string(pVal, &nLen);
		return QVariant(QString::fromUtf8(zVal, nLen));
	}
	else if (unqlite_value_is_json_array(pVal)) {
		unqlite_array_walk(pVal, walk_callback, this);			
	}
	return QVariant();
}

int ToVariantConverter::walk_callback(unqlite_value* pKey, unqlite_value* pValue, void *pUserData)
{
	ToVariantConverter* self = reinterpret_cast<ToVariantConverter*>(pUserData);
	
	return UNQLITE_OK;
}

}