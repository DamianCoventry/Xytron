#ifndef INCLUDED_DIBRESOURCES
#define INCLUDED_DIBRESOURCES

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>

#include <string>
#include <map>

namespace MyTest
{

class DibResources
{
public:
    DibResources() {}
    ~DibResources() { UnloadAll(); }

    HBITMAP Load(HDC dc, const std::string& filename);
    void Unload(const std::string& filename);
    void UnloadAll();

    HBITMAP Create(HDC dc, const std::string& filename, const POINT& frame_position, const SIZE& frame_size, const std::string& unique_name);

    HBITMAP GetDibHandle(const std::string& filename) const;
    bool GetDibTime(const std::string& filename, SYSTEMTIME& sys_time) const;

private:
    HBITMAP LoadDib(HDC dc, const std::string& filename, SYSTEMTIME& sys_time, HBITMAP* full_sized_image = NULL) const;
    bool IsNewer(const std::string& filename, SYSTEMTIME& sys_time) const;

    HBITMAP CreateThumbnail(HDC dc, HANDLE texture_handle, const POINT& frame_position, const SIZE& frame_size) const;

    bool GetLastWriteTime(const std::string& filename, SYSTEMTIME& sys_time) const;

private:
    struct DibInfo
    {
        HBITMAP handle_;
        SYSTEMTIME time_;
    };
    typedef std::map<std::string, DibInfo> DibMap;
    DibMap dib_map_;
};

}       // namespace MyTest

#endif  // INCLUDED_DIBRESOURCES
