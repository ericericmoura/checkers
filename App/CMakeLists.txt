cmake_minimum_required(VERSION 3.13)
project(App)

# APP_SOURCES is defined to be a list of all the source files for my game 
# DON'T ADD THE SOURCES BY HAND, they are already added with this macro
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE APP_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

set(MAIN_SOURCE       "${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp")
set(DEBUG_MAIN_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/DebugTerminal.cpp")

list(REMOVE_ITEM APP_SOURCES "${MAIN_SOURCE}" "${DEBUG_MAIN_SOURCE}")

add_executable(Game            ${APP_SOURCES} ${MAIN_SOURCE}       ${APP_HEADERS})
add_executable(DebugTerminal   ${APP_SOURCES} ${DEBUG_MAIN_SOURCE} ${APP_HEADERS})

set(APP_TARGETS Game DebugTerminal)
	
foreach(target ${APP_TARGETS})
	set_property(TARGET "${target}" PROPERTY CXX_STANDARD 23)

	target_include_directories("${target}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

	target_link_libraries("${target}" PRIVATE
		Core
		SFML::Graphics
		fmt
	)

	if(PRODUCTION_BUILD)
		target_compile_definitions("${target}" PUBLIC RESOURCES_PATH="./resources/")
		target_compile_definitions("${target}" PUBLIC PRODUCTION_BUILD=1)
	else()
		target_compile_definitions("${target}" PUBLIC RESOURCES_PATH="${ABSOLUTE_RESOURCES_DIR}")
		target_compile_definitions("${target}" PUBLIC PRODUCTION_BUILD=0)
	endif()

	if(MSVC)
		target_compile_definitions("${target}" PUBLIC _CRT_SECURE_NO_WARNINGS)
	endif()
endforeach()