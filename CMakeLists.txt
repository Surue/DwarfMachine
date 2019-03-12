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

include_directories(include ${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE DM_AUDIO_SRC src/audio/*.cpp include/audio/*.h)
file(GLOB_RECURSE DM_ENGINE_SRC src/engine/*.cpp include/engine/*.h)
file(GLOB_RECURSE DM_EDITOR_SRC src/editor/*.cpp include/editor/*.h)
file(GLOB_RECURSE DM_GRAPHICS_SRC src/graphics/*.cpp include/graphics/*.h)
file(GLOB_RECURSE DM_INPUT_SRC src/input/*.cpp include/input/*.h)
file(GLOB_RECURSE DM_PHYSICS_SRC src/physics/*.cpp include/physics/*.h)
file(GLOB_RECURSE DM_UTILITY_SRC src/utility/*.cpp include/utility/*.h)

set_property(GLOBAL PROPERTY USE_FOLDERS On)
SET_SOURCE_FILES_PROPERTIES( ${DM_SRC} PROPERTIES
                FOLDER "${CMAKE_SOURCE_DIR}/src" )

source_group("Audio"            FILES ${DM_AUDIO_SRC})
source_group("Engine"            FILES ${DM_ENGINE_SRC})
source_group("Editor"            FILES ${DM_EDITOR_SRC})
source_group("Graphics"            FILES ${DM_GRAPHICS_SRC})
source_group("Input"            FILES ${DM_INPUT_SRC})
source_group("Physics"            FILES ${DM_PHYSICS_SRC})
source_group("Python"            FILES ${DM_PYTHON_SRC})
source_group("Utility"            FILES ${DM_UTILITY_SRC})
List(APPEND DM_SRC  ${DM_AUDIO_SRC} ${DM_ENGINE_SRC} ${DM_EDITOR_SRC} ${DM_GRAPHICS_SRC}
${DM_INPUT_SRC} ${DM_PHYSICS_SRC}  ${DM_PYTHON_SRC} ${DM_UTILITY_SRC})

#GLM
set(GLM_DIR ${EXTERNAL_DIR}/glm)
IF(NOT EXISTS ${GLM_DIR})
message(FATAL_ERROR "Please do git submodule update --init --recursive")
ENDIF()
include_directories(${GLM_DIR})

#SFML
set(SFML_DIR "${EXTERNAL_DIR}/SFML")
	set(SFML_BUILD_EXAMPLES FALSE)
	set(BUILD_SHARED_LIBS FALSE)
	set(SFML_STATIC_LIBRARIES TRUE)
	add_subdirectory(${SFML_DIR})
	include_directories(${SFML_DIR}/include ${SFML_DIR}/src)

LIST(APPEND DM_LIBRARIES
	sfml-system
	sfml-window
	sfml-graphics
	sfml-audio
	sfml-network)

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
		
#SFML Imgui
set(SFML_IMGUI ${EXTERNAL_DIR}/imgui-sfml/)
set(IMGUI_ROOT ${EXTERNAL_DIR}/imgui/)

set(SFML_ROOT ${EXTERNAL_DIR}/SFML/)
add_subdirectory(${SFML_IMGUI})
include_directories(imgui/${IMGUI_ROOT} ${SFML_IMGUI})
LIST(APPEND DM_LIBRARIES
	imgui-sfml)

#COMMON
LIST(APPEND DM_LIBRARIES)
if(UNIX AND NOT APPLE)
	LIST(APPEND DM_LIBRARIES
		stdc++fs)
ENDIF()

include_directories(include/)
include_directories(extensions/include/)

file(GLOB_RECURSE DM_EXTENSIONS_SRC extensions/src/*.cpp extensions/include/*.h)

macro(GroupSources curdir)
	file(GLOB children RELATIVE ${CMAKE_SOURCE_DIR}/${curdir} ${CMAKE_SOURCE_DIR}/${curdir}/*)
	foreach(child ${children})
			if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "include\\extensions" "" groupname ${groupname})
				string(REPLACE "src" "" groupname ${groupname})
				string(REPLACE "extensions" "Extension" groupname ${groupname})
				source_group(${groupname} FILES ${CMAKE_SOURCE_DIR}/${curdir}/${child})
			endif()
	endforeach()
endmacro()

# Execute the macro
GroupSources(extensions)
add_library(COMMON STATIC ${DM_SRC} ${DM_EXTENSIONS_SRC})

#COMMON
message("DM LIBRARIES : ${DM_LIBRARIES}")
target_link_libraries(COMMON PUBLIC ${DM_LIBRARIES})
set_property(TARGET COMMON PROPERTY CXX_STANDARD 17)

target_link_libraries(COMMON PUBLIC Vulkan::Vulkan)

target_compile_definitions(COMMON PRIVATE VK_USE_PLATFORM_WIN32_KHR)
target_compile_definitions(COMMON PUBLIC USE_SFML2=1)

#TEST
SET(DM_TEST_DIR ${CMAKE_SOURCE_DIR}/tests)
file(GLOB TEST_FILES ${DM_TEST_DIR}/*.cpp )
add_executable(DM_TEST ${TEST_FILES} )
target_link_libraries(DM_TEST gtest gtest_main COMMON)
if(APPLE )
	set_target_properties(DM_TEST PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
		ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
ENDIF()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fvisibility=hidden" )
elseif()
	SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fvisibility=hidden -masm=intel" )
endif()
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")

#DM
add_executable(DM src/main.cpp)

target_link_libraries(DM PUBLIC COMMON)
set_property(TARGET DM PROPERTY CXX_STANDARD 17)
if(APPLE)
	set_target_properties(DM PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
		RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
ENDIF()

#source
add_custom_command(TARGET COMMON POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/sources ${CMAKE_BINARY_DIR}/sources)
file(COPY sources/ DESTINATION ${CMAKE_BINARY_DIR}/sources/)

#shaders
add_custom_command(TARGET COMMON POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders)
file(COPY shaders/ DESTINATION ${CMAKE_BINARY_DIR}/shaders/)