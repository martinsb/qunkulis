#ifndef _CONVERTER_H
#define _CONVERTER_H

extern "C" 
{
#include "unqlite-db/unqlite.h"
}

#include <QVariant>

namespace UnqliteQt
{

template <class From, class To>
	class Converter 
{
public:
	virtual To convert(const From& source) = 0;
};

class ToVariantConverter : public Converter<unqlite_value*, QVariant>
{
public:
	virtual QVariant convert(const unqlite_value*& pVal);

private:
	static int walk_callback(unqlite_value* pKey, unqlite_value* pValue, void *pUserData);
};

}

#endif //_CONVERTER_H
