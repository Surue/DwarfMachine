cmake_minimum_required(VERSION 3.8.0 FATAL_ERROR)
project(DwarfMachine VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(WIN32)
add_compile_definitions(NOMINMAX)
endif(WIN32)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_VISIBILITY_PRESET hidden)

set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/externals)

include_directories(include ${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE SRC src/*.cpp include/*.h)

set_property(GLOBAL PROPERTY USE_FOLDERS On)

if(MSVC)
    add_compile_options(
        $<$<CONFIG:>:/MT> #---------|
        $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
    )
endif()

set(_DWARF_MACHINE_ALL_SYSTEM_LIBS true)

# Looks for a appropriate threads package for this platform
find_package(Threads REQUIRED)
# Finds and loads Vulkan, env "VULKAN_SDK" must be set
find_package(Vulkan REQUIRED)

# Finds and loads glfw3
find_package(glfw3)
if(NOT TARGET glfw)
	set(_DWARF_MACHINE_ALL_SYSTEM_LIBS false)
	# Used later to define as a pre-build dependency
	# The glfw3Config.cmake file doesn't define it automatically
	set(glfw_FOUND false)
	foreach(_glfw3_option "GLFW_BUILD_TESTS" "GLFW_BUILD_EXAMPLES" "GLFW_BUILD_DOCS" "GLFW_INSTALL" "GLFW_VULKAN_STATIC")
		set(${_glfw3_option} OFF CACHE INTERNAL "")
	endforeach()

	add_subdirectory(${PROJECT_SOURCE_DIR}/Libraries/glfw)
else()
	set(glfw_FOUND true)
endif()

set(SPIRV_ROOT CACHE PATH "An optional path to the system's SPIRV root dir to help find it. Ignore if building Glslang locally.")
find_library(SPIRV_LIBRARY
		NAMES "SPIRV" "libSPIRV"
		HINTS "${SPIRV_ROOT}"
		)
find_path(SPIRV_INCLUDE_DIR
		NAMES "GlslangToSpv.h"
		PATH_SUFFIXES "SPIRV"
		HINTS "${SPIRV_ROOT}"
		)

if(NOT SPIRV_LIBRARY OR NOT SPIRV_INCLUDE_DIR)
	set(_DWARF_MACHINE_ALL_SYSTEM_LIBS false)
	# Used later to define as a pre-build dependencies
	# Have to manually define because we manually searched for SPIRV
	set(SPIRV_FOUND false)
	foreach(_glslang_option "BUILD_TESTING" "ENABLE_GLSLANG_BINARIES" "ENABLE_SPVREMAPPER" "ENABLE_HLSL" "ENABLE_AMD_EXTENSIONS" "ENABLE_NV_EXTENSIONS")
		set(${_glslang_option} OFF CACHE INTERNAL "")
	endforeach()
	foreach(_glslang_option "SKIP_GLSLANG_INSTALL" "ENABLE_OPT")
		set(${_glslang_option} ON CACHE INTERNAL "")
	endforeach()
	
	add_subdirectory(${PROJECT_SOURCE_DIR}/Libraries/glslang)
	# Used in target_link_libraries()
	set(SPIRV_LIBRARY "SPIRV")
else()
	set(SPIRV_FOUND true)
endif()
