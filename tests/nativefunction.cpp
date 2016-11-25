#include <catch.hpp>

#include <dtwrap/Context.hpp>
#include <dtwrap/util/Function.hpp>
#include <dtwrap/util/StdFunction.hpp>

#include <functional>

using namespace dtwrap;

// parameter: int
// return: parameter + 20
duk_ret_t natfunc_dukfunction(duk_context *ctx)
{
	int value = duk_require_int(ctx, 0);

	duk_push_int(ctx, value + 20);
	return 1;
}

int natfunc_direct(int value)
{
	return value + 20;
}

std::string natfunc_direct2(int value, std::string str)
{
	std::stringstream ss;
	ss << str << "_" << value;
	return ss.str();
}

int natfunc_direct_optional(Optional<int> value)
{
	if (value)
		return value.val() + 20;
	else
		return 3;
}

void natfunc_direct_noresult(int value)
{
	// do nothing
}


TEST_CASE("Test native functions", "[ref][func]") {

	const char* test_script = R"JS(

	var call_natfunc_dukfunction = function(value)
	{
		return natfunc_dukfunction(value);
	}

	var call_natfunc_direct = function(value)
	{
		return natfunc_direct(value);
	}

	var call_natfunc_direct2 = function(value, str)
	{
		return natfunc_direct2(value, str);
	}

	var call_stdfunc2 = function(value, str)
	{
		return stdfunc2(value, str);
	}

	var call_natfunc_direct_optional = function(value)
	{
		return natfunc_direct_optional(value);
	}

	var call_natfunc_direct_noresult = function(value)
	{
		return natfunc_direct_noresult(value);
	}

	)JS";

	auto ctx = make_context();
	REQUIRE_NOTHROW(ctx->eval(test_script));

	ctx->global()->putProp("natfunc_dukfunction", ctx->createRef(natfunc_dukfunction));
	CHECK(ctx->global()->getProp("natfunc_dukfunction")->call(15)->get<int>() == 35);
	CHECK(ctx->global()->getProp("call_natfunc_dukfunction")->call(15)->get<int>() == 35);

	ctx->global()->putProp("natfunc_direct", ctx->createRef(natfunc_direct));
	CHECK(ctx->global()->getProp("natfunc_direct")->call(15)->get<int>() == 35);
	CHECK(ctx->global()->getProp("call_natfunc_direct")->call(15)->get<int>() == 35);

	ctx->global()->putProp("natfunc_direct2", ctx->createRef(natfunc_direct2));
	CHECK(ctx->global()->getProp("natfunc_direct2")->call(15, "natf")->get<std::string>() == "natf_15");
	CHECK(ctx->global()->getProp("call_natfunc_direct2")->call(15, "natf")->get<std::string>() == "natf_15");

	std::function<std::string(int, std::string)> stdfunc2 = natfunc_direct2;
	ctx->global()->putProp("stdfunc2", ctx->createRef(stdfunc2));
	CHECK(ctx->global()->getProp("stdfunc2")->call(15, "natf")->get<std::string>() == "natf_15");
	CHECK(ctx->global()->getProp("stdfunc2")->call(15, "natf")->get<std::string>() == "natf_15");

	ctx->global()->putProp("natfunc_direct_optional", ctx->createRef(natfunc_direct_optional));
	CHECK(ctx->global()->getProp("natfunc_direct_optional")->call(15)->get<int>() == 35);
	CHECK(ctx->global()->getProp("natfunc_direct_optional")->call()->get<int>() == 3);
	CHECK(ctx->global()->getProp("call_natfunc_direct_optional")->call(15)->get<int>() == 35);
	CHECK(ctx->global()->getProp("call_natfunc_direct_optional")->call()->get<int>() == 3);

	ctx->global()->putProp("natfunc_direct_noresult", ctx->createRef(natfunc_direct_noresult));
	CHECK_NOTHROW(ctx->global()->getProp("natfunc_direct_noresult")->call(15));
	CHECK_NOTHROW(ctx->global()->getProp("call_natfunc_direct_noresult")->call(15));

	// must not leave anything on the stack
	CHECK(duk_get_top(*ctx) == 0);

}
