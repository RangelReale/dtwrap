#pragma once

#include <dtwrap/BaseContext.hpp>
#include <dtwrap/util/Value.hpp>

#include <tuple>
#include <type_traits>
#include <utility>

namespace dtwrap {
namespace util {

template<class Cls, typename RetType, typename... Ts>
struct MethodInfoHolder
{
	typedef RetType(Cls::*MethodType)(Ts...);

	struct MethodHolder
	{
		MethodType method;
	};

	struct MethodRuntime
	{
		static duk_ret_t call_native_function(duk_context *ctx)
		{
			auto bctx = make_context_base(ctx);

			duk_push_this(ctx);
			if (!duk_is_object(ctx, -1))
			{
				duk_pop(ctx);
				throw Exception("Method must have a valid this");
			}
			duk_get_prop_string(ctx, -1, "\xFF" "obj_ptr");
			void* obj_void = duk_require_pointer(ctx, -1);
			if (obj_void == NULL) {
				duk_error(ctx, DUK_RET_TYPE_ERROR, "'this' doesn't have an obj_ptr pointer ");
				return DUK_RET_TYPE_ERROR;
			}
			duk_pop_2(ctx);

			Cls *obj = static_cast<Cls*>(obj_void);

			duk_push_current_function(ctx);
			duk_get_prop_string(ctx, -1, "\xFF" "method_ptr");
			void* fp_void = duk_require_pointer(ctx, -1);
			if (fp_void == NULL) {
				duk_error(ctx, DUK_RET_TYPE_ERROR, "what even");
				return DUK_RET_TYPE_ERROR;
			}
			duk_pop_2(ctx);

			MethodHolder *methodToCall = reinterpret_cast<MethodHolder*>(fp_void);

			actually_call(bctx, methodToCall->method, obj, get_stack_values(bctx));

			return std::is_void<RetType>::value ? 0 : 1;
		}

		static duk_ret_t finalizer_function(duk_context *ctx)
		{
			auto bctx = make_context_base(ctx);

			duk_get_prop_string(ctx, -1, "\xFF" "method_ptr");
			void* fp_void = duk_require_pointer(ctx, -1);
			if (fp_void == NULL) {
				duk_error(ctx, DUK_RET_TYPE_ERROR, "what even");
				return DUK_RET_TYPE_ERROR;
			}
			duk_pop(ctx);

			MethodHolder *methodToCall = reinterpret_cast<MethodHolder*>(fp_void);
			delete methodToCall;

			// set pointer as null
			duk_push_pointer(ctx, NULL);
			duk_put_prop_string(ctx, -2, "\xFF" "method_ptr");

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
			return get_stack_values_helper(ctx, typename std::make_index_sequence<num_args>::type());
		}

		// this mess is to support functions with void return values
		template<typename Dummy = RetType>
		static typename std::enable_if<!std::is_void<Dummy>::value>::type actually_call(BaseContext::Ptr ctx, MethodType method, Cls *obj, std::tuple<Ts...> args)
		{
			// ArgStorage has some static_asserts in it that validate value types,
			// so we typedef it to force ArgStorage<RetType> to compile and run the asserts
			//typedef typename dukglue::types::ArgStorage<RetType>::type ValidateReturnType;

			RetType return_val = apply_method_helper(method, typename std::make_index_sequence<sizeof...(Ts)>::type(), obj, args);

			Value<RetType>::push(ctx, return_val);
		}

		template<typename Dummy = RetType>
		static typename std::enable_if<std::is_void<Dummy>::value>::type actually_call(BaseContext::Ptr ctx, MethodType method, Cls *obj, std::tuple<Ts...> args)
		{
			apply_method_helper(method, typename std::make_index_sequence<sizeof...(Ts)>::type(), obj, args);
		}

		// function pointer
		template<size_t... Indexes >
		static RetType apply_method_helper(RetType(Cls::*pf)(Ts...), std::index_sequence< Indexes... >, Cls* obj, std::tuple<Ts...> tup)
		{
			return (*obj.*pf)(std::forward<Ts>(std::get<Indexes>(tup))...);
		}
	};
};

} }
