include("${CMAKE_CURRENT_LIST_DIR}/cmake/global-flags.cmake")
project(Driver)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(WDK REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")

wdk_add_driver(${CMAKE_PROJECT_NAME} ${SOURCES})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "thirdparty/Zydis/include")
target_link_libraries(${CMAKE_PROJECT_NAME} "${CMAKE_CURRENT_LIST_DIR}/thirdparty/Zydis/lib/Zydis.lib")

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "thirdparty/kCaveHook/include")
target_link_libraries(${CMAKE_PROJECT_NAME} "${CMAKE_CURRENT_LIST_DIR}/thirdparty/kCaveHook/lib/kCaveHook.lib")
