#include "dibresources.h"
#include <boost/algorithm/string.hpp>
#include <windowsx.h>
#include "../gfx/gfx.h"

using namespace MyTest;

HBITMAP DibResources::Load(HDC dc, const std::string& filename)
{
    DibMap::iterator itor = dib_map_.find(filename);
    if(itor == dib_map_.end())
    {
        // Add it.
        DibInfo dib_info;
        dib_info.handle_ = LoadDib(dc, filename, dib_info.time_);
        dib_map_[filename] = dib_info;
        return dib_info.handle_;
    }

    // It already exists.  Has the version on disk been updated since
    // we last loaded it?
    if(IsNewer(filename, itor->second.time_))
    {
        DeleteObject(itor->second.handle_);
        itor->second.handle_ = LoadDib(dc, filename, itor->second.time_);
    }
    return itor->second.handle_;
}

void DibResources::Unload(const std::string& filename)
{
    DibMap::iterator itor = dib_map_.find(filename);
    if(itor != dib_map_.end())
    {
        DeleteObject(itor->second.handle_);
        dib_map_.erase(itor);
    }
}

void DibResources::UnloadAll()
{
    DibMap::iterator itor;
    for(itor = dib_map_.begin(); itor != dib_map_.end(); ++itor)
    {
        DeleteObject(itor->second.handle_);
    }
    dib_map_.clear();
}

HBITMAP DibResources::Create(HDC dc, const std::string& filename, const POINT& frame_position, const SIZE& frame_size, const std::string& unique_name)
{
    SYSTEMTIME sys_time;
    HBITMAP full_sized_image;
    HBITMAP temp_handle = LoadDib(dc, filename, sys_time, &full_sized_image);
    DeleteObject(temp_handle);

    DibMap::iterator thumb_itor = dib_map_.find(unique_name);
    if(thumb_itor == dib_map_.end())
    {
        // Add it.
        dib_map_[unique_name] = DibInfo();
        thumb_itor = dib_map_.find(unique_name);
    }
    else // It already exists
    {
        DeleteObject(thumb_itor->second.handle_);
    }

    thumb_itor->second.handle_ = CreateThumbnail(dc, full_sized_image, frame_position, frame_size);

    DeleteObject(full_sized_image);

    return thumb_itor->second.handle_;
}

HBITMAP DibResources::CreateThumbnail(HDC dc, HANDLE texture_handle, const POINT& frame_position, const SIZE& frame_size) const
{
    BITMAP info;
    GetObject(texture_handle, sizeof(BITMAP), (void*)&info);

    BITMAPINFO bi;
    memset(&bi, 0, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize             = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth            = 64;
    bi.bmiHeader.biHeight           = 64;
    bi.bmiHeader.biPlanes           = 1;
    bi.bmiHeader.biBitCount         = info.bmBitsPixel;
    bi.bmiHeader.biCompression      = BI_RGB;

    unsigned char* dest_bits;
    HBITMAP thumbnail = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, (void**)&dest_bits, NULL, 0);
    if(thumbnail == NULL)
    {
        return NULL;
    }

    HDC thumb_dc = CreateCompatibleDC(dc);
    HDC image_dc = CreateCompatibleDC(dc);
    HBITMAP old_thumbnail_bmp = SelectBitmap(thumb_dc, thumbnail);
    HBITMAP old_texture_bmp = SelectBitmap(image_dc, texture_handle);

    StretchBlt(
        thumb_dc, 0, 0, 64, 64,
        image_dc, frame_position.x, frame_position.y, frame_size.cx, frame_size.cy,
        SRCCOPY);

    SelectBitmap(image_dc, old_texture_bmp);
    SelectBitmap(thumb_dc, old_thumbnail_bmp);
    DeleteDC(image_dc);
    DeleteDC(thumb_dc);

    return thumbnail;
}

HBITMAP DibResources::GetDibHandle(const std::string& filename) const
{
    DibMap::const_iterator itor = dib_map_.find(filename);
    if(itor != dib_map_.end())
    {
        return itor->second.handle_;
    }
    return NULL;
}

bool DibResources::GetDibTime(const std::string& filename, SYSTEMTIME& sys_time) const
{
    DibMap::const_iterator itor = dib_map_.find(filename);
    if(itor != dib_map_.end())
    {
        sys_time = itor->second.time_;
        return true;
    }
    return false;
}

