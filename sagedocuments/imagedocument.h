#ifndef INCLUDED_IMAGEDOCUMENT
#define INCLUDED_IMAGEDOCUMENT

#include <string>
#include <map>

class ImageDocument
{
public:
    ImageDocument()
        : frame_x_(0), frame_y_(0), frame_width_(100), frame_height_(50) {}

    void TextureFilename(const std::string& Filename) { TextureFilename_ = Filename; }

    void FrameX(int frame_x)            { frame_x_ = frame_x; }
    void FrameY(int frame_y)            { frame_y_ = frame_y; }
    void FrameWidth(int frame_width)    { frame_width_ = frame_width; }
    void FrameHeight(int frame_height)  { frame_height_ = frame_height; }

    const std::string& TextureFilename() const { return TextureFilename_; }

    int FrameX() const      { return frame_x_; }
    int FrameY() const      { return frame_y_; }
    int FrameWidth() const  { return frame_width_; }
    int FrameHeight() const { return frame_height_; }

private:
    std::string TextureFilename_;
    int frame_x_, frame_y_;
    int frame_width_, frame_height_;
};
typedef std::map<std::string, ImageDocument> ImageDocumentMap;

#endif  // INCLUDED_IMAGEDOCUMENT
