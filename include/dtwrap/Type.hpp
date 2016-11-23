#pragma once

#include <iostream>

namespace dtwrap
{

enum class Type
{
	UNKNOWN,
	NONE,
	UNDEFINED,
	TNULL,
	BOOLEAN,
	NUMBER,
	STRING,
	OBJECT,
	BUFFER,
	POINTER,
	LIGHTFUNC,

	PUSH_ARRAY
};

std::string typeToString(Type t);

std::ostream& operator<< (std::ostream &os, Type t);


}