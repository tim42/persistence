

include_directories(./persistence/)

file(GLOB PROJ_SOURCES *.cpp)

add_definitions(${PROJ_FLAGS} "-fno-whole-program")

add_library(${PROJ_APP} STATIC ${PROJ_SOURCES})

install(TARGETS ${PROJ_APP} DESTINATION lib/neam)
install(DIRECTORY ./ DESTINATION include/neam/persistence
        FILES_MATCHING PATTERN "*.hpp"
        PATTERN "tools" EXCLUDE        # submodule tools has it own include rules
        PATTERN ".git" EXCLUDE
        PATTERN "cmake" EXCLUDE
        PATTERN "samples" EXCLUDE)

# link tools
set(tools_dest ${NEAM_INSTALL_PREFIX}/include/neam/persistence/)
set(tools_target ${NEAM_INSTALL_PREFIX}/include/neam/tools)
if (UNIX)
  install(CODE "execute_process(COMMAND ln -s \"${tools_target}\" \"${tools_dest}/tools\")")
else()
  install(CODE "file(COPY ${tools_target} DESTINATION ${tools_dest})")
endif()
