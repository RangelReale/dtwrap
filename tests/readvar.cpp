#include <catch.hpp>

#include <dtwrap/Context.hpp>

using namespace dtwrap;

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