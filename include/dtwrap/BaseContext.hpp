#pragma once

#include <dtwrap/IntrusiveRefCntPtr.h>

#include <duktape.h>

#include <string>
#include <memory>

namespace dtwrap
{

class BaseContext : public ThreadSafeRefCountedBase<BaseContext>
{
public:
	typedef IntrusiveRefCntPtr<BaseContext> Ptr;

	virtual ~BaseContext();

	virtual duk_context *ctx() = 0;

	// implicit conversion operator
	operator duk_context*();

	void eval(const std::string &code);

	void refPush(duk_uint32_t id);
	duk_uint32_t ref(duk_idx_t index);
	void unref(duk_uint32_t id);
	duk_uint32_t refCount();
	duk_uint32_t refMax();
protected:
	BaseContext::Ptr thisPtr();
private:
	void push_ref_array();
};

}
