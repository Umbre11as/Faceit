// AKA ntimage.h
#pragma once

#include "Types.h"

// Macros from ntdef.h
#define FIELD_OFFSET(type, field)    ((LONG)(LONG_PTR)&(((type *)0)->field))
#define UFIELD_OFFSET(type, field)    ((ULONG)(LONG_PTR)&(((type *)0)->field))
#define RTL_FIELD_SIZE(type, field) (sizeof(((type *)0)->field))
#define RTL_SIZEOF_THROUGH_FIELD(type, field) \
    (FIELD_OFFSET(type, field) + RTL_FIELD_SIZE(type, field))
#define RTL_CONTAINS_FIELD(Struct, Size, Field) \
    ( (((PCHAR)(&(Struct)->Field)) + sizeof((Struct)->Field)) <= (((PCHAR)(Struct))+(Size)) )
#define RTL_NUMBER_OF_V1(A) (sizeof(A)/sizeof((A)[0]))

#ifndef LONG_PTR
#define LONG_PTR long long
#endif

#ifndef ULONG_PTR
#define ULONG_PTR QWORD
#endif

#define IMAGE_DOS_SIGNATURE 0x5A4D      // MZ
#define IMAGE_OS2_SIGNATURE 0x454E      // NE
#define IMAGE_OS2_SIGNATURE_LE 0x454C   // LE
#define IMAGE_VXD_SIGNATURE 0x454C      // LE
#define IMAGE_NT_SIGNATURE 0x00004550   // PE00

#define IMAGE_DOS_SIGNATURE 0x4D5A      // MZ
#define IMAGE_OS2_SIGNATURE 0x4E45      // NE
#define IMAGE_OS2_SIGNATURE_LE 0x4C45   // LE
#define IMAGE_NT_SIGNATURE 0x50450000   // PE00

