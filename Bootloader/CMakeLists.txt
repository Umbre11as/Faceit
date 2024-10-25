include("${CMAKE_CURRENT_LIST_DIR}/cmake/flags.cmake")
project(Bootloader LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(Bootloader ${SOURCES})
set_target_properties(Bootloader PROPERTIES SUFFIX ".efi")

target_include_directories(Bootloader PRIVATE VisualUefi/edk2/CryptoPkg/Include)
target_include_directories(Bootloader PRIVATE VisualUefi/edk2/ShellPkg/Include)
target_include_directories(Bootloader PRIVATE VisualUefi/edk2/MdePkg/Include)
target_include_directories(Bootloader PRIVATE VisualUefi/edk2/MdePkg/Include/X64)

file(GLOB_RECURSE LIBS "VisualUefi/EDK-II/x64/Release/*.lib")
target_link_libraries(Bootloader PRIVATE ${LIBS})
