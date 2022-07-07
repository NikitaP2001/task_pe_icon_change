#include <set>
#include <vector>
#include <iostream>
#include <algorithm>
#include "main.hpp"
#include "pe_file.hpp"

pe_file::pe_file(std::string path, bool rd_only)
: mm_file(path, rd_only)
{
    std::string pe_read_err = "pe-image format error";

    // read coff header offset at 3c
    DWORD pe_sig_offset;
    if (read(&pe_sig_offset, 0x3C, 4) == 4) {
        image_header_offset = pe_sig_offset + 4;
        SUCC("image header file offset: 0x" << std::hex << image_header_offset);
    } else {
        ERR("Unable to read image header offset");
        throw std::logic_error(pe_read_err);
    }

    // read image header at finded offset
    if (read(&img_hdr, image_header_offset, sizeof(img_hdr))
    == sizeof(img_hdr)) {
        SUCC("read image header");
    } else {
        ERR("Unable to read image header");
        throw std::logic_error(pe_read_err);
    }

    // right after image header
    opt_header_offset = image_header_offset + sizeof(img_hdr);

    // get pe type
    uint16_t Magic;
    if (read(&Magic, opt_header_offset, sizeof(Magic)) == sizeof(Magic)) {
        if (Magic == 0x10b)
            isPE32 = true;
        else if (Magic == 0x20b)
            isPE32 = false;
        else
            throw std::logic_error(pe_read_err);
        SUCC("exec type is: " << (char*)((isPE32) ? "PE32" : "PE64"));
    } else
        throw std::logic_error(pe_read_err);
    
    // read optional header
    void *pOptStruc = (isPE32) ? (void*)&opt_hdr32 : (void*)&opt_hdr64;
    if (read(pOptStruc, opt_header_offset, 
    img_hdr.SizeOfOptionalHeader) == img_hdr.SizeOfOptionalHeader) {
        SUCC("read image optional header");
    } else {
        ERR("Unable to read image optional header");
        throw std::logic_error(pe_read_err);
    }
    sec_table_offset = opt_header_offset + img_hdr.SizeOfOptionalHeader;

    // read section table 
    try {
        sec_table = new IMAGE_SECTION_HEADER[img_hdr.NumberOfSections];
        int tableSz = img_hdr.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
        if (read(sec_table, sec_table_offset, tableSz)
        == tableSz) {
            SUCC("read section table");
        } else {
            ERR("Unable to read section table");
            throw std::logic_error(pe_read_err);
        }
    } catch (std::exception &ex) {
        if (sec_table != NULL)
            delete[] sec_table;
        throw;
    }
}

pe_file::~pe_file()
{
    delete[] sec_table;
}

int pe_file::get_file_offset(uint64_t rva)
{
    int i;
    IMAGE_SECTION_HEADER *phdr = sec_table;
    // where within sections lay out rva ?
    for (i = 0; i < img_hdr.NumberOfSections; i++, phdr++) {
        DWORD sva = phdr->VirtualAddress;
        DWORD srs = phdr->SizeOfRawData;
        if (rva >= sva && rva < sva + srs) // if rva is laying in vm only
            break;                         // function will return -1, as expected
    }
    // rva does not lay within raw section data
    if (i == img_hdr.NumberOfSections)
        return -1;

    return rva - phdr->VirtualAddress + phdr->PointerToRawData; 
}

/* i beleive winapi means only this 
 * native libs */
std::set<std::string> windll = {
    "NTDLL.DLL",
    "KERNEL32.DLL",
    "GDI32.DLL",
    "USER32.DLL",
    "COMCTL32.DLL",
    "COMDLG32.DLL",
    "WS2_32.DLL",
    "ADVAPI32.DLL",
    "NETAPI32.DLL",
    "OLE32.DLL",
};

/* return number of imported winapi with W on success,
    -1 on failture */
int pe_file::print_api_info()
{
    int num_dirs;
    IMAGE_DATA_DIRECTORY *pdirs;
    num_dirs = (isPE32) ? opt_hdr32.NumberOfRvaAndSizes
    : opt_hdr64.NumberOfRvaAndSizes;
    if (isPE32) {
        num_dirs = opt_hdr32.NumberOfRvaAndSizes;
        pdirs = &opt_hdr32.DataDirectory[0];
    } else {
        num_dirs = opt_hdr64.NumberOfRvaAndSizes;
        pdirs = &opt_hdr64.DataDirectory[0];
    }

    if (num_dirs < 2) {
        ERR("No import dir found");
        return -1;
    }

    IMAGE_DATA_DIRECTORY idir = pdirs[1];
    if (idir.Size == 0) {
        INFO("Import table is empty");
        return 0;
    }

    int idirOffset = get_file_offset(idir.VirtualAddress);
    if (idirOffset == -1) {
        ERR("get import directory file offset");
        return -1;
    }

    // read import tables
    std::vector<IMPORT_DIRECTORY_TABLE> itables;
    do {
        IMPORT_DIRECTORY_TABLE table;
        if (read(&table, idirOffset, sizeof(table)) != sizeof(table)) {
            ERR("import table read");
            return -1;
        }
        // last table 0-filled
        if (table.ImportLookupTable == 0)
            break;
        idirOffset += sizeof(table);
        itables.push_back(table);
    } while (1);

    // read import info from each table entry
    int AmountNativeApisWithW = 0;
    for (auto &tb : itables) {
        int nameOffset = get_file_offset(tb.NameRva);
        if (nameOffset == -1) {
            ERR("get import name file offset");
            return -1;
        }

        // read dll name
        std::string iname = read_cstr(nameOffset);
        if (iname.size() == 0) {
            ERR("fail to read import name");
            return -1;
        }
        
        std::transform(iname.begin(), iname.end(), iname.begin(), ::toupper);
        std::cout << iname << std::endl;

        // next info will be printed only for native apis
        if (windll.find(iname) == windll.end())
            continue;

        int iltOffset = get_file_offset(tb.ImportLookupTable);
        if (iltOffset == -1) {
            ERR("get ilt offset");
            return -1;
        }

        auto getNameTblRva = [this](int offset) -> int
        {
            if (isPE32) {
                ILT32 ilt;
                if (read(&ilt, offset, sizeof(ilt)) != sizeof(ilt))
                    return 0;
                if (ilt.flag)   // import by ordinal
                    return 0;
                return ilt.rva_name_tbl;
            } else {
                ILT64 ilt;
                if (read(&ilt, offset, sizeof(ilt)) != sizeof(ilt))
                    return 0;
                if (ilt.flag)   
                    return 0;
                return ilt.rva_name_tbl;
            }
        };

        DWORD nameTblRva;
        while (nameTblRva = getNameTblRva(iltOffset)) {
            iltOffset += (isPE32) ? 4 : 8;
            int nameTblOffset = get_file_offset(nameTblRva);
            if (iltOffset == -1) {
                ERR("get name table offset");
                return -1;
            }
            std::string apiName = read_cstr(nameTblOffset + 2);
            if (apiName.size() == 0) {
                ERR("failed to read winapi");
                return -1;
            }
            std::cout << std::string(4, ' ') << apiName << std::endl;

            if (apiName.find('W') != std::string::npos)
                AmountNativeApisWithW += 1;
        }

        
    }

    return AmountNativeApisWithW;
}
