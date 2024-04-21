cmake_minimum_required(VERSION 3.1...3.14)

################################################################################
### User options.
################################################################################

option(BUILD_STATIC "Build static" OFF)

SET(BUILD_SHARED_LIBS TRUE)
if(BUILD_STATIC)
	SET(BUILD_SHARED_LIBS FALSE)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -lpthread -static-libgcc -static-libstdc++")
endif(BUILD_STATIC)

################################################################################
### Project files.
################################################################################

project(NBodySim)

SET(CMAKE_INCLUDE_PATH ${CMAKE_SOURCE_DIR})

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

FILE(GLOB_RECURSE NBodySim_SOURCES "src/*.cpp" "src/*.hpp" "${SPPAR_ROOT}/include/*.hpp")

add_executable(NBodySim ${NBodySim_SOURCES})

################################################################################
### Dependencies.
################################################################################

#sfml
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)
if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	target_link_libraries(NBodySim ${SFML_LIBRARIES})
endif(SFML_FOUND)

if(WIN32)
	FILE(GLOB_RECURSE SFML_DEBUG_DLLS "${SFML_ROOT}/bin/sfml-*-d-2.dll")
	FILE(GLOB_RECURSE SFML_RELEASE_DLLS "${SFML_ROOT}/bin/*-2.dll")
	if(NOT SFML_DEBUG_DLLS STREQUAL "")
		LIST(REMOVE_ITEM SFML_RELEASE_DLLS ${SFML_DEBUG_DLLS})
	endif(NOT SFML_DEBUG_DLLS STREQUAL "")
endif(WIN32)

################################################################################
### Enable C++17 and warnings
################################################################################

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")

if(WIN32)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -lpthread -static-libgcc -static-libstdc++ -mwindows")
elseif(UNIX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -static-libgcc -static-libstdc++")
endif(WIN32)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fexpensive-optimizations")
endif()

#################################################################################
### Installing Program
#################################################################################

install(TARGETS NBodySim RUNTIME DESTINATION bin ARCHIVE DESTINATION bin)
