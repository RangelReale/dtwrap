#pragma once

#include <dtwrap/BaseContext.hpp>
#include <dtwrap/Ref.hpp>

#include <type_traits>

namespace dtwrap
{

template <class T>
struct ClassInstance
{
	typedef std::true_type dtClassInstance;
};

}