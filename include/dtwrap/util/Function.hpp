#pragma once

#include <dtwrap/util/Value.hpp>

#include <tuple>
#include <type_traits>
#include <utility>

namespace dtwrap {
namespace util {

// http://www.preney.ca/paul/archives/486
// https://www.murrayc.com/permalink/2015/12/05/modern-c-variadic-template-parameters-and-tuples/

// http://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer
template<int ...> struct seq {};

template<int N, int ...S> struct gens : gens<N - 1, N - 1, S...> {};

template<int ...S> struct gens<0, S...> { typedef seq<S...> type; };


template<typename RetType, typename... Ts>
struct FuncInfoHolder
{
	typedef RetType(*FuncType)(Ts...);

	struct FuncRuntime
	{
		static duk_ret_t call_native_function(duk_context *ctx)
		{
			duk_push_current_function(ctx);
			duk_get_prop_string(ctx, -1, "\xFF" "func_ptr");
			void* fp_void = duk_require_pointer(ctx, -1);
			if (fp_void == NULL) {
				duk_error(ctx, DUK_RET_TYPE_ERROR, "what even");
				return DUK_RET_TYPE_ERROR;
			}
			duk_pop_2(ctx);

			RetType(*funcToCall)(Ts...) = reinterpret_cast<RetType(*)(Ts...)>(fp_void);

			std::tuple<Ts...> args = get_stack_values(ctx);

			/*
			std::tuple<Ts...> args;
			for (int x = 0; x < sizeof...(Ts); x++)
			{
				std::get<x>(args) = 1;
			}
			*/

			funcToCall(999);

			//return std::is_void<RetType>::value ? 0 : 1;
			return 0;
		}

		template<size_t... Indexes>
		std::tuple<Ts...> get_stack_values_helper(duk_context *ctx, std::index_sequence<Indexes...>)
		{
			return std::forward_as_tuple(Value<Ts..>::get(ctx, Indexes)...);
		}

		static std::tuple<Ts...> get_stack_values(duk_context *ctx)
		{
			auto indices = typename std::index_sequence_for<Ts...>::type();
			return get_stack_values_helper<Ts...>(ctx, indices);
		}

		/*
		template<typename Dummy = RetType>
		static typename std::enable_if<!std::is_void<Dummy>::value>::type actually_call(duk_context *ctx, RetType(*funcToCall)(Ts...), std::tuple<Ts..>&& args)
		{

		}
		*/
	};
};

} }