HBITMAP DibResources::LoadDib(HDC dc, const std::string& filename, SYSTEMTIME& sys_time, HBITMAP* full_sized_image) const
{
    int width, height, bpp;
    boost::shared_array<unsigned char> pixels;

    try
    {
        if(boost::algorithm::ifind_first(filename, ".bmp"))
        {
            Gfx::BitmapFile file;
            file.Load(filename);
            width   = file.GetWidth();
            height  = file.GetHeight();
            bpp     = file.GetBpp();
            pixels  = file.Pixels();
        }
        else if(boost::algorithm::ifind_first(filename, ".tga"))
        {
            Gfx::TargaFile file;
            file.Load(filename, false, false);
            width   = file.GetWidth();
            height  = file.GetHeight();
            bpp     = file.GetBpp();
            pixels  = file.Pixels();
        }
        else if(boost::algorithm::ifind_first(filename, ".png"))
        {
            Gfx::PngFile file;
            file.Load(filename, true, false);
            width   = file.GetWidth();
            height  = file.GetHeight();
            bpp     = file.GetBpp();
            pixels  = file.Pixels();
        }
        else
        {
            return NULL;
        }
    }
    catch(std::exception& )
    {
        return NULL;
    }

    // Create a bitmap the same size as the entire image.
    BITMAPINFO bi;
    memset(&bi, 0, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize             = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth            = width;
    bi.bmiHeader.biHeight           = height;
    bi.bmiHeader.biPlanes           = 1;
    bi.bmiHeader.biBitCount         = bpp;
    bi.bmiHeader.biCompression      = BI_RGB;

    unsigned char* dest_bits;
    HBITMAP orig_bitmap = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, (void**)&dest_bits, NULL, 0);
    if(orig_bitmap == NULL)
    {
        return NULL;
    }

    // Copy all the pixels from the file into the bitmap
    unsigned long scanline_length = (bpp >> 3) * width;
    unsigned char* source_bits = pixels.get();
    for(int y = 0; y < height; y++)
    {
        memcpy(dest_bits, source_bits, scanline_length);
        source_bits += scanline_length;
        dest_bits += scanline_length;
    }

    // Now create a 64x64 thumb_bitmap image.
    bi.bmiHeader.biWidth    = 64;
    bi.bmiHeader.biHeight   = 64;

    HBITMAP thumb_bitmap = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, (void**)&dest_bits, NULL, 0);
    if(thumb_bitmap == NULL)
    {
        DeleteObject(orig_bitmap);
        return NULL;
    }

    HDC thumb_dc = CreateCompatibleDC(dc);
    HDC image_dc = CreateCompatibleDC(dc);
    HBITMAP old_thumb_bitmap = SelectBitmap(thumb_dc, thumb_bitmap);
    HBITMAP old_orig_bitmap = SelectBitmap(image_dc, orig_bitmap);

    // Draw a scaled down version of the entire image on the thumb_bitmap image.
    StretchBlt(thumb_dc, 0, 0, 64, 64, image_dc, 0, 0, width, height, SRCCOPY);

    SelectBitmap(image_dc, old_orig_bitmap);
    SelectBitmap(thumb_dc, old_thumb_bitmap);
    DeleteDC(image_dc);
    DeleteDC(thumb_dc);

    if(full_sized_image == NULL)
    {
        DeleteObject(orig_bitmap);
    }
    else
    {
        *full_sized_image = orig_bitmap;
    }

    GetLastWriteTime(filename, sys_time);
    return thumb_bitmap;
}

bool DibResources::IsNewer(const std::string& filename, SYSTEMTIME& sys_time) const
{
    SYSTEMTIME last_write_time;
    if(!GetLastWriteTime(filename, last_write_time))
    {
        return false;
    }
    bool rv = false;

    if(last_write_time.wYear > sys_time.wYear) rv = true;
    else if(last_write_time.wYear == sys_time.wYear)
    {
        if(last_write_time.wMonth > sys_time.wMonth) rv = true;
        else if(last_write_time.wMonth == sys_time.wMonth)
        {
            if(last_write_time.wDay > sys_time.wDay) rv = true;
            else if(last_write_time.wDay == sys_time.wDay)
            {
                if(last_write_time.wHour > sys_time.wHour) rv = true;
                else if(last_write_time.wHour == sys_time.wHour)
                {
                    if(last_write_time.wMinute > sys_time.wMinute) rv = true;
                    else if(last_write_time.wMinute == sys_time.wMinute)
                    {
                        if(last_write_time.wSecond > sys_time.wSecond) rv = true;
                        else if(last_write_time.wSecond == sys_time.wSecond)
                        {
                            if(last_write_time.wMilliseconds > sys_time.wMilliseconds) rv = true;
                        }
                    }
                }
            }
        }
    }

    return rv;
}

bool DibResources::GetLastWriteTime(const std::string& filename, SYSTEMTIME& sys_time) const
{
    HANDLE file = CreateFile(filename.c_str(), 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(file == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    FILETIME lwt;
    if(!GetFileTime(file, NULL, NULL, &lwt))
    {
        CloseHandle(file);
        return false;
    }

    if(!FileTimeToSystemTime(&lwt, &(sys_time)))
    {
        CloseHandle(file);
        return false;
    }

    CloseHandle(file);
    return true;
}
