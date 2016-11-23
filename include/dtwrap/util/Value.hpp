#pragma once

#include <dtwrap/BaseContext.hpp>
#include <dtwrap/Exception.hpp>
#include <dtwrap/Type.hpp>
#include <dtwrap/util/Function.hpp>

#include <cstdint>

namespace dtwrap {
namespace util {

//
// Value
//

template <typename T>
struct Value
{
	static bool is(BaseContext::Ptr ctx, duk_idx_t index);
	static T require(BaseContext::Ptr ctx, duk_idx_t index);
	static T get(BaseContext::Ptr ctx, duk_idx_t index);
	static T to(BaseContext::Ptr ctx, duk_idx_t index);
	static void push(BaseContext::Ptr ctx, const T& value);
};

template<>
struct Value<bool> 
{
	static bool is(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_is_boolean(*ctx, index) != 0); }
	static bool require(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_require_boolean(*ctx, index) != 0); }
	static bool get(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_get_boolean(*ctx, index) != 0); }
	static bool to(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_to_boolean(*ctx, index) != 0); }
	static void push(BaseContext::Ptr ctx, const bool& value) { duk_push_boolean(*ctx, value); }
};

template<>
struct Value<const char *> {
	static bool is(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_is_string(*ctx, index) != 0); }
	static const char* require(BaseContext::Ptr ctx, duk_idx_t index) { return duk_require_string(*ctx, index); }
	static const char* get(BaseContext::Ptr ctx, duk_idx_t index) { return duk_get_string(*ctx, index); }
	static const char* to(BaseContext::Ptr ctx, duk_idx_t index) { return duk_safe_to_string(*ctx, index); }
	static void push(BaseContext::Ptr ctx, const char* const& value) { duk_push_string(*ctx, value); }
};

template<>
struct Value<uint8_t> {
	static bool is(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(*ctx, index) != 0); }
	static uint8_t require(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<uint8_t>(duk_require_uint(*ctx, index)); }
	static uint8_t get(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<uint8_t>(duk_get_uint(*ctx, index)); }
	static uint8_t to(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<uint8_t>(duk_to_uint(*ctx, index)); }
	static void push(BaseContext::Ptr ctx, const uint8_t& value) { duk_push_uint(*ctx, value); }
};

template<>
struct Value<uint16_t> {
	static bool is(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(*ctx, index) != 0); }
	static uint16_t require(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<uint16_t>(duk_require_uint(*ctx, index)); }
	static uint16_t get(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<uint16_t>(duk_get_uint(*ctx, index)); }
	static uint16_t to(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<uint16_t>(duk_to_uint(*ctx, index)); }
	static void push(BaseContext::Ptr ctx, const uint16_t& value) { duk_push_uint(*ctx, value); }
};

template<>
struct Value<uint32_t> {
	static bool is(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(*ctx, index) != 0); }
	static uint32_t require(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<uint32_t>(duk_require_uint(*ctx, index)); }
	static uint32_t get(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<uint32_t>(duk_get_uint(*ctx, index)); }
	static uint32_t to(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<uint32_t>(duk_to_uint(*ctx, index)); }
	static void push(BaseContext::Ptr ctx, const uint32_t& value) { duk_push_uint(*ctx, value); }
};

/*
template<>
struct Value<uint64_t> {
	static bool is(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(*ctx, index) != 0); }
	static uint64_t require(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<uint64_t>(duk_require_uint(*ctx, index)); }
	static uint64_t get(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<uint64_t>(duk_get_uint(*ctx, index)); }
	static uint64_t to(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<uint64_t>(duk_to_uint(*ctx, index)); }
	static void push(BaseContext::Ptr ctx, const uint64_t& value) { duk_push_uint(*ctx, value); }
};
*/

template<>
struct Value<int8_t> {
	static bool is(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(*ctx, index) != 0); }
	static int8_t require(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<int8_t>(duk_require_int(*ctx, index)); }
	static int8_t get(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<int8_t>(duk_get_int(*ctx, index)); }
	static int8_t to(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<int8_t>(duk_to_int(*ctx, index)); }
	static void push(BaseContext::Ptr ctx, const int8_t& value) { duk_push_int(*ctx, value); }
};

template<>
struct Value<int16_t> {
	static bool is(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(*ctx, index) != 0); }
	static int16_t require(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<int16_t>(duk_require_int(*ctx, index)); }
	static int16_t get(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<int16_t>(duk_get_int(*ctx, index)); }
	static int16_t to(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<int16_t>(duk_to_int(*ctx, index)); }
	static void push(BaseContext::Ptr ctx, const int16_t& value) { duk_push_int(*ctx, value); }
};

template<>
struct Value<int32_t> {
	static bool is(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(*ctx, index) != 0); }
	static int32_t require(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<int32_t>(duk_require_int(*ctx, index)); }
	static int32_t get(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<int32_t>(duk_get_int(*ctx, index)); }
	static int32_t to(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<int32_t>(duk_to_int(*ctx, index)); }
	static void push(BaseContext::Ptr ctx, const int32_t& value) { duk_push_int(*ctx, value); }
};

