set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist")

add_compile_options("/W0 /GL- /Ox /Ob1 /Os /EHa") # No warnings, no whole program optimization, optimization favor speed, only __inline, intrinsic functions, favor small code, SEH exceptions support
add_link_options("/ENTRY:DriverEntry")
