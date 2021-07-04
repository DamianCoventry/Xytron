#ifndef INCLUDED_CHOOSEIMAGEDIALOG
#define INCLUDED_CHOOSEIMAGEDIALOG

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include "../sagedocuments/imagedocument.h"
#include "../sagedocuments/imageanimationdocument.h"

namespace MyTest
{
class DibResources;
}

class ChooseImageDialog
{
public:
    ChooseImageDialog();
    ~ChooseImageDialog();

    void SetImageAnimationDocumentMap(ImageAnimationDocumentMap* image_anim_map)    { image_anim_map_ = image_anim_map; }
    void SetImageDocumentMap(ImageDocumentMap* image_map)                           { image_map_ = image_map; }
    void DibResources(MyTest::DibResources* dib_resources)                          { dib_resources_ = dib_resources; }

    void ImageFilename(const std::string& image_filename)   { image_filename_ = image_filename; }
    const std::string& ImageFilename() const                { return image_filename_; }

    bool Run(HINSTANCE instance, HWND parent);

private:
    friend INT_PTR CALLBACK ChooseImageDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnButtonOK();
    void OnListboxMeasureItem(MEASUREITEMSTRUCT* measure_item);
    void OnListboxDrawItem(DRAWITEMSTRUCT* draw_item);

private:
    HWND dialog_;
    HFONT od_font_;
    std::string image_filename_;
    ImageAnimationDocumentMap* image_anim_map_;
    ImageDocumentMap* image_map_;
    MyTest::DibResources* dib_resources_;
};

#endif  // INCLUDED_CHOOSEIMAGEDIALOG
