#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

#include <dtwrap/Context.hpp>

using namespace dtwrap;
//using namespace dtwrap::util;

int main(int argc, char* const argv[])
{
	int result = Catch::Session().run(argc, argv);
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
	var infofunc2 = function(a, b)
	{
		return 'testinfo_func'+a+b;
	}
	)JS";

	auto ctx = make_context();
	REQUIRE_NOTHROW(ctx->eval(test_script));

	CHECK(ctx->global()->getProp("info")->getProp("description")->get<std::string>() == "testinfo");

	CHECK(ctx->global()->getProp("infovar")->get<std::string>() == "infostring");
	CHECK(ctx->global()->getProp("infovar2")->get<int>() == 677);
	CHECK(ctx->global()->getProp("infovar3")->get<float>() == Approx(918.15));

	CHECK(ctx->global()->getProp("infofunc")->call()->get<std::string>() == "testinfo_func");
	CHECK(ctx->global()->getProp("infofunc2")->call("x", "y")->get<std::string>() == "testinfo_funcxy");

	// must not leave anything on the stack
	CHECK(duk_get_top(*ctx) == 0);

}