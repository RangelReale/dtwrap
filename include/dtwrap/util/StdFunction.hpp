#pragma once

#include <dtwrap/BaseContext.hpp>
#include <dtwrap/util/Value.hpp>

#include <tuple>
#include <type_traits>
#include <utility>
#include <functional>

namespace dtwrap {
namespace util {

/*
template <size_t ...I>
struct index_sequence {};

template <size_t N, size_t ...I>
struct make_index_sequence : public make_index_sequence<N - 1, N - 1, I...> {};

template <size_t ...I>
struct make_index_sequence<0, I...> : public index_sequence<I...> {};
*/

template<typename RetType, typename... Ts>
struct StdFuncInfoHolder
{
	typedef std::function<RetType(Ts...)> FuncType;

	struct FuncRuntime
	{
		static duk_ret_t call_native_function(duk_context *ctx)
		{
			auto bctx = make_context_base(ctx);

			duk_push_current_function(ctx);
			duk_get_prop_string(ctx, -1, "\xFF" "stdfunc_ptr");
			void* fp_void = duk_require_pointer(ctx, -1);
			if (fp_void == NULL) {
				duk_error(ctx, DUK_RET_TYPE_ERROR, "what even");
				return DUK_RET_TYPE_ERROR;
			}
			duk_pop_2(ctx);

			FuncType* funcToCall = reinterpret_cast<FuncType*>(fp_void);

			actually_call(bctx, *funcToCall, get_stack_values(bctx));

			return std::is_void<RetType>::value ? 0 : 1;
		}

		static duk_ret_t finalizer_function(duk_context *ctx)
		{
			auto bctx = make_context_base(ctx);

			duk_get_prop_string(ctx, -1, "\xFF" "stdfunc_ptr");
			void* fp_void = duk_require_pointer(ctx, -1);
			if (fp_void == NULL) {
				duk_error(ctx, DUK_RET_TYPE_ERROR, "what even");
				return DUK_RET_TYPE_ERROR;
			}
			duk_pop(ctx);

			FuncType* funcToCall = reinterpret_cast<FuncType*>(fp_void);
			delete funcToCall;

			// set pointer as null
			duk_push_pointer(ctx, NULL);
			duk_put_prop_string(ctx, -2, "\xFF" "stdfunc_ptr");

			return 0;
		}

		template<size_t... N>
		static std::tuple<Ts...> get_stack_values_helper(BaseContext::Ptr ctx, std::index_sequence<N...>)
		{
			return std::make_tuple(Value<Ts>::require(ctx, N)...);
		}

		static std::tuple<Ts...> get_stack_values(BaseContext::Ptr ctx)
		{
			constexpr std::size_t num_args = sizeof...(Ts);
			return get_stack_values_helper(ctx, typename std::make_index_sequence<num_args>{});
		}

		// this mess is to support functions with void return values
		template<typename Dummy = RetType>
		static typename std::enable_if<!std::is_void<Dummy>::value>::type actually_call(BaseContext::Ptr ctx, std::function<RetType(Ts...)> funcToCall, std::tuple<Ts...> args)
		{
			// ArgStorage has some static_asserts in it that validate value types,
			// so we typedef it to force ArgStorage<RetType> to compile and run the asserts
			//typedef typename dukglue::types::ArgStorage<RetType>::type ValidateReturnType;

			RetType return_val = apply_fp_helper(funcToCall, typename std::make_index_sequence<sizeof...(Ts)>{}, args);

			Value<RetType>::push(ctx, return_val);
		}

		template<typename Dummy = RetType>
		static typename std::enable_if<std::is_void<Dummy>::value>::type actually_call(BaseContext::Ptr ctx, std::function<RetType(Ts...)> funcToCall, std::tuple<Ts...> args)
		{
			apply_fp_helper(funcToCall, typename std::make_index_sequence<sizeof...(Ts)>{}, args);
		}

		// function pointer
		template<size_t... Indexes >
		static RetType apply_fp_helper(std::function<RetType(Ts...)> pf, std::index_sequence< Indexes... >, std::tuple<Ts...> tup)
		{
			return pf(std::forward<Ts>(std::get<Indexes>(tup))...);
		}
	};
};

template<typename RetType, typename... Ts>
struct Value<std::function<RetType(Ts...)>> {
	static void push(BaseContext::Ptr ctx, std::function<RetType(Ts...)> funcToCall)
	{
		duk_c_function evalFunc = StdFuncInfoHolder<RetType, Ts...>::FuncRuntime::call_native_function;
		duk_c_function finFunc = StdFuncInfoHolder<RetType, Ts...>::FuncRuntime::finalizer_function;

		duk_push_c_function(*ctx, evalFunc, sizeof...(Ts));

		//static_assert(sizeof(RetType(*)(Ts...)) == sizeof(void*), "Function pointer and data pointer are different sizes");
		// put <internal> func_ptr prop with function pointer

		auto nf = new std::function<RetType(Ts...)>(funcToCall);

		duk_push_pointer(*ctx, reinterpret_cast<void*>(nf));
		duk_put_prop_string(*ctx, -2, "\xFF" "stdfunc_ptr");

		duk_push_c_function(*ctx, finFunc, 1);
		duk_set_finalizer(*ctx, -2);
	}
};

} }
