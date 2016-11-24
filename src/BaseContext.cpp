#include <dtwrap/BaseContext.hpp>
#include <dtwrap/Exception.hpp>

#include <iostream>

namespace dtwrap
{

BaseContext::~BaseContext()
{

}

BaseContext::operator duk_context*()
{
	return ctx();
}

void BaseContext::eval(const std::string &code)
{
	if (duk_peval_string(ctx(), code.c_str()) != 0)
	{
		std::string emsg(duk_safe_to_string(ctx(), -1));
		duk_pop(ctx());
		throw Exception(emsg);
	}
	duk_pop(ctx());
}

void BaseContext::refPush(duk_uint32_t id)
{
	push_ref_array();

	// push from ref array
	duk_get_prop_index(*this, -1, id);

	// pop ref array
	duk_remove(*this, -2); 
}

duk_uint32_t BaseContext::ref(duk_idx_t index)
{
	duk_idx_t ri = duk_normalize_index(*this, index);

	push_ref_array();

	// find next id from array[0]
	duk_get_prop_index(*this, -1, 0);
	duk_uint32_t ret = duk_get_uint(*this, -1);
	duk_pop(*this); // pop id

	// find current amount from array[1]
	duk_get_prop_index(*this, -1, 1);
	duk_uint32_t amount = duk_get_uint(*this, -1);
	duk_pop(*this); // pop id

	// increment next id in array[0]
	duk_push_uint(*this, ret + 1);
	duk_put_prop_index(*this, -2, 0);

	// increment amount in array[1]
	duk_push_uint(*this, amount + 1);
	duk_put_prop_index(*this, -2, 1);

	// dup index to ref id
	duk_dup(*this, ri);
	// put in ref array
	duk_put_prop_index(*this, -2, ret);

	// pop ref array
	duk_pop(*this);


	//std::cout << "$$ REF " << ret << " [" << refCount() << "]" << std::endl;

	return ret;
}

void BaseContext::unref(duk_uint32_t id)
{
	push_ref_array();

	// find current amount from array[1]
	duk_get_prop_index(*this, -1, 1);
	duk_uint32_t amount = duk_get_uint(*this, -1);
	duk_pop(*this); // pop id

	// decrement amount
	duk_push_uint(*this, amount - 1);
	duk_put_prop_index(*this, -2, 1);

	// put undefined in the array position
	duk_push_undefined(*this);
	duk_put_prop_index(*this, -2, id);

	// pop ref array
	duk_pop(*this);

	//std::cout << "## UNREF " << id << " [" << refCount() << "]" << std::endl;
}

duk_uint32_t BaseContext::refCount()
{
	push_ref_array();

	// find current amount from array[1]
	duk_get_prop_index(*this, -1, 1);
	duk_uint32_t amount = duk_get_uint(*this, -1);
	duk_pop(*this); // pop id

	// pop ref array
	duk_pop(*this);

	return amount;
}

duk_uint32_t BaseContext::refMax()
{
	push_ref_array();

	// find current max from array[0]
	duk_get_prop_index(*this, -1, 0);
	duk_uint32_t max = duk_get_uint(*this, -1);
	duk_pop(*this); // pop id

	// pop ref array
	duk_pop(*this);

	return max-2; // 0 and 1 are special
}

void BaseContext::push_ref_array()
{
	static const char* DTWRAP_REF = "dtwrap_ref";

	duk_push_heap_stash(*this);

	if (!duk_has_prop_string(*this, -1, DTWRAP_REF))
	{
		// put array in dtwrap_ref
		duk_push_array(*this);

		// index 0 will be the next id counter (starts at 2)
		duk_push_int(*this, 2);
		duk_put_prop_index(*this, -2, 0);

		// index 1 will be the amount of valid ids
		duk_push_int(*this, 0);
		duk_put_prop_index(*this, -2, 1);

		// add array to root heap stash
		duk_put_prop_string(*this, -2, DTWRAP_REF);
	}

	// get the dtwrap_ref property
	duk_get_prop_string(*this, -1, DTWRAP_REF);

	// remove heap stash
	duk_remove(*this, -2);
}

BaseContext::Ptr BaseContext::thisPtr()
{
	return BaseContext::Ptr(this);
}

}

