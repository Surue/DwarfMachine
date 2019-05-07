cmake_minimum_required(VERSION 3.8.0 FATAL_ERROR)
project(DwarfMachine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/externals)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(WIN32)
add_compile_definitions(NOMINMAX)
endif(WIN32)

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

# Use ccache if it is installed, will cach previous compilations
find_program(CCACHE_FOUND "ccache")
if(CCACHE_FOUND)
	message(STATUS "Using ccache for building.")
	set_property(GLOBAL PROPERTY LAUNCH_COMPILE ccache)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

set(_DWARF_MACHINE_ALL_SYSTEM_LIBS true)

# Looks for a appropriate threads package for this platform
find_package(Threads REQUIRED)
# Finds and loads Vulkan, env "VULKAN_SDK" must be set
find_package(Vulkan REQUIRED)
LIST(APPEND DWARF_MACHINE_LIBRARIES Vulkan::Vulkan)


include_directories(include ${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE DWARF_MACHINE_AUDIO_SRC src/audio/*.cpp include/audio/*.h)
file(GLOB_RECURSE DWARF_MACHINE_COMPONENT_SRC src/component/*.cpp include/component/*.h)
file(GLOB_RECURSE DWARF_MACHINE_EDITOR_SRC src/editor/*.cpp include/editor/*.h)
file(GLOB_RECURSE DWARF_MACHINE_ENGINE_SRC src/engine/*.cpp include/engine/*.h)
file(GLOB_RECURSE DWARF_MACHINE_ENTITY_SRC src/entity/*.cpp include/entity/*.h)
file(GLOB_RECURSE DWARF_MACHINE_GRAPHICS_SRC src/graphics/*.cpp include/graphics/*.h)
file(GLOB_RECURSE DWARF_MACHINE_INPUT_SRC src/input/*.cpp include/input/*.h)
file(GLOB_RECURSE DWARF_MACHINE_SYSTEM_SRC src/system/*.cpp include/system/*.h)
file(GLOB_RECURSE DWARF_MACHINE_UTILITY_SRC src/utility/*.cpp include/utility/*.h)

set_property(GLOBAL PROPERTY USE_FOLDERS On)
SET_SOURCE_FILES_PROPERTIES( ${DWARF_MACHINE_SRC} PROPERTIES
                FOLDER "${CMAKE_SOURCE_DIR}/src" )

source_group("Audio"               FILES ${DWARF_MACHINE_AUDIO_SRC})
source_group("Component"           FILES ${DWARF_MACHINE_COMPONENT_SRC})
source_group("Editor"              FILES ${DWARF_MACHINE_EDITOR_SRC})
source_group("Engine"              FILES ${DWARF_MACHINE_ENGINE_SRC})
source_group("Entity"              FILES ${DWARF_MACHINE_ENTITY_SRC})
source_group("Graphics"            FILES ${DWARF_MACHINE_GRAPHICS_SRC})
source_group("Input"               FILES ${DWARF_MACHINE_INPUT_SRC})
source_group("System"              FILES ${DWARF_MACHINE_SYSTEM_SRC})
source_group("Utility"             FILES ${DWARF_MACHINE_UTILITY_SRC})

file(GLOB_RECURSE SHADERS_SRC shaders/*.vert shaders/*.frag)
source_group("Shaders" FILES ${SHADERS_SRC})

List(APPEND DWARF_MACHINE_SRC  ${DWARF_MACHINE_AUDIO_SRC} ${DWARF_MACHINE_COMPONENT_SRC} ${DWARF_MACHINE_EDITOR_SRC} ${DWARF_MACHINE_ENGINE_SRC} ${DWARF_MACHINE_ENTITY_SRC} 
${DWARF_MACHINE_GRAPHICS_SRC} ${DWARF_MACHINE_SYSTEM_SRC} ${DWARF_MACHINE_INPUT_SRC} ${DWARF_MACHINE_UTILITY_SRC} ${SHADERS_SRC})

#SDL
set(SDL_VERSION 2.0.9)
set(SDL_DIR ${EXTERNAL_DIR}/SDL2-${SDL_VERSION}/)
set(SDL_STATIC ON)
set(SDL_SHARED OFF)
add_subdirectory(${SDL_DIR})
include_directories(${SDL2_DIR}/include ${SDL2_DIR}/src)
LIST(APPEND DWARF_MACHINE_LIBRARIES SDL2-static)
if(MSVC)
	LIST(APPEND DWARF_MACHINE_LIBRARIES SDL2main)
endif(MSVC)

#ImGui
#include_directories(externals/imgui)
set(IMGUI_ROOT ${EXTERNAL_DIR}/imgui/)
set(SDL2_IMGUI ${EXTERNAL_DIR}/imgui-sdl/)
add_subdirectory(${SDL2_IMGUI})
include_directories(${IMGUI_ROOT} ${SDL2_IMGUI})

LIST(APPEND DWARF_MACHINE_LIBRARIES imgui-sdl)

# SPIRV is needed (from Glslang)
# NOTE: End-users can pass -DSPIRV_ROOT=/some/path to find the lib
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
	
	add_subdirectory(${PROJECT_SOURCE_DIR}/externals/glslang)
	# Used in target_link_libraries()
	set(SPIRV_LIBRARY "SPIRV")
else()
	set(SPIRV_FOUND true)
endif()

#Google Test
set(GOOGLE_TEST_DIR ${EXTERNAL_DIR}/googletest)
add_subdirectory(${GOOGLE_TEST_DIR})
include_directories(${GOOGLE_TEST_DIR}/include)
set_target_properties (gtest gtest_main PROPERTIES
		FOLDER GTest)

#DWARF_MACHINE_COMMON
LIST(APPEND DWARF_MACHINE_LIBRARIES)
if(UNIX AND NOT APPLE)
	LIST(APPEND DWARF_MACHINE_LIBRARIES
		stdc++fs)
ENDIF()

include_directories(include/)
include_directories(extensions/include/)

file(GLOB_RECURSE DWARF_MACHINE_EXTENSIONS_SRC extensions/src/*.cpp extensions/include/*.h)

macro(GroupSources curdir)
	file(GLOB children RELATIVE ${CMAKE_SOURCE_DIR}/${curdir} ${CMAKE_SOURCE_DIR}/${curdir}/*)
	foreach(child ${children})
			if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "extensions" "Extension" groupname ${groupname})
				source_group(${groupname} FILES ${CMAKE_SOURCE_DIR}/${curdir}/${child})
			endif()
	endforeach()
endmacro()

# Execute the macro
GroupSources(extensions)
add_library(DWARF_MACHINE_COMMON STATIC ${DWARF_MACHINE_SRC} ${DWARF_MACHINE_EXTENSIONS_SRC})

#DWARF_MACHINE_COMMON
message("DWARF MACHINE LIBRARIES : ${DWARF_MACHINE_LIBRARIES}")
target_link_libraries(DWARF_MACHINE_COMMON PUBLIC ${DWARF_MACHINE_LIBRARIES})
set_property(TARGET DWARF_MACHINE_COMMON PROPERTY CXX_STANDARD 17)

target_link_libraries(DWARF_MACHINE_COMMON PRIVATE ${SPIRV_LIBRARY})

target_compile_definitions(DWARF_MACHINE_COMMON PRIVATE VK_USE_PLATFORM_WIN32_KHR)

#TEST
SET(DWARF_MACHINE_TEST_DIR ${CMAKE_SOURCE_DIR}/tests)
file(GLOB TEST_FILES ${DWARF_MACHINE_TEST_DIR}/*.cpp )
add_executable(DWARF_MACHINE_TEST ${TEST_FILES} )
target_link_libraries(DWARF_MACHINE_TEST gtest gtest_main DWARF_MACHINE_COMMON)
if(APPLE )
	set_target_properties(DWARF_MACHINE_TEST PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
		ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
ENDIF()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fvisibility=hidden" )
elseif()
	SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fvisibility=hidden -masm=intel" )
endif()
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")

#DWARF_MACHINE
add_executable(DWARF_MACHINE src/main.cpp)

target_link_libraries(DWARF_MACHINE PUBLIC DWARF_MACHINE_COMMON)
set_property(TARGET DWARF_MACHINE PROPERTY CXX_STANDARD 17)
if(APPLE)
	set_target_properties(DWARF_MACHINE PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
		RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
ENDIF()

set_target_properties(DWARF_MACHINE_COMMON PROPERTIES COMPILE_FLAGS "-save-temps -ffast-math")

#source
add_custom_command(TARGET DWARF_MACHINE_COMMON POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/ressources ${CMAKE_BINARY_DIR}/ressources)
add_custom_command(TARGET DWARF_MACHINE_COMMON POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/ressources ${CMAKE_BINARY_DIR}/ressources)
file(COPY ressources/ DESTINATION ${CMAKE_BINARY_DIR}/ressources/)

#shaders
add_custom_command(TARGET DWARF_MACHINE_COMMON POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders)
file(COPY shaders/ DESTINATION ${CMAKE_BINARY_DIR}/shaders/)