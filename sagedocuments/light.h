#ifndef INCLUDED_LIGHT
#define INCLUDED_LIGHT

#include "../gfx/color.h"
#include "../math/vector.h"
#include "../sagestudio/worldtypes.h"
#include <boost/algorithm/string.hpp>
#include <list>

class Light
{
public:
    bool operator ==(const std::string& name) const
    {
        return boost::algorithm::iequals(name, name_);
    }

public:
    Light() : radius_(256.0f), use_world_ambient_(true), ambient_(25), Color_(1.0f, 1.0f, 1.0f) {}

    const std::string& Name() const         { return name_; }
    const CellIndex& GetCellIndex() const   { return cell_index_; }
    const Math::Vector& Position() const    { return position_; }
    float Radius() const                    { return radius_; }
    bool UseWorldAmbient() const            { return use_world_ambient_; }
    int Ambient() const                     { return ambient_; }
    Gfx::Color GetColor() const             { return Color_; }

    void Name(const std::string& name)              { name_ = name; }
    void SetCellIndex(const CellIndex& ci)          { cell_index_ = ci; }
    void Position(const Math::Vector& position, float cell_size)     { position_ = position; cell_index_.x_ = int(position_.x_ / cell_size); cell_index_.z_ = int(position_.z_ / cell_size); }
    void Radius(float radius)                       { radius_ = radius; }
    void UseWorldAmbient(bool use_world_ambient)    { use_world_ambient_ = use_world_ambient; }
    void Ambient(int ambient)                       { ambient_ = ambient; }
    void SetColor(const Gfx::Color& C)              { Color_ = C; }

private:
    std::string name_;
    CellIndex cell_index_;
    Math::Vector position_;
    float radius_;
    bool use_world_ambient_;
    int ambient_;
    Gfx::Color Color_;
};

typedef std::list<Light> LightList;

#endif
