#pragma once

#include <dtwrap/BaseContext.hpp>
#include <dtwrap/Ref.hpp>

namespace dtwrap
{

template<class T>
class ClassInstance
{
public:
	ClassInstance(T* value) :
		_value(value)
	{

	}

	~ClassInstance()
	{
		delete _value;
	}

	virtual void registerObject(BaseContext::Ptr ctx, Ref::Ptr obj) = 0;

	T* getValue() { return _value; }
protected:
	T* _value;
};

namespace util {

	template<class X>
	struct Value<ClassInstance<X>> {
		static void push(BaseContext::Ptr ctx, ClassInstance<X>* value)
		{
			Ref::Ptr obj(createRef(ctx, Type::OBJECT));

			obj->push();

			// put classinstance in object's ciobj_ptr
			duk_push_pointer(*ctx, reinterpret_cast<void*>(value));
			duk_put_prop_string(*ctx, -2, "\xFF" "ciobj_ptr");

			// put instance in object's obj_ptr
			duk_push_pointer(*ctx, reinterpret_cast<void*>(value->getValue()));
			duk_put_prop_string(*ctx, -2, "\xFF" "obj_ptr");

			// register object
			value->registerObject(ctx, obj);

			// push finalizer
			duk_push_c_function(*ctx, destruct, 1);
			duk_set_finalizer(*ctx, -2);

			// leave pushed object on the stack
		}

		static duk_ret_t destruct(duk_context *ctx)
		{
			duk_get_prop_string(ctx, -1, "\xFF" "ciobj_ptr");
			void* ciop_void = duk_require_pointer(ctx, -1);
			if (ciop_void == NULL) {
				duk_error(ctx, DUK_RET_TYPE_ERROR, "what even");
				return DUK_RET_TYPE_ERROR;
			}
			duk_pop(ctx);

			// set pointer as null
			duk_push_pointer(ctx, NULL);
			duk_put_prop_string(ctx, -2, "\xFF" "obj_ptr");


			// set ci pointer as null
			duk_push_pointer(ctx, NULL);
			duk_put_prop_string(ctx, -2, "\xFF" "ciobj_ptr");
			
			// delete instance
			ClassInstance<X>* cv = reinterpret_cast<ClassInstance<X>*>(ciop_void);
			delete cv;

			return 0;
		}

	};
}

#define CLASSINSTANCE_REGISTER(CLASS, CONTAINER)												\
namespace dtwrap {																				\
namespace util {																				\
	template<>																					\
	struct Value<CLASS*> {																		\
		static void push(BaseContext::Ptr ctx, CLASS* value)									\
		{																						\
			Value<ClassInstance<CLASS>>::push(ctx, new CONTAINER(value));						\
		}																						\
	};																							\
}}


}