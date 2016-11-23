#include <dtwrap/util/StackPop.hpp>

namespace dtwrap { 
namespace util {

StackPop::StackPop(BaseContext::Ptr ctx, duk_idx_t amount) :
	_ctx(ctx)
{
	setAmount(amount);
}

StackPop::~StackPop()
{
	if (_amount > 0)
	{
		duk_pop_n(*_ctx, _amount);
	}
}

void StackPop::setAmount(duk_idx_t amount)
{
	if (amount >= 0)
		_amount = amount;
	else
		_amount = 0;
}

StackPop& StackPop::operator ++()
{
	setAmount(_amount + 1);
	return *this;
}

StackPop& StackPop::operator --()
{
	setAmount(_amount - 1);
	return *this;
}

StackPop& StackPop::operator +=(duk_idx_t value)
{
	setAmount(_amount + value);
	return *this;
}

StackPop& StackPop::operator -=(duk_idx_t value)
{
	setAmount(_amount - value);
	return *this;
}



} }