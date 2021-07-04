#ifndef INCLUDED_ARROWLINE
#define INCLUDED_ARROWLINE

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>

#include <vector>

class ArrowLine
{
public:
    ArrowLine(const POINT& begin, const POINT& end)
    { Rebuild(begin, end); }

    void Rebuild(const POINT& begin, const POINT& end);

    void Draw(HDC dc);

private:
    struct ArrowHeadState
    {
        POINT points_[3];
    };
    typedef std::vector<ArrowHeadState> LineArrowHeads;

    POINT begin_;
    POINT end_;
    LineArrowHeads arrow_heads_;
};


#endif  // INCLUDED_ARROWLINE
