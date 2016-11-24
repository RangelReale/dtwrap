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

	ctx->global()->putProp("localvar", ctx->createRef(415));
	CHECK(ctx->global()->getProp("localvar")->get<int>() == 415);

	ctx->global()->putProp("localvar2", ctx->createRef("lvarvalue"));
	CHECK(ctx->global()->getProp("localvar2")->get<std::string>() == "lvarvalue");

	{
		auto objtest = ctx->global()->putProp("objtest", ctx->createRef(Type::OBJECT));
		objtest->putProp("test1", ctx->createRef(12349));
		objtest->putProp("test2", ctx->createRef("test2_value"));
	}
	CHECK(ctx->global()->getProp("objtest")->getProp("test1")->get<int>() == 12349);
	CHECK(ctx->global()->getProp("objtest")->getProp("test2")->get<std::string>() == "test2_value");

	{
		auto arrtest = ctx->global()->putProp("arrtest", ctx->createRef(Type::PUSH_ARRAY));
		arrtest->putIndex(0, ctx->createRef(12349));
		arrtest->putIndex(1, ctx->createRef("test2_value"));
	}
	CHECK(ctx->global()->getProp("arrtest")->getIndex(0)->get<int>() == 12349);
	CHECK(ctx->global()->getProp("arrtest")->getIndex(1)->get<std::string>() == "test2_value");

	// must not leave anything on the stack
	CHECK(duk_get_top(*ctx) == 0);

}