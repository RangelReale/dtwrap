include_directories(.)
add_executable(dtwrap_test 
	test.cpp
	readvar.cpp
	loop.cpp
	function.cpp
	nativefunction.cpp
)
target_link_libraries(dtwrap_test dtwrap)

add_test(NAME dtwrap_test COMMAND dtwrap_test)