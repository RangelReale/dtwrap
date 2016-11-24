#include <catch.hpp>

#include <dtwrap/Context.hpp>
#include <dtwrap/util/Value.hpp>

using namespace dtwrap;
using namespace dtwrap::util;

TEST_CASE("Value", "[duk]") {

	auto ctx = make_context();

	Value<bool>::push(ctx, true);
	CHECK(duk_is_boolean(*ctx, -1) != 0);
	CHECK(Value<bool>::get(ctx, -1) == true);
	duk_pop(*ctx);

	Value<unsigned int>::push(ctx, 48);
	CHECK(duk_is_number(*ctx, -1) != 0);
	CHECK(Value<unsigned int>::get(ctx, -1) == 48);
	duk_pop(*ctx);

	Value<int>::push(ctx, -48);
	CHECK(duk_is_number(*ctx, -1) != 0);
	CHECK(Value<int>::get(ctx, -1) == -48);
	duk_pop(*ctx);

	Value<float>::push(ctx, float(93.7));
	CHECK(duk_is_number(*ctx, -1) != 0);
	CHECK(Value<float>::get(ctx, -1) == Approx(93.7));
	duk_pop(*ctx);

	Value<std::string>::push(ctx, "strval");
	CHECK(duk_is_string(*ctx, -1) != 0);
	CHECK(Value<std::string>::get(ctx, -1) == "strval");
	duk_pop(*ctx);

	Value<Type>::push(ctx, Type::UNDEFINED);
	CHECK(duk_is_undefined(*ctx, -1) != 0);
	duk_pop(*ctx);

	Value<Type>::push(ctx, Type::TNULL);
	CHECK(duk_is_null(*ctx, -1) != 0);
	duk_pop(*ctx);

	Value<Type>::push(ctx, Type::OBJECT);
	CHECK(duk_is_object(*ctx, -1) != 0);
	duk_pop(*ctx);

	Value<Type>::push(ctx, Type::PUSH_ARRAY);
	CHECK(duk_is_array(*ctx, -1) != 0);
	duk_pop(*ctx);

	// must not leave anything on the stack
	CHECK(duk_get_top(*ctx) == 0);

}