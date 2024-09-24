include("${CMAKE_CURRENT_LIST_DIR}/cmake/global-flags.cmake")
project(Cheat)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(Cheat ${SOURCES})
