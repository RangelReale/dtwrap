#pragma once

#include <dtwrap/Type.hpp>

#include <duktape.h>

namespace dtwrap {
namespace util {

class Util
{
public:
	static Type getType(duk_int_t duktype);
};

} }