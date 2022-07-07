#include "file.hpp"

#pragma pack(push, 1)
struct IconDirEntry {
    BYTE Width;
    BYTE Height;
    BYTE Colors;
    BYTE Reserved;
    WORD Planes;
    WORD BitsPerPixel;
    DWORD ImageSize;
    DWORD Offset;
};

struct IconDir {
    WORD Reserved;
    WORD ResourceType;
    WORD ImageCount;
};

struct IconDirResEntry
{
   BYTE Width;
   BYTE Height;
   BYTE Colors;
   BYTE Reserved;
   WORD Planes;
   WORD BitsPerPixel;
   DWORD ImageSize;
   WORD ResourceID;
};

#pragma pack(pop)

class icon : public mm_file {
    IconDirEntry *idirEntry;
    IconDir idir;

    int groupIconSz;
    char *groupicon;   // formed of IconDir header and
                    // IconDirResEntry entries

public:
    icon(std::string path);
    
    int UpdatePeMainIcon(const char *fpath);

    ~icon();

};