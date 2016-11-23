#include <dtwrap/Context.hpp>

namespace dtwrap
{

Context::Context() : 
	BaseContext(), _ctx(duk_create_heap_default()), _owned(true)
{

}

Context::Context(duk_context *ctx) : 
	BaseContext(), _ctx(ctx), _owned(false)
{

}

Context::~Context()
{
	if (_owned)
	{
		duk_destroy_heap(_ctx);
	}
}

duk_context *Context::ctx()
{
	return _ctx;
}

Ref::Ptr Context::global()
{
	return Ref::Ptr(new RefGlobal(this));
}


}
