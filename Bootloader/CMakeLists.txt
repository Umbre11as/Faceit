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

add_custom_command(
        TARGET Bootloader POST_BUILD
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/debugger/qemu.exe"
        ARGS "-name \"UEFI Debugger\" -drive file=OVMF_CODE-need-smm.fd,if=pflash,format=raw,unit=0,readonly=on -drive file=OVMF_VARS-need-smm.fd,if=pflash,format=raw,unit=1 -drive file=fat:rw:..\\..\\dist\\Release,media=disk,if=virtio,format=raw -drive file=UefiShell.iso,format=raw -m 512 -machine q35,smm=on -nodefaults -vga std -global driver=cfi.pflash01,property=secure,value=on -global ICH9-LPC.disable_s3=1 -device ich9-intel-hda,id=sound0,bus=pcie.0,addr=0x1b -device hda-duplex,id=sound0-codec0,bus=sound0.0,cad=0 -global ICH9-LPC.disable_s3=1 -global ICH9-LPC.disable_s4=1 -soundhw all"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/debugger"
        COMMENT "Debug bootloader with QEMU"
)
