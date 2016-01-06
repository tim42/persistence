##
## CMAKE file for neam projects
##

set(PROJ_FLAGS "-march=native -mtune=native")

# general flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(PROJ_FLAGS "${PROJ_FLAGS} -O0 -g3")
else(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(PROJ_FLAGS "${PROJ_FLAGS} -DNDEBUG -fmerge-all-constants")
else(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
  set(PROJ_FLAGS "${PROJ_FLAGS} -DNDEBUG -fmerge-all-constants")
endif()

# some gcc/clang flags
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  set(PROJ_FLAGS "${PROJ_FLAGS} -std=gnu++14 -Wall -Wextra -Wno-unused-function")

  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(PROJ_FLAGS "${PROJ_FLAGS} -fno-inline")
  else(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(PROJ_FLAGS "${PROJ_FLAGS} -O3 -finline-limit=50000 -fwhole-program -fmerge-constants -fmerge-all-constants")
  else(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(PROJ_FLAGS "${PROJ_FLAGS} -Os -fwhole-program -fmerge-constants -fmerge-all-constants")
  endif()

elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  set(PROJ_FLAGS "${PROJ_FLAGS} -std=c++14 -Wall -Wextra -Wno-unused-function")

  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(PROJ_FLAGS "${PROJ_FLAGS}")
  else(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(PROJ_FLAGS "${PROJ_FLAGS} -O3")
  else(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(PROJ_FLAGS "${PROJ_FLAGS} -Oz")
  endif()
endif()



