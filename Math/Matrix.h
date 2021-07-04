#ifndef INCLUDED_MATRIX
#define INCLUDED_MATRIX

#include "Vector.h"

namespace Math
{

struct Matrix
{
    Vector operator*(const Vector& V) const
    {
        return Vector(
            V.x_*V_[0].x_ + V.y_*V_[1].x_ + V.z_*V_[2].x_,
            V.x_*V_[0].y_ + V.y_*V_[1].y_ + V.z_*V_[2].y_,
            V.x_*V_[0].z_ + V.y_*V_[1].z_ + V.z_*V_[2].z_);
    }

    Vector V_[3];
};

}       // namespace Math

#endif  // INCLUDED_MATRIX
