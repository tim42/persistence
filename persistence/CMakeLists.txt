

include_directories(./persistence/)

file(GLOB PROJ_SOURCES *.cpp)

add_definitions(${PROJ_FLAGS} "-fno-whole-program")

add_library(${PROJ_APP} STATIC ${PROJ_SOURCES})

