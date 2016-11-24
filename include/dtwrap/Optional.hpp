#pragma once

#include <dtwrap/util/Value.hpp>

namespace dtwrap
{

template<class T>
class Optional
{
public:
	Optional() : _isvalue(false) {}
	Optional(T value) : _isvalue(true), _value(value) {}

	bool isValue() const { return _isvalue; }
	T val() { return _value; }

	explicit operator bool() const
	{
		return isValue();
	}

	/*
	operator T()
	{
		return getValue();
	}
	*/
private:
	bool _isvalue;
	T _value;
};

namespace util {

template<class T>
struct Value<Optional<T>> {

	static Optional<T> require(BaseContext::Ptr ctx, duk_idx_t index)
	{ 
		if (duk_is_undefined(*ctx, index))
			return Optional<T>();
		return Optional<T>(Value<T>::require(ctx, index));
	}

	static void push(BaseContext::Ptr ctx, Optional<T> value)
	{
		if (value)
			value->push();
		else
			duk_push_undefined(*ctx);
	}
};

}


}