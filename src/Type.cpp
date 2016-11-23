#include <dtwrap/Type.hpp>

#include <sstream>

namespace dtwrap
{

std::string typeToString(Type t)
{
	std::stringstream r;
	r << t;
	return r.str();
}

std::ostream& operator<< (std::ostream &os, Type t)
{
	switch (t)
	{
	case Type::UNKNOWN: return os << "UNKNOWN";
	case Type::NONE: return os << "NONE";
	case Type::UNDEFINED: return os << "UNDEFINED";
	case Type::TNULL: return os << "NULL";
	case Type::BOOLEAN: return os << "BOOLEAN";
	case Type::NUMBER: return os << "NUMBER";
	case Type::STRING: return os << "STRING";
	case Type::OBJECT: return os << "OBJECT";
	case Type::BUFFER: return os << "BUFFER";
	case Type::POINTER: return os << "POINTER";
	case Type::LIGHTFUNC: return os << "LIGHTFUNC";

	case Type::PUSH_ARRAY: return os << "PUSH_ARRAY";
	}
	return os << static_cast<std::uint16_t>(t);
}


}