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
	LIGHTFUNC
};

std::string typeToString(Type t);

std::ostream& operator<< (std::ostream &os, Type t);


}