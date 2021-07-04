#include "dialogutil.h"

void DialogUtil::CenterInParent(HWND window)
{
    // We center the window by finding its parent's center point and placing child's
    // center point on top of it.  However, we must make sure we don't let any part
    // of the window fall outside of the desktop.
    RECT work_area;
    SystemParametersInfo(SPI_GETWORKAREA, 0, reinterpret_cast<void*>(&work_area), 0);

    // Get the parent's rect, if there is a parent.
    RECT parent_rect;
    HWND parent = GetParent(window);
    if(!parent)
    {
        memcpy(&parent_rect, &work_area, sizeof(RECT));
    }
    else
    {
        GetWindowRect(parent, &parent_rect);
    }

    // Get the child's rect.
    RECT dialog_rect;
    GetWindowRect(window, &dialog_rect);

    // Don't let the window fall outside the desktop.
    int width = dialog_rect.right - dialog_rect.left;
    int x = parent_rect.left + ((parent_rect.right - parent_rect.left)/2) - (width/2);
    if(x < 0)
    {
        x = 0;
    }
    else if(x + width > work_area.right)
    {
        x = work_area.right - width;
    }

    // Don't let the window fall outside the desktop.
    int height = dialog_rect.bottom - dialog_rect.top;
    int y = parent_rect.top  + ((parent_rect.bottom - parent_rect.top)/2) - (height/2);
    if(y < 0)
    {
        y = 0;
    }
    else if(y + height > work_area.bottom)
    {
        y  = work_area.bottom - height;
    }

    // Move the window.
    SetWindowPos(window, NULL, x, y, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
}
