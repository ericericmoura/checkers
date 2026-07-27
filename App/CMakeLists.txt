cmake_minimum_required(VERSION 3.13)
project(App)

# APP_SOURCES is defined to be a list of all the source files for my game 
# DON'T ADD THE SOURCES BY HAND, they are already added with this macro
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE APP_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
add_executable("${PROJECT_NAME}" ${APP_SOURCES} ${APP_HEADERS})

set_property(TARGET "${PROJECT_NAME}" PROPERTY CXX_STANDARD 23)

target_include_directories("${PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# Add Libraries
target_link_libraries("${PROJECT_NAME}" PRIVATE
	Core
	SFML::Graphics
	fmt
)

if(PRODUCTION_BUILD)
	# setup the ASSETS_PATH macro to be in the root folder of your exe
	target_compile_definitions("${PROJECT_NAME}" PUBLIC RESOURCES_PATH="./resources/") 
	target_compile_definitions("${PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=1) 
else()
	# This is useful to get an ASSETS_PATH in your IDE during development
	target_compile_definitions("${PROJECT_NAME}" PUBLIC RESOURCES_PATH="${ABSOLUTE_RESOURCES_DIR}")
	target_compile_definitions("${PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=0) 
endif()

if(MSVC) # If using the VS compiler...
	target_compile_definitions("${PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)

	#YOU CAN REMOVE THE CONSOLE WITH THIS LINE! YOU CAN EVEN DO AN IF PRODUCTION_BUILD TO REMOVE IT ONLY IN PRODUCTION BUILDS
	#set_target_properties("${PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") #no console
endif()