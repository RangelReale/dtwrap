#pragma once

#include <dtwrap/BaseContext.hpp>

#include <duktape.h>

namespace dtwrap { 
namespace util {

class StackPop
{
public:
	StackPop(BaseContext::Ptr ctx, duk_idx_t amount = 1);
	~StackPop();

	void setAmount(duk_idx_t amount);

	StackPop& operator ++();
	StackPop& operator --();
	StackPop& operator +=(duk_idx_t value);
	StackPop& operator -=(duk_idx_t value);
private:
	BaseContext::Ptr _ctx;
	duk_idx_t _amount;
};

} }