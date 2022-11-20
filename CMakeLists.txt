project(KataScript CXX)
cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

file(GLOB src "src/Library/*.hpp" "src/Interpreter/*.cpp")

# Generate a shared library if needed
#add_library(KataScriptlib SHARED ${src})

add_executable(KataScript ${src})

if (WIN32)
	set(KATASCRIPT_EXEC KataScript.exe)
	#set(KATASCRIPT_LIB libKataScriptlib.dll)
	#target_link_libraries(KataScript KataScriptlib)
elseif (APPLE)
	set(KATASCRIPT_EXEC KataScript)
	#set(KATASCRIPT_LIB libKataScriptlib.dylib)
	#target_link_libraries(KataScript KataScriptlib)
else()
	set(KATASCRIPT_EXEC KataScript)
	#set(KATASCRIPT_LIB libKataScriptlib.so)
	#target_link_libraries(KataScript KataScriptlib)
endif()

add_custom_command(TARGET KataScript
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${KATASCRIPT_EXEC} ${CMAKE_SOURCE_DIR})
