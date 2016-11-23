SET( LIB_SOURCES
    ../duktape/duktape.c
    
    Type.cpp
    Exception.cpp
    BaseContext.cpp
    Context.cpp
    Ref.cpp

    util/StackPop.cpp
    util/Util.cpp
)    

SET( LIB_INCS
    ../duktape/duktape.h
    ../duktape/duk_config.h

	../include/dtwrap/IntrusiveRefCntPtr.h
    ../include/dtwrap/Type.hpp
    ../include/dtwrap/Exception.hpp
    ../include/dtwrap/BaseContext.hpp
    ../include/dtwrap/Context.hpp
    ../include/dtwrap/Ref.hpp

    ../include/dtwrap/util/Value.hpp
    ../include/dtwrap/util/StackPop.hpp
    ../include/dtwrap/util/Util.hpp
    ../include/dtwrap/util/Function.hpp
)

ADD_LIBRARY(dtwrap ${LIB_SOURCES} ${LIB_INCS})
