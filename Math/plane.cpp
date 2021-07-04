#include "stdafx.h"
#include "plane.h"
#include "linesegment.h"
#include <math.h>

using namespace Math;

bool Plane::GetIntersection(const LineSegment& ls, Vector& intersection, float plane_shift, float* percentage)
{
    Vector dir, L1;

    // calculate the lines dir vector (Green line in figure d)
    dir.x_ = ls.end_.x_ - ls.begin_.x_;
    dir.y_ = ls.end_.y_ - ls.begin_.y_;
    dir.z_ = ls.end_.z_ - ls.begin_.z_;

    float linelength = dir.DotProduct(normal_); 
    // This gives us the line length (the blue dot L3  +  L4 in figure d)

    if(fabsf(linelength) < Math::EPSILON) 
    // check it does not = 0 with tolerance for floating point rounding errors
    {
        return false; 
    // = 0 means the line is parallel to the plane so can not intersect it
    }

    L1.x_ = on_plane_.x_ - ls.begin_.x_; // calculate vector L1 (the PINK line in figure d)
    L1.y_ = on_plane_.y_ - ls.begin_.y_;
    L1.z_ = on_plane_.z_ - ls.begin_.z_;

    float dist_from_plane = L1.DotProduct(normal_); 
    // gives the distance from the plane (ORANGE Line L3 in figure d)

    *percentage = (dist_from_plane + plane_shift) / linelength; 
    // How far from Linestart, intersection is as a percentage of 0 to 1 

    if(*percentage < 0) // The plane is behind the start of the line
    {
        return false;
    }
    else if(*percentage > 1) // The line does not reach the plane
    {
        return false;
    }

    // add the percentage of the line to line start
    intersection.x_ = ls.begin_.x_ + dir.x_ * (*percentage); 
    intersection.y_ = ls.begin_.y_ + dir.y_ * (*percentage);
    intersection.z_ = ls.begin_.z_ + dir.z_ * (*percentage);

    return true ; // ooh yes we have hit a plane; 
}

Plane::PointPosition Plane::ClassifyPoint(const Vector& point, float plane_shift) const
{
    Vector dir = on_plane_ - point;
    float result = dir.DotProduct(normal_) + plane_shift;
    if(result < -Math::EPSILON) return PP_INFRONT;
    if(result > Math::EPSILON) return PP_BEHIND;
    return PP_ONPLANE;
}

Plane::LineSegmentPosition Plane::ClassifyLineSegment(const LineSegment& ls, float plane_shift) const
{
    Plane::PointPosition ppb = ClassifyPoint(ls.begin_, plane_shift);
    Plane::PointPosition ppe = ClassifyPoint(ls.end_, plane_shift);

    switch(ppb)
    {
    case PP_BEHIND:
        switch(ppe)
        {
        case PP_BEHIND:
            return LSP_BEHIND;
        case PP_INFRONT:
            return LSP_SPANNING;
        }
        // else PP_ONPLANE
        return LSP_BEHIND;
    case PP_INFRONT:
        switch(ppe)
        {
        case PP_BEHIND:
            return LSP_SPANNING;
        case PP_INFRONT:
            return LSP_INFRONT;
        }
        // else PP_ONPLANE
        return LSP_INFRONT;
    }
    // else PP_ONPLANE

    switch(ppe)
    {
    case PP_BEHIND:
        return LSP_SPANNING;//LSP_BEHIND;
    case PP_INFRONT:
        return LSP_INFRONT;
    }
    // else PP_ONPLANE
    return LSP_ONPLANE;
}
