cmake_minimum_required(VERSION 3.13)
project(Core)

# CORE_SOURCES is defined to be a list of all the source files for my game 
# DON'T ADD THE SOURCES BY HAND, they are already added with this macro
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE CORE_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/Core/*.h")
add_library(Core STATIC ${CORE_SOURCES} ${CORE_HEADERS})

set_property(TARGET "${PROJECT_NAME}" PROPERTY CXX_STANDARD 23)

target_include_directories("${PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

target_link_libraries("${PROJECT_NAME}" PUBLIC
	SFML::Graphics
	fmt
)

if(PRODUCTION_BUILD)
	target_compile_definitions(Core PUBLIC PRODUCTION_BUILD=1)
else()
	target_compile_definitions(Core PUBLIC PRODUCTION_BUILD=0)
endif()