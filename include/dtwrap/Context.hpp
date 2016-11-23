#pragma once

#include <dtwrap/BaseContext.hpp>
#include <dtwrap/Ref.hpp>
#include <dtwrap/IntrusiveRefCntPtr.h>

#include <duktape.h>

#include <memory>

namespace dtwrap
{

class Context : public BaseContext
{
public:
	typedef IntrusiveRefCntPtr<Context> Ptr;

	Context();
	Context(duk_context *ctx);
	virtual ~Context();

	duk_context *ctx() override;

	Ref::Ptr global();
private:
	duk_context *_ctx;
	bool _owned;
};

template <class ...Ts>
Context::Ptr make_context(Ts... args) {
	return Context::Ptr(new Context(std::forward<Ts>(args)...));
}

}
