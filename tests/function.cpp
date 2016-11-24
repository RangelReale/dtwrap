#include <catch.hpp>

#include <dtwrap/Context.hpp>

using namespace dtwrap;

TEST_CASE("Test function", "[ref][func]") {

	const char* test_script = R"JS(
	var infofunc = function()
	{
		return 'testinfo_func';
	}
	var infofunc2 = function(a, b)
	{
		return 'testinfo_func'+a+b;
	}

	var infoobj = {
		objprop: 7662,

		objfunc1: function()
		{
			return 'objfunc1_test';
		},
		objfunc2: function(a, b)
		{
			return 'objfunc2_test'+a+b;
		},
		objfunc3: function()
		{
			return this.objprop;
		},
	};

	)JS";

	auto ctx = make_context();
	REQUIRE_NOTHROW(ctx->eval(test_script));

	CHECK(ctx->global()->getProp("infofunc")->call()->get<std::string>() == "testinfo_func");
	CHECK(ctx->global()->getProp("infofunc2")->call("x", "y")->get<std::string>() == "testinfo_funcxy");

	CHECK(ctx->global()->getProp("infoobj")->getProp("objfunc1")->call()->get<std::string>() == "objfunc1_test");
	CHECK(ctx->global()->getProp("infoobj")->getProp("objfunc2")->call("x", "y")->get<std::string>() == "objfunc2_testxy");
	CHECK(ctx->global()->getProp("infoobj")->callProp("objfunc3")->get<int>() == 7662);

	// must not leave anything on the stack
	CHECK(duk_get_top(*ctx) == 0);

}
