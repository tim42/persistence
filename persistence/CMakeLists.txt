

file(GLOB_RECURSE PROJ_SOURCES *.cpp)

add_definitions(${PROJ_FLAGS} "-fno-whole-program")

add_library(${PROJ_APP} STATIC ${PROJ_SOURCES})

