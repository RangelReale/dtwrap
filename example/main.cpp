#include <dtwrap/Context.hpp>
#include <dtwrap/Ref.hpp>
#include <dtwrap/util/Value.hpp>

#include <iostream>

using namespace dtwrap;
using namespace dtwrap::util;

duk_ret_t native_print_add_number(duk_context *ctx)
{
	std::cout << "---- NATIVE_PRINT_ADD_NUMBER PARAMS: " << duk_get_top(ctx) << std::endl;

	int value = duk_require_int(ctx, 0);

	std::cout << "---- NATIVE_PRINT_ADD_NUMBER: " << value << std::endl;

	duk_push_int(ctx, value + 11);
	return 1;
}

int raw_native_print_add_number(int value, int value2)
{
	std::cout << "---- RAW_NATIVE_PRINT_ADD_NUMBER: " << value << std::endl;
	return value + 11;
}

void test_core()
{
	auto ctx = make_context();

	ctx->eval(R"JS(
	var Info = {
		version: 1,
		description: 'Test JS Plugin',
		test1: 41,
		test2: true,

		functest: function(v1, v2)
		{
			print(this);
			return "Info.functest = " + v1 + "-" + v2+" [" + this.description + "]";	
		}
	};

	var GetObject = function() 
	{
		return "axyz";
	}

	var func2 = function(v1, v2)
	{
		print(this);

		return "func2 = " + v1 + "-" + v2;
	}

	var xatest = [12, "abc", "xyz"];

	var call_cfunc = function()
	{
		return testcfunc(14);
	}	

	var call_rcfunc = function()
	{
		return rtestcfunc(35, 15);
	}	

	)JS");

	// register c function
	ctx->global()->putProp("testcfunc", ctx->createRef(&native_print_add_number));

	// call c function
	std::cout << "C FUNC=" << ctx->global()->getProp("testcfunc")->call(55)->get<int>() << std::endl;
	std::cout << "C FUNC VIA JS=" << ctx->global()->getProp("call_cfunc")->call()->get<int>() << std::endl;

	// register raw c function
	ctx->global()->putProp("rtestcfunc", ctx->createRef(&raw_native_print_add_number));

	std::cout << "RAW C FUNC=" << ctx->global()->getProp("rtestcfunc")->call(81, 90)->get<int>() << std::endl;
	std::cout << "RAW C FUNC VIA JS=" << ctx->global()->getProp("call_rcfunc")->call()->get<int>() << std::endl;

	// Enum object fields
	auto e = dynamic_pointer_cast<RefEnum>(ctx->global()->getProp("Info")->getEnum());
	
	std::cout << "=======" << std::endl;
	for (Ref::Ptr i = e->next(); i; i = e->next())
	{
		std::cout << i->key()->cast<std::string>();
		std::cout << " == ";
		std::cout << i->cast<std::string>();
		std::cout << std::endl;
	}
	std::cout << "=======" << std::endl;

	e.reset();

	/***/
	std::cout << ctx->global()->getProp("Info")->getProp("description")->get<std::string>() << std::endl;
	std::cout << ctx->global()->getProp("Info")->getProp("description")->description() << std::endl;

	std::cout << ctx->global()->getProp("Info")->getProp("description")->type() << std::endl;

	std::cout << ctx->global()->getProp("Info")->getProp("test1")->cast<std::string>() << std::endl;
	//std::cout << ctx->global()->getProp("Info")->getProp("test1")->cast<const char*>() << std::endl; // static_assert

	std::cout << ctx->global()->getProp("xatest")->getIndex(1)->get<std::string>() << std::endl;

	for (std::size_t i = 0; i < ctx->global()->getProp("xatest")->length(); i++)
	{
		std::cout << "@@@ " << i << " - " << ctx->global()->getProp("xatest")->getIndex(i)->cast<std::string>() << std::endl;
	}

	ctx->global()->putProp("xatest", ctx->createRef(98872));
	std::cout << "CHANGED PROP: " << ctx->global()->getProp("xatest")->get<int>() << std::endl;

	auto objtest = ctx->global()->putProp("objtest", ctx->createRef(Type::OBJECT));
	objtest->putProp("test1", ctx->createRef(12349));
	objtest->putProp("test2", ctx->createRef("test2_value"));
	objtest.reset();

	std::cout << ctx->global()->getProp("objtest")->getProp("test2")->get<std::string>() << std::endl;

	/***/

	/***/
	std::cout << ctx->global()->getProp("GetObject")->cast<std::string>() << std::endl;
	std::cout << ctx->global()->getProp("GetObject")->type() << std::endl;
	std::cout << ctx->global()->getProp("GetObject")->isFunction() << std::endl;

	std::cout << ctx->global()->getProp("GetObject")->call()->get<std::string>() << std::endl;
	std::cout << ctx->global()->getProp("GetObject")->call()->description() << std::endl;

	std::cout << ctx->global()->getProp("func2")->call("maria", 15)->get<std::string>() << std::endl;

	std::cout << ctx->global()->getProp("Info")->getProp("functest")->call("keila", "matos")->get<std::string>() << std::endl;

	std::cout << ctx->global()->getProp("Info")->callProp("functest", "keila", "matos")->get<std::string>() << std::endl;

	std::cout << ctx->global()->getProp("Info")->getProp("functest")->callMethod(ctx->global()->getProp("Info"), "keila", "matos")->get<std::string>() << std::endl;
	/***/

	std::cout << "REF AMOUNT: " << ctx->refCount() << std::endl;

	duk_dump_context_stdout(*ctx);
}

int main(int argc, char *argv[])
{
	try
	{
		test_core();
		//test_readobj();
	}
	catch (std::exception &e) {
		std::cout << "ERROR: " << e.what() << std::endl;
	}

	std::cout << "PRESS ANY KEY TO CONTINUE";
	std::cin.ignore();
}
