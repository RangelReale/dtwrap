#include <dtwrap/util/Util.hpp>

namespace dtwrap {
namespace util {

Type Util::getType(duk_int_t duktype)
{
	switch (duktype)
	{
	case DUK_TYPE_NONE: return Type::NONE;
	case DUK_TYPE_UNDEFINED: return Type::UNDEFINED;
	case DUK_TYPE_NULL: return Type::TNULL;
	case DUK_TYPE_BOOLEAN: return Type::BOOLEAN;
	case DUK_TYPE_NUMBER: return Type::NUMBER;
	case DUK_TYPE_STRING: return Type::STRING;
	case DUK_TYPE_OBJECT: return Type::OBJECT;
	case DUK_TYPE_BUFFER: return Type::BUFFER;
	case DUK_TYPE_POINTER: return Type::POINTER;
	case DUK_TYPE_LIGHTFUNC: return Type::LIGHTFUNC;
	}
	return Type::UNKNOWN;
}


} }