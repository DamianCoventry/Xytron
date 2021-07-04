#ifndef INCLUDED_MATH_LINESEGMENT
#define INCLUDED_MATH_LINESEGMENT

#include "vector.h"

namespace Math
{

struct LineSegment
{
    LineSegment(const LineSegment& rhs)
        : begin_(rhs.begin_), end_(rhs.end_) {}

    LineSegment(const Vector& begin, const Vector& end)
        : begin_(begin), end_(end) {}

    Vector begin_;
    Vector end_;
};

}       // namespace Math

#endif  // INCLUDED_MATH_LINESEGMENT
