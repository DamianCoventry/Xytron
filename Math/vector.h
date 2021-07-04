///////////////////////////////////////////////////////////////////////////////
//
//  PROJECT: Sage
//
//  AUTHOR: Damian Coventry
//
//  CREATION DATE: June-July 2007
// 
//  COPYRIGHT NOTICE:
//
//      (C) Omenware
//      Created in 2007 as an unpublished copyright work.  All rights reserved.
//      This document and the information it contains is confidential and
//      proprietary to Omenware.  Hence, it may not be  used, copied, reproduced,
//      transmitted, or stored in any form or by any means, electronic,
//      recording, photocopying, mechanical or otherwise, without the prior
//      written permission of Omenware
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
#ifndef INCLUDED_MATH_VECTOR
#define INCLUDED_MATH_VECTOR

#include <vector>
#include <math.h>

///////////////////////////////////////////////////////////////////////////////
namespace Math
{

extern const float EPSILON;
extern const float DTOR;
extern const float RTOD;

///////////////////////////////////////////////////////////////////////////////
struct Vector
{
    // Construction
    Vector(float f = 0.0f)
        : x_(f), y_(f), z_(f) {}

    Vector(float x, float y, float z)
        : x_(x), y_(y), z_(z) {}

    Vector(const Vector& vec)
        : x_(vec.x_), y_(vec.y_), z_(vec.z_) {}

    // Assignment
    void operator=(const Vector& vec)
    {
        x_ = vec.x_;
        y_ = vec.y_;
        z_ = vec.z_;
    }

    void operator=(float f)
    {
        x_ = f;
        y_ = f;
        z_ = f;
    }

    void operator+=(const Vector& vec)
    {
        x_ += vec.x_;
        y_ += vec.y_;
        z_ += vec.z_;
    }

    void operator-=(const Vector& vec)
    {
        x_ -= vec.x_;
        y_ -= vec.y_;
        z_ -= vec.z_;
    }

    void operator*=(const Vector& vec)
    {
        x_ *= vec.x_;
        y_ *= vec.y_;
        z_ *= vec.z_;
    }

    void operator/=(const Vector& vec)
    {
        x_ /= vec.x_;
        y_ /= vec.y_;
        z_ /= vec.z_;
    }

    void operator+=(float f)
    {
        x_ += f;
        y_ += f;
        z_ += f;
    }

    void operator-=(float f)
    {
        x_ -= f;
        y_ -= f;
        z_ -= f;
    }

    void operator*=(float f)
    {
        x_ *= f;
        y_ *= f;
        z_ *= f;
    }

    void operator/=(float f)
    {
        x_ /= f;
        y_ /= f;
        z_ /= f;
    }

    // Comparision
    bool operator==(const Vector& vec)
    {
        return (x_ >= vec.x_ - EPSILON) && (x_ <= vec.x_ + EPSILON) &&
               (y_ >= vec.y_ - EPSILON) && (y_ <= vec.y_ + EPSILON) &&
               (z_ >= vec.z_ - EPSILON) && (z_ <= vec.z_ + EPSILON);
    }

    bool operator!=(const Vector& vec) const
    {
        return ((x_ < vec.x_ - EPSILON) || (x_ > vec.x_ + EPSILON)) ||
               ((y_ < vec.y_ - EPSILON) || (y_ > vec.y_ + EPSILON)) ||
               ((z_ < vec.z_ - EPSILON) || (z_ > vec.z_ + EPSILON));
    }

    // Basic Math
    Vector operator+(const Vector& vec) const
    {
        return Vector(x_+vec.x_, y_+vec.y_, z_+vec.z_);
    }

    Vector operator-(const Vector& vec) const
    {
        return Vector(x_-vec.x_, y_-vec.y_, z_-vec.z_);
    }

    Vector operator*(const Vector& vec) const
    {
        return Vector(x_*vec.x_, y_*vec.y_, z_*vec.z_);
    }

    Vector operator/(const Vector& vec) const
    {
        return Vector(x_/vec.x_, y_/vec.y_, z_/vec.z_);
    }

    Vector operator+(float f) const
    {
        return Vector(x_+f, y_+f, z_+f);
    }

    Vector operator-(float f) const
    {
        return Vector(x_-f, y_-f, z_-f);
    }

    Vector operator*(float f) const
    {
        return Vector(x_*f, y_*f, z_*f);
    }

    Vector operator/(float f) const
    {
        return Vector(x_/f, y_/f, z_/f);
    }

    Vector operator-()
    {
        return Vector(-x_, -y_, -z_);
    }

    // Advanced Math
    float DotProduct(const Vector& vec) const
    {
        return (x_*vec.x_) + (y_*vec.y_) + (z_*vec.z_);
    }

    Vector CrossProduct(const Vector& vec) const
    {
        return Vector(y_*vec.z_ - z_*vec.y_,
                      z_*vec.x_ - x_*vec.z_,
                      x_*vec.y_ - y_*vec.x_);
    }

    void Normalise()
    {
        float d = (float)sqrt(x_*x_ + y_*y_ + z_*z_);
        if(d == 0.0f)
        {
            d = 0.00000001f;
        }

        x_ /= d;
        y_ /= d;
        z_ /= d;
    }

    float Magnitude() const
    {
        return (float)sqrt(x_*x_ + y_*y_ + z_*z_);
    }

    // Reflect "this" vector around "unit_vec" and returns the reflected vector
    Vector Reflect(const Vector& unit_vec) const
    {
        // Dot the inverse of this vector
        Vector inv(-x_, -y_, -z_);
        float dot = DotProduct(inv);

        return Vector(2.0f * unit_vec.x_ * dot - inv.x_,
                      2.0f * unit_vec.y_ * dot - inv.y_,
                      2.0f * unit_vec.z_ * dot - inv.z_);
    }

    float x_, y_, z_;
};

typedef std::vector<Vector> VectorList;

}       // namespace Math

#endif  // INCLUDED_MATH_VECTOR
