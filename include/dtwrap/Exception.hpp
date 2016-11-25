#pragma once

#include <stdexcept>

namespace dtwrap
{

class Exception : public std::exception
{
public:
	Exception(const std::string &message);
	~Exception() throw() {}

	char const* what() const throw ();
private:
	std::string _what;
};


}