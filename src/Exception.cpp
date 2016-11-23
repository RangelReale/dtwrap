#include <dtwrap/Exception.hpp>

namespace dtwrap
{

Exception::Exception(const std::string &message) : 
	_what(message) 
{
}

char const* Exception::what() const throw ()
{
	return _what.c_str();
}

}