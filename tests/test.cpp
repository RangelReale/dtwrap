#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

#include <dtwrap/Context.hpp>

#include <functional>

using namespace dtwrap;
//using namespace dtwrap::util;

int main(int argc, char* const argv[])
{
	int result = Catch::Session().run(argc, argv);
	if (result > 0)
	{
		std::cout << "PRESS ANY KEY TO CONTINUE";
		std::cin.ignore();
	}
	return result;
}

TEST_CASE("Eval script in context", "[context]") {

	const char* test_script = R"JS(
	var info = {version: 1, description: 'test'};
	)JS";

	const char* test_script_bug = R"JS(
	var info = {NOTVALID=1, ERROR=2};
	)JS";

	auto ctx = make_context();
	REQUIRE_NOTHROW(ctx->eval(test_script));
	REQUIRE_THROWS_AS(ctx->eval(test_script_bug), Exception);

}

TEST_CASE("Read variables", "[ref]") {

	const char* test_script = R"JS(
	var info = {version: 1, description: 'testinfo'};

	var infovar = 'infostring';
	var infovar2 = 677;
	var infovar3 = 918.15;

	var infofunc = function()
	{
		return 'testinfo_func';
	}
	)JS";

	auto ctx = make_context();
	REQUIRE_NOTHROW(ctx->eval(test_script));

	CHECK(ctx->global()->getProp("info")->getProp("description")->get<std::string>() == "testinfo");

	CHECK(ctx->global()->getProp("infovar")->get<std::string>() == "infostring");
	CHECK(ctx->global()->getProp("infovar2")->get<int>() == 677);
	CHECK(ctx->global()->getProp("infovar3")->get<float>() == Approx(918.15));

	CHECK(ctx->global()->getProp("infofunc")->isFunction() == true);

	// must not leave anything on the stack
	CHECK(duk_get_top(*ctx) == 0);

}

TEST_CASE("Test loop", "[ref]") {

	const char* test_script = R"JS(
	var t_object = {
		prop1: 1,
		prop2: "2",
		prop3: 3.1,
		prop4: function() { return "prop4_result"; }
	};

	var t_array = [1, "2", 3.1, function() { return "prop4_result"; }];

	)JS";

	auto ctx = make_context();
	REQUIRE_NOTHROW(ctx->eval(test_script));

	{
		int objPropCount = 0;
		auto objEnum = ctx->global()->getProp("t_object")->getEnum();
		for (Ref::Ptr i = objEnum->next(); i; i = objEnum->next())
		{
			objPropCount++;
			if (i->key()->cast<std::string>() == "prop1")
				CHECK(i->get<int>() == 1);
			else if (i->key()->cast<std::string>() == "prop2")
				CHECK(i->get<std::string>() == "2");
			else if (i->key()->cast<std::string>() == "prop3")
				CHECK(i->get<float>() == Approx(3.1));
			else if (i->key()->cast<std::string>() == "prop4")
				CHECK(i->call()->get<std::string>() == "prop4_result");
		}
		CHECK(objPropCount == 4);
	}

	int arrayItemCount = 0;
	for (std::size_t i = 0; i < ctx->global()->getProp("t_array")->length(); i++)
	{
		arrayItemCount++;
		auto ref = ctx->global()->getProp("t_array")->getIndex(i);
		if (i == 0)
			CHECK(ref->get<int>() == 1);
		else if (i == 1)
			CHECK(ref->get<std::string>() == "2");
		else if (i == 2)
			CHECK(ref->get<float>() == Approx(3.1));
		else if (i == 3)
			CHECK(ref->call()->get<std::string>() == "prop4_result");
	}
	CHECK(arrayItemCount == 4);

	// must not leave anything on the stack
	CHECK(duk_get_top(*ctx) == 0);

}

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

	std::function<std::string(int,std::string)> stdfunc2 = natfunc_direct2;
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