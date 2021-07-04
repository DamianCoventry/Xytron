#ifndef INCLUDED_CAMERADOCUMENT
#define INCLUDED_CAMERADOCUMENT

#include <string>
#include <map>

#include "../math/vector.h"

class CameraDocument
{
public:
    const std::string& Name() const         { return name_; }
    const Math::Vector& Position() const    { return position_; }
    const Math::Vector& Rotation() const    { return rotation_; }

    void Name(const std::string& name)          { name_ = name; }
    void Position(const Math::Vector& position) { position_ = position; }
    void Rotation(const Math::Vector& rotation) { rotation_ = rotation; }

private:
    std::string name_;
    Math::Vector position_;
    Math::Vector rotation_;
};

typedef std::map<std::string, CameraDocument> CameraDocumentMap;


#endif  // INCLUDED_CAMERADOCUMENT
