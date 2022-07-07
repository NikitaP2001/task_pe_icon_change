#include "file.hpp"

#pragma pack(push, 1)
struct IMPORT_DIRECTORY_TABLE {
    DWORD ImportLookupTable;
    DWORD TimeDataStamp;
    DWORD ForwarderChain;
    DWORD NameRva;
    DWORD ImportAddressTable;
};
#pragma pack(pop)

struct ILT32 {
    union {
        uint32_t ord_num : 16;
        uint32_t rva_name_tbl : 31;
    };
    uint32_t flag : 1;
};

struct ILT64 {
    union {
        uint64_t ord_num : 16;
        uint64_t rva_name_tbl : 31;
    };
    uint64_t : 32;
    uint64_t flag : 1;
};

class pe_file : public mm_file {
    bool isPE32;

    DWORD image_header_offset;
    IMAGE_FILE_HEADER img_hdr;

    DWORD opt_header_offset;
    union {
        IMAGE_OPTIONAL_HEADER32 opt_hdr32;
        IMAGE_OPTIONAL_HEADER64 opt_hdr64;
    };

    DWORD sec_table_offset;
    IMAGE_SECTION_HEADER *sec_table = NULL;

    int get_file_offset(uint64_t rva);

public:

    pe_file(std::string path, bool rd_only = true);

    int print_api_info();

    ~pe_file();

};