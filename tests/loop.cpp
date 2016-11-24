#include <catch.hpp>

#include <dtwrap/Context.hpp>

using namespace dtwrap;

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
