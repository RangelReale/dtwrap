#include <dtwrap/Ref.hpp>
#include <dtwrap/util/StackPop.hpp>
#include <dtwrap/util/Util.hpp>
#include <dtwrap/Exception.hpp>

#include <iostream>
#include <sstream>

namespace dtwrap
{

Ref::Ref(BaseContext::Ptr ctx, const std::string &description) :
	_ctx(ctx), _description(description)
{

}

Type Ref::type()
{
	util::StackPop p(_ctx);
	push();
	return util::Util::getType(duk_get_type(*_ctx, -1));
}

bool Ref::isFunction()
{
	util::StackPop p(_ctx);
	push();
	return duk_is_function(*_ctx, -1) != 0;
}

duk_size_t Ref::length()
{
	util::StackPop p(_ctx);
	push();
	return duk_get_length(*_ctx, -1);
}


Ref::Ptr Ref::getProp(const std::string &name)
{
	util::StackPop p(_ctx); // ref

	// push current object
	push();

	// check if type os object coercible
	if (!duk_is_object_coercible(*_ctx, -1)) 
	{
		std::stringstream smsg;
		smsg << "Can't get property '" << name << "' of '" << type() << "' (" << _description << ")";
		throw Exception(smsg.str());
	}

	// get request prop
	duk_get_prop_string(*_ctx, -1, name.c_str());
	// pop prop
	++p; 

	// build description
	std::string desc(_description);
	if (!desc.empty()) desc.append(".");
	desc.append(name);

	// return reference
	return Ref::Ptr(new RefVal(_ctx, -1, desc));
}

Ref::Ptr Ref::getIndex(duk_uarridx_t index)
{
	util::StackPop p(_ctx); // ref

	// push current object
	push();

	// check if type os object coercible
	if (!duk_is_object_coercible(*_ctx, -1))
	{
		std::stringstream smsg;
		smsg << "Can't get index '" << index << "' of '" << type() << "' (" << _description << ")";
		throw Exception(smsg.str());
	}

	// get request prop
	duk_get_prop_index(*_ctx, -1, index);
	// pop prop
	++p;

	// build description
	std::stringstream desc;
	desc << _description;
	if (!_description.empty()) desc << ".";
	desc << index;

	// return reference
	return Ref::Ptr(new RefVal(_ctx, -1, desc.str()));
}

Ref::Ptr Ref::getEnum()
{
	util::StackPop p(_ctx);
	push();
	return Ref::Ptr(new RefEnum(_ctx, -1, _description));
}


Ref::Ptr Ref::do_call(int args)
{
	// function and args are already pushed

	util::StackPop p(_ctx); // retval|error

	// call - always pops the function itself
	if (duk_pcall(*_ctx, args) == DUK_EXEC_ERROR)
	{
		std::string emsg(duk_safe_to_string(*_ctx, -1));
		throw Exception(emsg);
	}

	// build description
	std::stringstream desc;
	desc << _description;
	desc << "()";

	return Ref::Ptr(new RefVal(_ctx, -1, desc.str()));
}

Ref::Ptr Ref::do_call_method(int args)
{
	// function, this,  and args are already pushed

	util::StackPop p(_ctx); // retval|error

	// call - always pops the function itself
	if (duk_pcall_method(*_ctx, args) == DUK_EXEC_ERROR)
	{
		std::string emsg(duk_safe_to_string(*_ctx, -1));
		throw Exception(emsg);
	}

	// build description
	std::stringstream desc;
	desc << _description;
	desc << "()";

	return Ref::Ptr(new RefVal(_ctx, -1, desc.str()));
}

Ref::Ptr Ref::do_call_prop(const std::string &name, int args)
{
	// object, propname and args are already pushed

	util::StackPop p(_ctx, 2); // ref, retval|error

	// call - always pops the function itself
	if (duk_pcall_prop(*_ctx, (-1*args)-2, args) == DUK_EXEC_ERROR)
	{
		std::string emsg(duk_safe_to_string(*_ctx, -1));
		throw Exception(emsg);
	}

	// build description
	std::stringstream desc;
	desc << _description << "." << name << "#()";

	return Ref::Ptr(new RefVal(_ctx, -1, desc.str()));
}

//
// RefGlobal
//

RefGlobal::RefGlobal(BaseContext::Ptr ctx) :
	Ref(ctx, "[global]")
{

}

void RefGlobal::push()
{
	duk_push_global_object(*_ctx);
}

//
// RefVal
//

RefVal::RefVal(BaseContext::Ptr ctx, duk_idx_t index, const std::string &description) :
	Ref(ctx, description)
{
	_id = _ctx->ref(index);
}

RefVal::~RefVal()
{
	_ctx->unref(_id);
}

void RefVal::push()
{
	_ctx->refPush(_id);
}

//
// RefEnum
//

RefEnum::RefEnum(BaseContext::Ptr ctx, duk_idx_t index, const std::string &description)
	: Ref(ctx, description)
{
	duk_enum(*ctx, index, 0);
	_id = _ctx->ref(index);
	duk_pop(*ctx); // pop enum
}

RefEnum::~RefEnum()
{
	_ctx->unref(_id);
}

void RefEnum::push()
{
	_ctx->refPush(_id);
}

Ref::Ptr RefEnum::next()
{
	util::StackPop p(_ctx); // pop enum

	push();
	if (!duk_next(*_ctx, -1, 1))
		return Ref::Ptr();

	p += 2; // pop key, value

	Ref::Ptr ret(new RefVal(_ctx, -1));
	ret->setKey(Ref::Ptr(new RefVal(_ctx, -2)));
	return ret;
}


}