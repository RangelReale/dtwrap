#pragma once

#include <dtwrap/BaseContext.hpp>
#include <dtwrap/Type.hpp>
#include <dtwrap/Optional.hpp>
#include <dtwrap/util/Value.hpp>
#include <dtwrap/util/StackPop.hpp>
#include <dtwrap/IntrusiveRefCntPtr.h>

#include <duktape.h>

#include <string>
#include <memory>

namespace dtwrap
{

class Ref : public ThreadSafeRefCountedBase<Ref>
{
public:
	typedef IntrusiveRefCntPtr<Ref> Ptr;

	Ref(BaseContext::Ptr ctx, const std::string &description = "");
	virtual ~Ref() {}

	virtual void push() = 0;

	template <typename T>
	T is()
	{
		util::StackPop p(_ctx);
		push();
		return util::Value<T>::is(_ctx, -1);
	}

	template <typename T>
	T require()
	{
		util::StackPop p(_ctx);
		push();
		return util::Value<T>::require(_ctx, -1);
	}

	template <typename T>
	T get()
	{
		util::StackPop p(_ctx);
		push();
		return util::Value<T>::get(_ctx, -1);
	}

	template <typename T>
	T cast()
	{
		util::StackPop p(_ctx);
		push();
		return util::ValueCast<T>::cast(_ctx, -1);
	}

	Type type();
	bool isFunction();

	// prop
	bool hasProp(const std::string &name);
	bool delProp(const std::string &name);

	duk_size_t length();
	Ref::Ptr getProp(const std::string &name);
	Ref::Ptr getIndex(duk_uarridx_t index);

	Ref::Ptr getEnum();

	Ref::Ptr putProp(const std::string &name, Ref::Ptr value);
	Ref::Ptr putIndex(duk_uarridx_t index, Ref::Ptr value);

	const Ref::Ptr key() const { return _key; }
	void setKey(Ref::Ptr key) { _key = key; }

	const std::string &description() const { return _description; }

	template<typename ...Ts>
	Ref::Ptr call(Ts... args)
	{
		// push current object
		push();

		// push args
		util::PushArg::push(_ctx, args...);

		return do_call(sizeof...(Ts));
	}

	template<typename ...Ts>
	Ref::Ptr callMethod(Ref::Ptr m_this, Ts... args)
	{
		// push current object
		push();

		// push "this"
		m_this->push();

		// push args
		util::PushArg::push(_ctx, args...);

		return do_call_method(sizeof...(Ts));
	}

	template<typename ...Ts>
	Ref::Ptr callProp(const std::string &name, Ts... args)
	{
		// push current object
		push();

		// push property name
		duk_push_string(*_ctx, name.c_str());

		// push args
		util::PushArg::push(_ctx, args...);

		return do_call_prop(name, sizeof...(Ts));
	}

protected:
	Ref::Ptr do_call(int args);
	Ref::Ptr do_call_method(int args);
	Ref::Ptr do_call_prop(const std::string &name, int args);

	BaseContext::Ptr _ctx;
	Ref::Ptr _key;
	std::string _description;
};

class RefGlobal : public Ref
{
public:
	RefGlobal(BaseContext::Ptr ctx);

	void push() override;
};

class RefVal : public Ref
{
public:
	RefVal(BaseContext::Ptr ctx, duk_idx_t index, const std::string &description = "");
	~RefVal();

	void push() override;
private:
	duk_uint32_t _id;
};

class RefEnum : public Ref
{
public:
	//typedef IntrusiveRefCntPtr<RefEnum> RefEnumPtr;

	RefEnum(BaseContext::Ptr ctx, duk_idx_t index, const std::string &description = "");
	~RefEnum();

	Ref::Ptr next();

	void push() override;
private:
	duk_uint32_t _id;
};

namespace util {

	template<>
	struct Value<Ref::Ptr> {
		static void push(BaseContext::Ptr ctx, Ref::Ptr ref)
		{
			ref->push();
		}
	};

}


}