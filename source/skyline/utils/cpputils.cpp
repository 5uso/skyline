#include "skyline/utils/cpputils.hpp"

#include "nn/nn.h"
#include "skyline/utils/utils.h"

namespace skyline {

std::string utils::g_RomMountStr = "rom:/";

u64 utils::g_MainTextAddr;
u64 utils::g_MainRodataAddr;
u64 utils::g_MainDataAddr;
u64 utils::g_MainBssAddr;
u64 utils::g_MainHeapAddr;

void utils::init() {
    // find .text
    utils::g_MainTextAddr =
        memGetMapAddr((u64)nninitStartup);  // nninitStartup can be reasonably assumed to be exported by main
    // find .rodata
    utils::g_MainRodataAddr = memNextMap(utils::g_MainTextAddr);
    // find .data
    utils::g_MainDataAddr = memNextMap(utils::g_MainRodataAddr);
    // find .bss
    utils::g_MainBssAddr = memNextMap(utils::g_MainDataAddr);
    // find heap
    utils::g_MainHeapAddr = memNextMapOfType(utils::g_MainBssAddr, MemType_Heap);

}

bool endsWith(std::string const& str1, std::string const& str2) {
    return str2.size() <= str1.size() && str1.find(str2, str1.size() - str2.size()) != str1.npos;
}

//Reimplemented with nn::Result class
Result utils::walkDirectory(std::string const& root,
                            std::function<void(nn::fs::DirectoryEntry const&, std::shared_ptr<std::string>)> callback,
                            bool recursive) {
    Result r;

    nn::fs::DirectoryHandle rootHandle;
    nn::Result temp_res = nn::fs::OpenDirectory(&rootHandle, root.c_str(), nn::fs::OpenDirectoryMode_All);
    if (temp_res.IsFailure()) {
        return temp_res.GetInnerValueForDebug(); 
    }

    s64 entryCount;
    r = nn::fs::GetDirectoryEntryCount(&entryCount, rootHandle).GetInnerValueForDebug();
    if (R_FAILED(r)) {
        nn::fs::CloseDirectory(rootHandle);
        return r;
    }

    nn::fs::DirectoryEntry* entryBuffer = new nn::fs::DirectoryEntry[entryCount];
    r = nn::fs::ReadDirectory(&entryCount, entryBuffer, rootHandle, entryCount).GetInnerValueForDebug();
    nn::fs::CloseDirectory(rootHandle);

    if (R_FAILED(r)) goto exit;

    for (int i = 0; i < entryCount; i++) {
        nn::fs::DirectoryEntry& entry = entryBuffer[i];

        std::string entryStr(entry.name);
        std::string fullPath = root;

        while (endsWith(fullPath, "/")) fullPath.pop_back();
        fullPath += "/";
        fullPath += entryStr;

        if (entry.type == nn::fs::DirectoryEntryType_Directory && recursive) r = walkDirectory(fullPath, callback);

        if (R_FAILED(r)) goto exit;

        callback(entry, std::make_shared<std::string>(fullPath));
    }

exit:
    delete[] entryBuffer;
    return r;
}

//Reimplemented with nn::Result class
Result utils::readFile(std::string const& str, s64 offset, void* data, size_t length) {
    if (data == NULL) return -1;
    nn::fs::FileHandle handle;
    nn::Result temp_res = nn::fs::OpenFile(&handle, str.c_str(), nn::fs::OpenMode_Read);
    if (temp_res.IsFailure()) {
        return temp_res.GetInnerValueForDebug(); 
    }

    Result r;
    s64 fileSize;
    r = nn::fs::GetFileSize(&fileSize, handle).GetInnerValueForDebug();
    if (R_FAILED(r)) {
        nn::fs::CloseFile(handle);
        return r;
    }

    length = MIN((s64)length, fileSize);

    r = nn::fs::ReadFile(handle, offset, data, length).GetInnerValueForDebug();

    nn::fs::CloseFile(handle);
    return r;
}

void* utils::getRegionAddress(skyline::utils::region region) {
    switch (region) {
        case region::Text:
            return (void*)g_MainTextAddr;
        case region::Rodata:
            return (void*)g_MainRodataAddr;
        case region::Data:
            return (void*)g_MainDataAddr;
        case region::Bss:
            return (void*)g_MainBssAddr;
        case region::Heap:
            return (void*)g_MainHeapAddr;
        default:
            return NULL;
    }
}
};  // namespace skyline