/*
template<>
struct Value<int64_t> {
	static bool is(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(*ctx, index) != 0); }
	static int64_t require(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<int64_t>(duk_require_int(*ctx, index)); }
	static int64_t get(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<int64_t>(duk_get_int(*ctx, index)); }
	static int64_t to(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<int64_t>(duk_to_int(*ctx, index)); }
	static void push(BaseContext::Ptr ctx, const int64_t& value) { duk_push_int(*ctx, value); }
};
*/

template<>
struct Value<float> {
	static bool is(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(*ctx, index) != 0); }
	static float require(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<float>(duk_require_number(*ctx, index)); }
	static float get(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<float>(duk_get_number(*ctx, index)); }
	static float to(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<float>(duk_to_number(*ctx, index)); }
	static void push(BaseContext::Ptr ctx, const float& value) { duk_push_number(*ctx, value); }
};

template<>
struct Value<double> {
	static bool is(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_is_number(*ctx, index) != 0); }
	static double require(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<double>(duk_require_number(*ctx, index)); }
	static double get(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<double>(duk_get_number(*ctx, index)); }
	static double to(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<double>(duk_to_number(*ctx, index)); }
	static void push(BaseContext::Ptr ctx, const double& value) { duk_push_number(*ctx, value); }
};

/*
template<>
struct Value<void*> {
static bool is(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_is_pointer(*ctx, index) != 0); }
static void* require(BaseContext::Ptr ctx, duk_idx_t index) { return (duk_require_pointer(*ctx, index)); }
static void* get(BaseContext::Ptr ctx, duk_idx_t index) { return (duk_get_pointer(*ctx, index)); }
static void to(BaseContext::Ptr ctx, duk_idx_t index) { (duk_to_pointer(*ctx, index)); }
static void push(BaseContext::Ptr ctx, const (void*)& value) { duk_push_pointer(*ctx, value); }
};
*/

template<>
struct Value<std::string> {
	static bool is(BaseContext::Ptr ctx, duk_idx_t index) { return static_cast<bool>(duk_is_string(*ctx, index) != 0); }
	static std::string require(BaseContext::Ptr ctx, duk_idx_t index) { return std::string(duk_require_string(*ctx, index)); }
	static std::string get(BaseContext::Ptr ctx, duk_idx_t index) { return std::string(duk_get_string(*ctx, index)); }
	static std::string to(BaseContext::Ptr ctx, duk_idx_t index) { return std::string(duk_safe_to_string(*ctx, index)); }
	static void push(BaseContext::Ptr ctx, const std::string& value) { duk_push_string(*ctx, value.c_str()); }
};


template<>
struct Value<Type> {
	static void push(BaseContext::Ptr ctx, const Type& value) 
	{ 
		switch (value)
		{
		case Type::UNDEFINED: duk_push_undefined(*ctx); break;
		case Type::TNULL: duk_push_null(*ctx); break;
		case Type::OBJECT: duk_push_object(*ctx); break;
		case Type::PUSH_ARRAY: duk_push_array(*ctx); break;
		default:
			throw Exception("Cannot push primite object by type");
		}
	}
};


template<>
struct Value<duk_c_function> {
	static void push(BaseContext::Ptr ctx, duk_c_function func) { duk_push_c_function(*ctx, func, DUK_VARARGS); }
};

template<typename RetType, typename... Ts>
struct Value<RetType(*)(Ts...)> {
	static void push(BaseContext::Ptr ctx, RetType(*funcToCall)(Ts...))
	{
		duk_c_function evalFunc = FuncInfoHolder<RetType, Ts...>::FuncRuntime::call_native_function;

		duk_push_c_function(*ctx, evalFunc, sizeof...(Ts));

		//static_assert(sizeof(RetType(*)(Ts...)) == sizeof(void*), "Function pointer and data pointer are different sizes");
		// put <internal> func_ptr prop with function pointer
		duk_push_pointer(*ctx, reinterpret_cast<void*>(funcToCall));
		duk_put_prop_string(*ctx, -2, "\xFF" "func_ptr");
	}
};

//
// ValueCast
//

template <typename T>
struct ValueCast {
	static T cast(BaseContext::Ptr ctx, duk_idx_t index)
	{
		static_assert(!std::is_same<T, const char*>::value, "Cannot cast to const char *, cast to std::string");

		duk_dup(*ctx, index);
		auto ret = Value<T>::to(ctx, -1);
		duk_pop(*ctx);
		return ret;
	}
};

//
// PushArg
//


struct PushArg {
	template <typename T, typename... Ts>
	static void push(BaseContext::Ptr ctx, T arg, Ts... args)
	{
		Value<T>::push(ctx, arg);
		PushArg::push(ctx, std::forward<Ts>(args)...);
	}

	static void push(BaseContext::Ptr ctx)
	{
		// template termination
	}
};



} }