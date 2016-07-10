##
## CMAKE file for neam/...
##

cmake_minimum_required(VERSION 2.8)


set(PROJ_VERSION_MAJOR 0)
set(PROJ_VERSION_MINOR 0)
set(PROJ_VERSION_SUPERMINOR 2)

add_definitions(-D_PROJ_VERSION_MAJOR=${PROJ_VERSION_MAJOR})
add_definitions(-D_PROJ_VERSION_MINOR=${PROJ_VERSION_MINOR})
add_definitions(-D_PROJ_VERSION_SUPERMINOR=${PROJ_VERSION_SUPERMINOR})

set(PROJ_VERSION_NAME "")

set(PROJ_NAME persistence)

if (PROJ_VERSION_NAME STREQUAL "")
  set(PROJ_APP ${PROJ_NAME}-${PROJ_VERSION_MAJOR}.${PROJ_VERSION_MINOR}.${PROJ_VERSION_SUPERMINOR})
else ()
  set(PROJ_APP ${PROJ_NAME}-${PROJ_VERSION_NAME}-${PROJ_VERSION_MAJOR}.${PROJ_VERSION_MINOR}.${PROJ_VERSION_SUPERMINOR})
endif()

project(${PROJ_APP})

message(STATUS "-----------------------------------------------")
message(STATUS "Building neam/${PROJ_NAME} :: ${PROJ_APP}")
message(STATUS "-----------------------------------------------")


if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release MinSizeRel." FORCE)
endif ()

if (${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
  # And this screw up users.
  # YAY :D
  message(WARNING "when checking: '${CMAKE_CURRENT_BINARY_DIR}' is the same as '${PROJECT_SOURCE_DIR}'")
  message(WARNING "you shhould create a build folder in order to build ${PROJ_APP} !!!")
endif()

# output
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/")
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/libs")

# export the installation prefix
if (${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
  set(NEAM_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
endif()

# deps check
include(cmake/deps.cmake)

# include those third party libs

add_subdirectory("./${PROJ_NAME}/tools")

# create the PROJ_FLAGS variable
include(cmake/flags.cmake)

include_directories("./${PROJ_NAME}")

include_directories("./")

# build a static lib
add_subdirectory(${PROJ_NAME})

# export project name to parent project
if (NOT ${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
  set(lib${PROJ_NAME} ${PROJ_APP} PARENT_SCOPE)
endif()

# build the samples
if (${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
  add_subdirectory(samples/basic)
  add_subdirectory(samples/simple)
  add_subdirectory(samples/advanced)
  add_subdirectory(samples/benchmark)
  add_subdirectory(samples/storage)
  add_subdirectory(samples/unit-test)
endif()

# copy
# file(COPY ./data DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
