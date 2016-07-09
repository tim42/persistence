##
## CMAKE file for neam/nsched test sample
##

cmake_minimum_required(VERSION 2.8)

# set the name of the sample
set(SAMPLE_NAME "s-verbose_${PROJ_APP}")

# avoid listing all the files
file(GLOB_RECURSE srcs ./*.cpp ./*.hpp)

add_definitions(${PROJ_FLAGS})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${PROJ_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${PROJ_FLAGS}")

add_executable(${SAMPLE_NAME} ${srcs})
target_link_libraries(${SAMPLE_NAME} ${PROJ_APP})


