#include "arrowline.h"
#include <math.h>
#include "../math/vector.h"

void ArrowLine::Rebuild(const POINT& begin, const POINT& end)
{
    begin_ = begin;
    end_ = end;

    float x_length = float(end_.x - begin_.x);
    float y_length = float(end_.y - begin_.y);
    long line_length = long(sqrt(x_length*x_length + y_length*y_length));
    long num_arrows = line_length / 30;

    long multiplier = 1;

    arrow_heads_.clear();
    for(int i = 0; i < num_arrows; i++)
    {
        ArrowHeadState state;

        POINT arrow_tip_point;
        arrow_tip_point.x = begin_.x + (((end_.x - begin_.x)*multiplier)/num_arrows);
        arrow_tip_point.y = begin_.y + (((end_.y - begin_.y)*multiplier)/num_arrows);

        Math::Vector forward(float(end_.x - begin_.x),
                             float(end_.y - begin_.y), 0.0f);
        forward.Normalise();
        Math::Vector up(0.0f, 0.0f, 1.0f);
        Math::Vector right(forward.CrossProduct(up));

        state.points_[0].x = arrow_tip_point.x;
        state.points_[0].y = arrow_tip_point.y;

        Math::Vector temp(float(arrow_tip_point.x), float(arrow_tip_point.y), 0.0f);
        temp += right * 3.0f;
        temp -= forward * 9.0f;
        state.points_[1].x = long(temp.x_);
        state.points_[1].y = long(temp.y_);

        temp = Math::Vector(float(arrow_tip_point.x), float(arrow_tip_point.y), 0.0f);
        temp -= right * 3.0f;
        temp -= forward * 9.0f;
        state.points_[2].x = long(temp.x_);
        state.points_[2].y = long(temp.y_);

        multiplier++;
        arrow_heads_.push_back(state);
    }
}

void ArrowLine::Draw(HDC dc)
{
    MoveToEx(dc, begin_.x, begin_.y, NULL);
    LineTo(dc, end_.x, end_.y);

    LineArrowHeads::const_iterator itor;
    for(itor = arrow_heads_.begin(); itor != arrow_heads_.end(); ++itor)
    {
        Polygon(dc, itor->points_, 3);
    }
}
