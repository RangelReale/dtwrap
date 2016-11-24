#ifdef WIN32
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h> 
#endif

#include <dtwrap/Context.hpp>
#include <dtwrap/Ref.hpp>
#include <dtwrap/Optional.hpp>
#include <dtwrap/ClassInstance.hpp>
#include <dtwrap/util/Value.hpp>

#include <iostream>
#include <functional>

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

void raw_native_noreturn(Optional<int> x)
{
	if (x)
		std::cout << "---- RAW_NATIVE_NORETURN: " << x.val() << std::endl;
	else
		std::cout << "---- RAW_NATIVE_NORETURN: " << "NO VALUE" << std::endl;
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

	ctx->global()->putProp("rnoreturn", ctx->createRef(&raw_native_noreturn));

	//ctx->global()->getProp("rnoreturn")->call(ctx->createRef(Type::UNDEFINED));
	ctx->global()->getProp("rnoreturn")->call();

	std::cout << "@@STDFUNCTION" << std::endl;
	std::function<int(int, int)> f_raw_native_print_add_number = raw_native_print_add_number;

	ctx->global()->putProp("rstdfunc", ctx->createRef(f_raw_native_print_add_number));
	ctx->global()->getProp("rstdfunc")->call(15, 20);

	// lambdas not supported
	//std::cout << "@@LAMBDA" << std::endl;
	//ctx->global()->putProp("rlamba", ctx->createRef([](int x) { std::cout << "LAMBDA " << x << std::endl; }));
	//ctx->global()->getProp("rlamba")->call(488);


	// Enum object fields
	auto e = ctx->global()->getProp("Info")->getEnum();
	
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
	std::cout << "REF MAX: " << ctx->refMax() << std::endl;

	duk_dump_context_stdout(*ctx);
}


class TClass1
{
public:
	typedef std::shared_ptr<TClass1> Ptr;

	TClass1(int v1, int v2) : _v1(v1), _v2(v2) {}

	int calc(int v3) { 
		return _v1 + _v2 + v3; 
	}
private:
	int _v1, _v2;
};

namespace dtwrap {

	template<>
	struct ClassInstance<TClass1>
	{
		ClassInstance(const TClass1 *c) : _c() {}

		operator TClass1*()
		{
			return _c;
		}

		operator TClass1 const* ()
		{
			return _c;
		}

		TClass1 *_c;
	};

}

namespace dtwrap {
namespace util {

	/*
	template<>
	struct Value<TClass1*> {
		static void push(BaseContext::Ptr ctx, TClass1* value)
		{
			Ref::Ptr obj(createRef(ctx, Type::OBJECT));

			obj->push();

			// put instance in object's obj_ptr
			duk_push_pointer(*ctx, reinterpret_cast<void*>(value));
			duk_put_prop_string(*ctx, -2, "\xFF" "obj_ptr");

			// add methods
			obj->putProp("calc", createRef(ctx, &TClass1::calc));

			// push finalizer
			duk_push_c_function(*ctx, destruct, 1);
			duk_set_finalizer(*ctx, -2);

			// leave pushed object on the stack
		}

		static duk_ret_t destruct(duk_context *ctx)
		{
			duk_get_prop_string(ctx, -1, "\xFF" "obj_ptr");
			void* op_void = duk_require_pointer(ctx, -1);
			if (op_void == NULL) {
				duk_error(ctx, DUK_RET_TYPE_ERROR, "what even");
				return DUK_RET_TYPE_ERROR;
			}
			duk_pop(ctx);

			TClass1 *obj = reinterpret_cast<TClass1*>(op_void);
			delete obj;

			// set pointer as null
			duk_push_pointer(ctx, NULL);
			duk_put_prop_string(ctx, -2, "\xFF" "obj_ptr");

			return 0;
		}
	};
	*/

} }



void test_class()
{
	auto ctx = make_context();

	auto c = new TClass1(5, 10);

	//auto cc = ClassInstance<TClass1>(NULL);
	//c = cc;

	//auto cl = ctx->createRef(ClassInstance<TClass1>(c));
	auto cl = ctx->createRef(c);
	//std::cout << cl->callProp("calc", 10, 20)->get<int>() << std::endl;

	/*
	auto ci = TClass1_ClassInstance();
	ci.reg(ctx, c);

	//auto cl = ctx->createRef(c);
	*/

	std::cout << "REF AMOUNT: " << ctx->refCount() << std::endl;
	std::cout << "REF MAX: " << ctx->refMax() << std::endl;

	duk_dump_context_stdout(*ctx);
}

int main(int argc, char *argv[])
{
	try
	{
		//test_core();
		test_class();
	}
	catch (std::exception &e) {
		std::cout << "ERROR: " << e.what() << std::endl;
	}

#ifdef WIN32
	_CrtDumpMemoryLeaks();
#endif

	std::cout << "PRESS ANY KEY TO CONTINUE";
	std::cin.ignore();
}
