#include "main.hpp"
#include "icon.hpp"

icon::~icon()
{
    delete[] idirEntry;
    delete[] groupicon;
}

int icon::UpdatePeMainIcon(const char *fpath)
{
    HANDLE res_upd = BeginUpdateResourceA(fpath, false);
    if (res_upd == NULL) {
        ERR2("BeginUpdateResource failed");
        return 1;
    }

    // place each icon-image found in file to image
    for (int i = 0; i < idir.ImageCount; i++) {
        if (!UpdateResource(res_upd, RT_ICON, MAKEINTRESOURCE(i + 1),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (char*)pView + idirEntry[i].Offset,
            idirEntry[i].ImageSize)) {
            ERR2("UpdateResource failed");
            return 1;
        };
    }

    // place groupicon structure
    if (!UpdateResource(res_upd, RT_GROUP_ICON, "MAINICON",
    MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), groupicon, groupIconSz)) {
        ERR2("UpdateResource failed");
        return 1;
    }

    EndUpdateResource(res_upd, false);
    CloseHandle(res_upd);

    return 0;
}

icon::icon(std::string path)
: mm_file(path, true)
{
    // read icon header
    std::string ErrMsg = "Wrong icon format";
    if (read(&idir, 0, sizeof(idir)) != sizeof(idir)) {
        ERR("read icon directory");
        throw std::runtime_error(ErrMsg);
    }

    if (idir.ResourceType != 1) {
        ERR("type is not icon");
        throw std::logic_error(ErrMsg);
    }
    INFO("number of incons in file: " << idir.ImageCount);
    
    // read icond directory entries
    idirEntry = new IconDirEntry[idir.ImageCount];
    try {

        int size = sizeof(IconDirEntry) * idir.ImageCount;
        if (read(idirEntry, sizeof(idir), size) != size) {
            ERR("read icon dir entries failed");
            throw std::runtime_error(ErrMsg);
        }

    } catch (std::exception &ex) {
        delete[] idirEntry;
        throw;
    }

    // form groupicon structure
    int  dirSz = sizeof(IconDir);
    int resEntriesSz = sizeof(IconDirResEntry) * idir.ImageCount;
    groupIconSz = dirSz + resEntriesSz;
    groupicon = new char[groupIconSz];

    memmove(groupicon, &idir, dirSz); // place dir 
    char *giPos = (char*)groupicon + dirSz; // place entries after directory
    for (int i = 0; i < idir.ImageCount; i++) {
        IconDirResEntry ires;
        // all except last field
        memmove(&ires, &idirEntry[i], sizeof(IconDirResEntry) - sizeof(WORD));
        ires.ResourceID = i + 1;
        memmove(giPos, &ires, sizeof(ires));
        giPos += sizeof(ires); 
    }
}