#ifndef INCLUDED_IMAGEANIMATIONDOCUMENT
#define INCLUDED_IMAGEANIMATIONDOCUMENT

#include <string>
#include <map>

class ImageAnimationDocument
{
public:
    ImageAnimationDocument()
        : start_x_(0), start_y_(0), frame_width_(100), frame_height_(50), frame_count_(16) {}

    void TextureFilename(const std::string& Filename) { TextureFilename_ = Filename; }

    void StartX(int start_x)            { start_x_ = start_x; }
    void StartY(int start_y)            { start_y_ = start_y; }
    void FrameWidth(int frame_width)    { frame_width_ = frame_width; }
    void FrameHeight(int frame_height)  { frame_height_ = frame_height; }
    void FrameCount(int frame_count)    { frame_count_ = frame_count; }

    const std::string& TextureFilename() const { return TextureFilename_; }

    int StartX() const      { return start_x_; }
    int StartY() const      { return start_y_; }
    int FrameWidth() const  { return frame_width_; }
    int FrameHeight() const { return frame_height_; }
    int FrameCount() const  { return frame_count_; }

private:
    std::string TextureFilename_;
    int start_x_, start_y_;
    int frame_width_, frame_height_;
    int frame_count_;
};
typedef std::map<std::string, ImageAnimationDocument> ImageAnimationDocumentMap;

#endif  // INCLUDED_IMAGEANIMATIONDOCUMENT
