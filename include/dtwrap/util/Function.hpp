#pragma once

#include <dtwrap/BaseContext.hpp>
#include <dtwrap/util/Value.hpp>

#include <tuple>
#include <type_traits>
#include <utility>

namespace dtwrap {
namespace util {

// http://www.preney.ca/paul/archives/486
// https://www.murrayc.com/permalink/2015/12/05/modern-c-variadic-template-parameters-and-tuples/

// http://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer

/*
template <size_t ...I>
struct index_sequence {};

template <size_t N, size_t ...I>
struct make_index_sequence : public make_index_sequence<N - 1, N - 1, I...> {};

template <size_t ...I>
struct make_index_sequence<0, I...> : public index_sequence<I...> {};
*/

template<typename RetType, typename... Ts>
struct FuncInfoHolder
{
	typedef RetType(*FuncType)(Ts...);

	struct FuncRuntime
	{
		static duk_ret_t call_native_function(duk_context *ctx)
		{
			auto bctx = make_context_base(ctx);

			duk_push_current_function(ctx);
			duk_get_prop_string(ctx, -1, "\xFF" "func_ptr");
			void* fp_void = duk_require_pointer(ctx, -1);
			if (fp_void == NULL) {
				duk_error(ctx, DUK_RET_TYPE_ERROR, "what even");
				return DUK_RET_TYPE_ERROR;
			}
			duk_pop_2(ctx);

			RetType(*funcToCall)(Ts...) = reinterpret_cast<RetType(*)(Ts...)>(fp_void);

			actually_call(bctx, funcToCall, get_stack_values(bctx));

			return std::is_void<RetType>::value ? 0 : 1;
		}

		template<size_t... N>
		static std::tuple<Ts...> get_stack_values_helper(BaseContext::Ptr ctx, std::index_sequence<N...>)
		{
			return std::make_tuple(Value<Ts>::require(ctx, N)...);
		}

		static std::tuple<Ts...> get_stack_values(BaseContext::Ptr ctx)
		{
			constexpr std::size_t num_args = sizeof...(Ts);
			return get_stack_values_helper(ctx, typename std::make_index_sequence<num_args>::type());
		}

		// this mess is to support functions with void return values
		template<typename Dummy = RetType>
		static typename std::enable_if<!std::is_void<Dummy>::value>::type actually_call(BaseContext::Ptr ctx, RetType(*funcToCall)(Ts...), std::tuple<Ts...> args)
		{
			// ArgStorage has some static_asserts in it that validate value types,
			// so we typedef it to force ArgStorage<RetType> to compile and run the asserts
			//typedef typename dukglue::types::ArgStorage<RetType>::type ValidateReturnType;

			RetType return_val = apply_fp_helper(funcToCall, typename std::make_index_sequence<sizeof...(Ts)>::type(), args);

			Value<RetType>::push(ctx, return_val);
		}

		template<typename Dummy = RetType>
		static typename std::enable_if<std::is_void<Dummy>::value>::type actually_call(BaseContext::Ptr ctx, RetType(*funcToCall)(Ts...), std::tuple<Ts...> args)
		{
			apply_fp_helper(funcToCall, typename std::make_index_sequence<sizeof...(Ts)>::type(), args);
		}

		// function pointer
		template<size_t... Indexes >
		static RetType apply_fp_helper(RetType(*pf)(Ts...), std::index_sequence< Indexes... >, std::tuple<Ts...> tup)
		{
			return pf(std::forward<Ts>(std::get<Indexes>(tup))...);
		}
	};
};

} }