typedef struct _IMAGE_DOS_HEADER {      // DOS .EXE header
    USHORT e_magic;                     // Magic number
    USHORT e_cblp;                      // Bytes on last page of file
    USHORT e_cp;                        // Pages in file
    USHORT e_crlc;                      // Relocations
    USHORT e_cparhdr;                   // Size of header in paragraphs
    USHORT e_minalloc;                  // Minimum extra paragraphs needed
    USHORT e_maxalloc;                  // Maximum extra paragraphs needed
    USHORT e_ss;                        // Initial (relative) SS value
    USHORT e_sp;                        // Initial SP value
    USHORT e_csum;                      // Checksum
    USHORT e_ip;                        // Initial IP value
    USHORT e_cs;                        // Initial (relative) CS value
    USHORT e_lfarlc;                    // File address of relocation table
    USHORT e_ovno;                      // Overlay number
    USHORT e_res[4];                    // Reserved words
    USHORT e_oemid;                     // OEM identifier (for e_oeminfo)
    USHORT e_oeminfo;                   // OEM information; e_oemid specific
    USHORT e_res2[10];                  // Reserved words
    LONG   e_lfanew;                    // File address of new exe header
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_FILE_HEADER {
    USHORT  Machine;
    USHORT  NumberOfSections;
    ULONG   TimeDateStamp;
    ULONG   PointerToSymbolTable;
    ULONG   NumberOfSymbols;
    USHORT  SizeOfOptionalHeader;
    USHORT  Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

#define IMAGE_SIZEOF_FILE_HEADER             20

#define IMAGE_FILE_RELOCS_STRIPPED           0x0001  // Relocation info stripped from file.
#define IMAGE_FILE_EXECUTABLE_IMAGE          0x0002  // File is executable  (i.e. no unresolved external references).
#define IMAGE_FILE_LINE_NUMS_STRIPPED        0x0004  // Line nunbers stripped from file.
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED       0x0008  // Local symbols stripped from file.
#define IMAGE_FILE_AGGRESIVE_WS_TRIM         0x0010  // Aggressively trim working set
#define IMAGE_FILE_LARGE_ADDRESS_AWARE       0x0020  // App can handle >2gb addresses
#define IMAGE_FILE_BYTES_REVERSED_LO         0x0080  // Bytes of machine word are reversed.
#define IMAGE_FILE_32BIT_MACHINE             0x0100  // 32 bit word machine.
#define IMAGE_FILE_DEBUG_STRIPPED            0x0200  // Debugging info stripped from file in .DBG file
#define IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP   0x0400  // If Image is on removable media, copy and run from the swap file.
#define IMAGE_FILE_NET_RUN_FROM_SWAP         0x0800  // If Image is on Net, copy and run from the swap file.
#define IMAGE_FILE_SYSTEM                    0x1000  // System File.
#define IMAGE_FILE_DLL                       0x2000  // File is a DLL.
#define IMAGE_FILE_UP_SYSTEM_ONLY            0x4000  // File should only be run on a UP machine
#define IMAGE_FILE_BYTES_REVERSED_HI         0x8000  // Bytes of machine word are reversed.

#define IMAGE_FILE_MACHINE_UNKNOWN           0
#define IMAGE_FILE_MACHINE_TARGET_HOST       0x0001  // Useful for indicating we want to interact with the host and not a WoW guest.
#define IMAGE_FILE_MACHINE_I386              0x014c  // Intel 386.
#define IMAGE_FILE_MACHINE_R3000             0x0162  // MIPS little-endian, 0x160 big-endian
#define IMAGE_FILE_MACHINE_R4000             0x0166  // MIPS little-endian
#define IMAGE_FILE_MACHINE_R10000            0x0168  // MIPS little-endian
#define IMAGE_FILE_MACHINE_WCEMIPSV2         0x0169  // MIPS little-endian WCE v2
#define IMAGE_FILE_MACHINE_ALPHA             0x0184  // Alpha_AXP
#define IMAGE_FILE_MACHINE_SH3               0x01a2  // SH3 little-endian
#define IMAGE_FILE_MACHINE_SH3DSP            0x01a3
#define IMAGE_FILE_MACHINE_SH3E              0x01a4  // SH3E little-endian
#define IMAGE_FILE_MACHINE_SH4               0x01a6  // SH4 little-endian
#define IMAGE_FILE_MACHINE_SH5               0x01a8  // SH5
#define IMAGE_FILE_MACHINE_ARM               0x01c0  // ARM Little-Endian
#define IMAGE_FILE_MACHINE_THUMB             0x01c2  // ARM Thumb/Thumb-2 Little-Endian
#define IMAGE_FILE_MACHINE_ARMNT             0x01c4  // ARM Thumb-2 Little-Endian
#define IMAGE_FILE_MACHINE_AM33              0x01d3
#define IMAGE_FILE_MACHINE_POWERPC           0x01F0  // IBM PowerPC Little-Endian
#define IMAGE_FILE_MACHINE_POWERPCFP         0x01f1
#define IMAGE_FILE_MACHINE_IA64              0x0200  // Intel 64
#define IMAGE_FILE_MACHINE_MIPS16            0x0266  // MIPS
#define IMAGE_FILE_MACHINE_ALPHA64           0x0284  // ALPHA64
#define IMAGE_FILE_MACHINE_MIPSFPU           0x0366  // MIPS
#define IMAGE_FILE_MACHINE_MIPSFPU16         0x0466  // MIPS
#define IMAGE_FILE_MACHINE_AXP64             IMAGE_FILE_MACHINE_ALPHA64
#define IMAGE_FILE_MACHINE_TRICORE           0x0520  // Infineon
#define IMAGE_FILE_MACHINE_CEF               0x0CEF
#define IMAGE_FILE_MACHINE_EBC               0x0EBC  // EFI Byte Code
#define IMAGE_FILE_MACHINE_AMD64             0x8664  // AMD64 (K8)
#define IMAGE_FILE_MACHINE_M32R              0x9041  // M32R little-endian
#define IMAGE_FILE_MACHINE_ARM64             0xAA64  // ARM64 Little-Endian
#define IMAGE_FILE_MACHINE_CEE               0xC0EE

#define IMAGE_FILE_MACHINE_CHPE_X86          0x3A64
#define IMAGE_FILE_MACHINE_ARM64EC           0xA641
#define IMAGE_FILE_MACHINE_ARM64X            0xA64E

typedef struct _IMAGE_DATA_DIRECTORY {
    ULONG   VirtualAddress;
    ULONG   Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

typedef struct _IMAGE_OPTIONAL_HEADER {
    USHORT  Magic;
    UCHAR   MajorLinkerVersion;
    UCHAR   MinorLinkerVersion;
    ULONG   SizeOfCode;
    ULONG   SizeOfInitializedData;
    ULONG   SizeOfUninitializedData;
    ULONG   AddressOfEntryPoint;
    ULONG   BaseOfCode;
    ULONG   BaseOfData;
    ULONG   ImageBase;
    ULONG   SectionAlignment;
    ULONG   FileAlignment;
    USHORT  MajorOperatingSystemVersion;
    USHORT  MinorOperatingSystemVersion;
    USHORT  MajorImageVersion;
    USHORT  MinorImageVersion;
    USHORT  MajorSubsystemVersion;
    USHORT  MinorSubsystemVersion;
    ULONG   Win32VersionValue;
    ULONG   SizeOfImage;
    ULONG   SizeOfHeaders;
    ULONG   CheckSum;
    USHORT  Subsystem;
    USHORT  DllCharacteristics;
    ULONG   SizeOfStackReserve;
    ULONG   SizeOfStackCommit;
    ULONG   SizeOfHeapReserve;
    ULONG   SizeOfHeapCommit;
    ULONG   LoaderFlags;
    ULONG   NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

typedef struct _IMAGE_ROM_OPTIONAL_HEADER {
    USHORT Magic;
    UCHAR  MajorLinkerVersion;
    UCHAR  MinorLinkerVersion;
    ULONG  SizeOfCode;
    ULONG  SizeOfInitializedData;
    ULONG  SizeOfUninitializedData;
    ULONG  AddressOfEntryPoint;
    ULONG  BaseOfCode;
    ULONG  BaseOfData;
    ULONG  BaseOfBss;
    ULONG  GprMask;
    ULONG  CprMask[4];
    ULONG  GpValue;
} IMAGE_ROM_OPTIONAL_HEADER, *PIMAGE_ROM_OPTIONAL_HEADER;

typedef struct _IMAGE_OPTIONAL_HEADER64 {
    USHORT      Magic;
    UCHAR       MajorLinkerVersion;
    UCHAR       MinorLinkerVersion;
    ULONG       SizeOfCode;
    ULONG       SizeOfInitializedData;
    ULONG       SizeOfUninitializedData;
    ULONG       AddressOfEntryPoint;
    ULONG       BaseOfCode;
    ULONGLONG   ImageBase;
    ULONG       SectionAlignment;
    ULONG       FileAlignment;
    USHORT      MajorOperatingSystemVersion;
    USHORT      MinorOperatingSystemVersion;
    USHORT      MajorImageVersion;
    USHORT      MinorImageVersion;
    USHORT      MajorSubsystemVersion;
    USHORT      MinorSubsystemVersion;
    ULONG       Win32VersionValue;
    ULONG       SizeOfImage;
    ULONG       SizeOfHeaders;
    ULONG       CheckSum;
    USHORT      Subsystem;
    USHORT      DllCharacteristics;
    ULONGLONG   SizeOfStackReserve;
    ULONGLONG   SizeOfStackCommit;
    ULONGLONG   SizeOfHeapReserve;
    ULONGLONG   SizeOfHeapCommit;
    ULONG       LoaderFlags;
    ULONG       NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;

#define IMAGE_NT_OPTIONAL_HDR32_MAGIC 0x10b
#define IMAGE_NT_OPTIONAL_HDR64_MAGIC 0x20b
#define IMAGE_ROM_OPTIONAL_HDR_MAGIC 0x107

typedef IMAGE_OPTIONAL_HEADER64             IMAGE_OPTIONAL_HEADER;
typedef PIMAGE_OPTIONAL_HEADER64            PIMAGE_OPTIONAL_HEADER;
#define IMAGE_NT_OPTIONAL_HDR_MAGIC         IMAGE_NT_OPTIONAL_HDR64_MAGIC

typedef struct _IMAGE_NT_HEADERS64 {
    ULONG Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;

typedef struct _IMAGE_NT_HEADERS {
    ULONG Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;

typedef struct _IMAGE_ROM_HEADERS {
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_ROM_OPTIONAL_HEADER OptionalHeader;
} IMAGE_ROM_HEADERS, *PIMAGE_ROM_HEADERS;

typedef IMAGE_NT_HEADERS64                  IMAGE_NT_HEADERS;
typedef PIMAGE_NT_HEADERS64                 PIMAGE_NT_HEADERS;

#define IMAGE_SIZEOF_SHORT_NAME              8

typedef struct _IMAGE_SECTION_HEADER {
    UCHAR   Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
        ULONG   PhysicalAddress;
        ULONG   VirtualSize;
    } Misc;
    ULONG   VirtualAddress;
    ULONG   SizeOfRawData;
    ULONG   PointerToRawData;
    ULONG   PointerToRelocations;
    ULONG   PointerToLinenumbers;
    USHORT  NumberOfRelocations;
    USHORT  NumberOfLinenumbers;
    ULONG   Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

#define IMAGE_REL_BASED_ABSOLUTE              0
#define IMAGE_REL_BASED_HIGH                  1
#define IMAGE_REL_BASED_LOW                   2
#define IMAGE_REL_BASED_HIGHLOW               3
#define IMAGE_REL_BASED_HIGHADJ               4
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_5    5
#define IMAGE_REL_BASED_RESERVED              6
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_7    7
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_8    8
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_9    9
#define IMAGE_REL_BASED_DIR64                 10

#define IMAGE_REL_BASED_IA64_IMM64            9

#define IMAGE_REL_BASED_MIPS_JMPADDR          5
#define IMAGE_REL_BASED_MIPS_JMPADDR16        9

#define IMAGE_REL_BASED_ARM_MOV32             5
#define IMAGE_REL_BASED_THUMB_MOV32           7

#define IMAGE_FIRST_SECTION( ntheader ) ((PIMAGE_SECTION_HEADER)        \
    ((ULONG_PTR)(ntheader) +                                            \
     FIELD_OFFSET( IMAGE_NT_HEADERS, OptionalHeader ) +                 \
     ((ntheader))->FileHeader.SizeOfOptionalHeader   \
    ))

#define IMAGE_SUBSYSTEM_UNKNOWN              0   // Unknown subsystem.
#define IMAGE_SUBSYSTEM_NATIVE               1   // Image doesn't require a subsystem.
#define IMAGE_SUBSYSTEM_WINDOWS_GUI          2   // Image runs in the Windows GUI subsystem.
#define IMAGE_SUBSYSTEM_WINDOWS_CUI          3   // Image runs in the Windows character subsystem.

#define IMAGE_SUBSYSTEM_OS2_CUI              5   // image runs in the OS/2 character subsystem.
#define IMAGE_SUBSYSTEM_POSIX_CUI            7   // image runs in the Posix character subsystem.
#define IMAGE_SUBSYSTEM_NATIVE_WINDOWS       8   // image is a native Win9x driver.
#define IMAGE_SUBSYSTEM_WINDOWS_CE_GUI       9   // Image runs in the Windows CE subsystem.
#define IMAGE_SUBSYSTEM_EFI_APPLICATION      10  // UEFI Application
#define IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER  11   //
#define IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER   12  // UEFI DXE Driver
#define IMAGE_SUBSYSTEM_EFI_ROM              13
#define IMAGE_SUBSYSTEM_XBOX                 14
#define IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION 16
#define IMAGE_SUBSYSTEM_XBOX_CODE_CATALOG    17

#define IMAGE_DIRECTORY_ENTRY_EXPORT          0   // Export Directory
#define IMAGE_DIRECTORY_ENTRY_IMPORT          1   // Import Directory
#define IMAGE_DIRECTORY_ENTRY_RESOURCE        2   // Resource Directory
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION       3   // Exception Directory
#define IMAGE_DIRECTORY_ENTRY_SECURITY        4   // Security Directory
#define IMAGE_DIRECTORY_ENTRY_BASERELOC       5   // Base Relocation Table
#define IMAGE_DIRECTORY_ENTRY_DEBUG           6   // Debug Directory
//      IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7   // (X86 usage)
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7   // Architecture Specific Data
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8   // RVA of GP
#define IMAGE_DIRECTORY_ENTRY_TLS             9   // TLS Directory
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10   // Load Configuration Directory
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11   // Bound Import Directory in headers
#define IMAGE_DIRECTORY_ENTRY_IAT            12   // Import Address Table
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   // Delay Load Import Descriptors
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   // COM Runtime descriptor

#define IMAGE_SIZEOF_SECTION_HEADER          40

typedef struct _IMAGE_RELOCATION {
    union {
        ULONG   VirtualAddress;
        ULONG   RelocCount;             // Set to the real count when IMAGE_SCN_LNK_NRELOC_OVFL is set
    } DUMMYUNIONNAME;
    ULONG   SymbolTableIndex;
    USHORT  Type;
} IMAGE_RELOCATION;
typedef IMAGE_RELOCATION *PIMAGE_RELOCATION;

typedef struct _IMAGE_BASE_RELOCATION {
    ULONG VirtualAddress;
    ULONG SizeOfBlock;
} IMAGE_BASE_RELOCATION;

typedef IMAGE_BASE_RELOCATION *PIMAGE_BASE_RELOCATION;

typedef struct _IMAGE_EXPORT_DIRECTORY {
    ULONG Characteristics;
    ULONG TimeDateStamp;
    USHORT MajorVersion;
    USHORT MinorVersion;
    ULONG Name;
    ULONG Base;
    ULONG NumberOfFunctions;
    ULONG NumberOfNames;
    ULONG AddressOfFunctions;
    ULONG AddressOfNames;
    ULONG AddressOfNameOrdinals;
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;

typedef struct _IMAGE_IMPORT_BY_NAME {
    USHORT Hint;
    CHAR Name [1];
} IMAGE_IMPORT_BY_NAME, *PIMAGE_IMPORT_BY_NAME;

typedef struct _IMAGE_THUNK_DATA64 {
    union {
        ULONGLONG ForwarderString;
        ULONGLONG Function;
        ULONGLONG Ordinal;
        ULONGLONG AddressOfData;
    } u1;
} IMAGE_THUNK_DATA64;

typedef IMAGE_THUNK_DATA64 *PIMAGE_THUNK_DATA64;

typedef struct _IMAGE_THUNK_DATA32 {
    union {
        ULONG ForwarderString;
        ULONG Function;
        ULONG Ordinal;
        ULONG AddressOfData;
    } u1;
} IMAGE_THUNK_DATA32;

typedef IMAGE_THUNK_DATA32 *PIMAGE_THUNK_DATA32;

#define IMAGE_ORDINAL_FLAG64 0x8000000000000000
#define IMAGE_ORDINAL_FLAG32 0x80000000
#define IMAGE_ORDINAL64(Ordinal) (Ordinal & 0xffff)
#define IMAGE_ORDINAL32(Ordinal) (Ordinal & 0xffff)
#define IMAGE_SNAP_BY_ORDINAL64(Ordinal) ((Ordinal & IMAGE_ORDINAL_FLAG64) != 0)
#define IMAGE_SNAP_BY_ORDINAL32(Ordinal) ((Ordinal & IMAGE_ORDINAL_FLAG32) != 0)

typedef struct _IMAGE_IMPORT_DESCRIPTOR {
    union {
        ULONG Characteristics;
        ULONG OriginalFirstThunk;
    };
    ULONG TimeDateStamp;
    ULONG ForwarderChain;
    ULONG Name;
    ULONG FirstThunk;
} IMAGE_IMPORT_DESCRIPTOR;

typedef IMAGE_IMPORT_DESCRIPTOR *PIMAGE_IMPORT_DESCRIPTOR;

typedef struct _IMAGE_RESOURCE_DIRECTORY {
    ULONG   Characteristics;
    ULONG   TimeDateStamp;
    USHORT  MajorVersion;
    USHORT  MinorVersion;
    USHORT  NumberOfNamedEntries;
    USHORT  NumberOfIdEntries;
} IMAGE_RESOURCE_DIRECTORY, *PIMAGE_RESOURCE_DIRECTORY;

#define IMAGE_RESOURCE_NAME_IS_STRING        0x80000000
#define IMAGE_RESOURCE_DATA_IS_DIRECTORY     0x80000000
