

include_directories(./persistence/)

file(GLOB PROJ_SOURCES *.cpp)

add_definitions(${PROJ_FLAGS} "-fno-whole-program")

add_library(${PROJ_APP} STATIC ${PROJ_SOURCES})

install(TARGETS ${PROJ_APP} DESTINATION lib/neam)
install(DIRECTORY ./ DESTINATION include/neam/persistence
        FILES_MATCHING PATTERN "*.hpp"
        PATTERN ".git" EXCLUDE
        PATTERN "cmake" EXCLUDE
        PATTERN "samples" EXCLUDE)
