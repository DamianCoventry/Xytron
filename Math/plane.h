#ifndef INCLUDED_MATH_PLANE
#define INCLUDED_MATH_PLANE

#include "vector.h"

namespace Math
{

struct LineSegment;

struct Plane
{
    Plane() {}
    Plane(Vector& normal, Vector& on_plane)
        : normal_(normal), on_plane_(on_plane) {}

    bool GetIntersection(const LineSegment& ls, Vector& intersection, float plane_shift = 0.0f, float* percentage = NULL);

    enum PointPosition { PP_BEHIND, PP_INFRONT, PP_ONPLANE };
    PointPosition ClassifyPoint(const Vector& point, float plane_shift = 0.0f) const;

    enum LineSegmentPosition { LSP_BEHIND, LSP_INFRONT, LSP_ONPLANE , LSP_SPANNING };
    LineSegmentPosition ClassifyLineSegment(const LineSegment& ls, float plane_shift = 0.0f) const;

    // Data
    Vector normal_;
    Vector on_plane_;
};

}       // namespace Math

#endif  // INCLUDED_MATH_PLANE
