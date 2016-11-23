#include <dtwrap/Context.hpp>
#include <dtwrap/Ref.hpp>
#include <dtwrap/util/Value.hpp>

#include <iostream>

using namespace dtwrap;
using namespace dtwrap::util;

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
	)JS");


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
