#include "statemachinewindow.h"
#include "resource.h"
#include "../math/vector.h"

#include <algorithm>
#include <stdexcept>
#include <sstream>

#include <windowsx.h>
#include <commctrl.h>
#include <boost/bind.hpp>
#include <typeinfo.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'StateMachineWindow *' of greater size

const char* StateMachineWindow::window_title_ = "StateMachineWindow";
const char* StateMachineWindow::class_name_   = "StateMachineWindow";

const std::string EntryNodeDrawingElement::entry_text_  = "Entry";
const std::string ExitNodeDrawingElement::exit_text_    = "Exit";



DrawingElement::DrawingElement(int id, const POINT& position, const SIZE& size)
: id_(id)
, selected_(false)
, mark_(false)
{
    position_   = position;
    size_       = size;

    drag_offset_.x = drag_offset_.y = 0;

    selected_pen_   = CreatePen(PS_SOLID, 0, RGB(255, 127, 0));
    selected_brush_ = CreateSolidBrush(RGB(255, 127, 0));
}

DrawingElement::~DrawingElement()
{
    DeletePen(selected_pen_);
    DeleteBrush(selected_brush_);
}

bool DrawingElement::HitTest(const POINT& point)
{
    if(point.x < position_.x - size_.cx/2) return false;
    if(point.x > position_.x + size_.cx/2) return false;
    if(point.y < position_.y - size_.cy/2) return false;
    if(point.y > position_.y + size_.cy/2) return false;
    return true;
}





EntryNodeDrawingElement::EntryNodeDrawingElement(int id, const POINT& position, HDC dc)
: DrawingElement(id, position)
{
    outline_pen_    = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
    fill_brush_     = CreateSolidBrush(RGB(192, 192, 192));

    font_ = CreateFont(-MulDiv(10, GetDeviceCaps(
        dc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");

    SIZE size;
    size.cx = size.cy = 16;
    Size(size);
}

EntryNodeDrawingElement::~EntryNodeDrawingElement()
{
    DeleteObject(outline_pen_);
    DeleteObject(fill_brush_);
    DeleteFont(font_);
}

void EntryNodeDrawingElement::Draw(HDC dc)
{
    HPEN prev_pen;
    HBRUSH prev_brush;
    RECT rect;

    if(Selected())
    {
        prev_pen       = SelectPen(dc, SelectedPen());
        prev_brush   = SelectBrush(dc, SelectedBrush());

        rect.left   = (Position().x - Size().cx/2)-5;
        rect.right  = (Position().x + Size().cx/2)+5;
        rect.top    = (Position().y - Size().cy/2)-5;
        rect.bottom = (Position().y + Size().cy/2)+5;
        Ellipse(dc, rect.left, rect.top, rect.right, rect.bottom);

        SelectObject(dc, prev_pen);
        SelectObject(dc, prev_brush);
    }

    prev_pen        = SelectPen(dc, outline_pen_);
    prev_brush      = SelectBrush(dc, fill_brush_);
    HFONT prev_font = SelectFont(dc, font_);
    int prev_mode   = SetBkMode(dc, TRANSPARENT);

    rect.left   = Position().x - Size().cx/2;
    rect.right  = Position().x + Size().cx/2;
    rect.top    = Position().y - Size().cy/2;
    rect.bottom = Position().y + Size().cy/2;
    Ellipse(dc, rect.left, rect.top, rect.right, rect.bottom);

    rect.left   = Position().x - Size().cx*2;
    rect.right  = Position().x + Size().cx*2;
    rect.top    = Position().y - Size().cy*2;
    rect.bottom = Position().y;
    DrawText(dc, entry_text_.c_str(), int(entry_text_.size()), &rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

    SetBkMode(dc, prev_mode);
    SelectFont(dc, prev_font);
    SelectObject(dc, prev_pen);
    SelectObject(dc, prev_brush);
}





ExitNodeDrawingElement::ExitNodeDrawingElement(int id, const POINT& position, HDC dc)
: DrawingElement(id, position)
{
    outline_pen_    = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
    fill_brush_     = CreateSolidBrush(RGB(127, 127, 127));

    font_ = CreateFont(-MulDiv(10, GetDeviceCaps(
        dc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");

    SIZE size;
    size.cx = size.cy = 16;
    Size(size);
}

ExitNodeDrawingElement::~ExitNodeDrawingElement()
{
    DeleteObject(outline_pen_);
    DeleteObject(fill_brush_);
    DeleteFont(font_);
}

void ExitNodeDrawingElement::Draw(HDC dc)
{
    HPEN prev_pen;
    HBRUSH prev_brush;
    if(Selected())
    {
        prev_pen    = SelectPen(dc, SelectedPen());
        prev_brush  = SelectBrush(dc, SelectedBrush());

        RECT rect;
        rect.left   = (Position().x - Size().cx/2)-5;
        rect.right  = (Position().x + Size().cx/2)+5;
        rect.top    = (Position().y - Size().cy/2)-5;
        rect.bottom = (Position().y + Size().cy/2)+5;
        Ellipse(dc, rect.left, rect.top, rect.right, rect.bottom);

        SelectObject(dc, prev_pen);
        SelectObject(dc, prev_brush);
    }

    prev_pen        = SelectPen(dc, outline_pen_);
    prev_brush      = SelectBrush(dc, fill_brush_);
    HFONT prev_font = SelectFont(dc, font_);
    int prev_mode   = SetBkMode(dc, TRANSPARENT);

    RECT rect;
    rect.left   = Position().x - Size().cx/2;
    rect.right  = Position().x + Size().cx/2;
    rect.top    = Position().y - Size().cy/2;
    rect.bottom = Position().y + Size().cy/2;
    Ellipse(dc, rect.left, rect.top, rect.right, rect.bottom);

    rect.left   = Position().x - Size().cx*2;
    rect.right  = Position().x + Size().cx*2;
    rect.top    = Position().y - Size().cy*2;
    rect.bottom = Position().y;
    DrawText(dc, exit_text_.c_str(), int(exit_text_.size()), &rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

    SetBkMode(dc, prev_mode);
    SelectFont(dc, prev_font);
    SelectObject(dc, prev_pen);
    SelectObject(dc, prev_brush);
}





StateDrawingElement::StateDrawingElement(int id, const POINT& position, HDC dc)
: DrawingElement(id, position)
{
    outline_pen_    = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
    fill_brush_     = CreateSolidBrush(RGB(32, 224, 32));

    font_ = CreateFont(-MulDiv(10, GetDeviceCaps(
        dc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");

    SIZE size;
    size.cx = 128;
    size.cy = 64;
    Size(size);
}

StateDrawingElement::~StateDrawingElement()
{
    DeleteObject(outline_pen_);
    DeleteObject(fill_brush_);
    DeleteFont(font_);
}

void StateDrawingElement::Draw(HDC dc)
{
    HPEN prev_pen;
    HBRUSH prev_brush;
    if(Selected())
    {
        prev_pen    = SelectPen(dc, SelectedPen());
        prev_brush  = SelectBrush(dc, SelectedBrush());

        RECT rect;
        rect.left   = (Position().x - Size().cx/2)-5;
        rect.right  = (Position().x + Size().cx/2)+5;
        rect.top    = (Position().y - Size().cy/2)-5;
        rect.bottom = (Position().y + Size().cy/2)+5;
        Ellipse(dc, rect.left, rect.top, rect.right, rect.bottom);

        SelectObject(dc, prev_pen);
        SelectObject(dc, prev_brush);
    }

    prev_pen        = SelectPen(dc, outline_pen_);
    prev_brush      = SelectBrush(dc, fill_brush_);
    HFONT prev_font = SelectFont(dc, font_);
    int prev_mode   = SetBkMode(dc, TRANSPARENT);

    RECT rect;
    rect.left   = Position().x - Size().cx/2;
    rect.right  = Position().x + Size().cx/2;
    rect.top    = Position().y - Size().cy/2;
    rect.bottom = Position().y + Size().cy/2;

    Ellipse(dc, rect.left, rect.top, rect.right, rect.bottom);
    DrawText(dc, text_.c_str(), int(text_.size()), &rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

    SetBkMode(dc, prev_mode);
    SelectFont(dc, prev_font);
    SelectObject(dc, prev_pen);
    SelectObject(dc, prev_brush);
}





EventDrawingElement::EventDrawingElement(int id, HDC dc)
: DrawingElement(id)
, source_(NULL)
, dest_(NULL)
{
    line_pen_       = CreatePen(PS_SOLID, 0, RGB(16, 16, 224));
    fill_brush_     = CreateSolidBrush(RGB(0, 0, 0));

    font_ = CreateFont(-MulDiv(10, GetDeviceCaps(
        dc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");

    SIZE size;
    size.cx = 128;
    size.cy = 64;
    Size(size);
}

EventDrawingElement::~EventDrawingElement()
{
    DeleteObject(fill_brush_);
    DeleteObject(line_pen_);
    DeleteFont(font_);
}

void EventDrawingElement::Draw(HDC dc)
{
    if(source_ && dest_)
    {
        HPEN prev_pen       = SelectPen(dc, Selected() ? SelectedPen() : line_pen_);
        HFONT prev_font     = SelectFont(dc, font_);
        HBRUSH prev_brush   = SelectBrush(dc, fill_brush_);
        int prev_mode       = SetBkMode(dc, Selected() ? OPAQUE : TRANSPARENT);
        COLORREF prev_color = SetBkColor(dc, Selected() ? RGB(255, 127, 0) : RGB(255, 255, 255));

        ArrowLine arrow_line_0(source_->Position(), Position());
        arrow_line_0.Draw(dc);
        ArrowLine arrow_line_1(Position(), dest_->Position());
        arrow_line_1.Draw(dc);

        RECT rect;
        rect.left   = Position().x - Size().cx/2;
        rect.right  = Position().x + Size().cx/2;
        rect.top    = Position().y - Size().cy/2;
        rect.bottom = Position().y + Size().cy/2;
        DrawText(dc, text_.c_str(), int(text_.size()), &rect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

        SetBkColor(dc, prev_color);
        SetBkMode(dc, prev_mode);
        SelectFont(dc, prev_font);
        SelectObject(dc, prev_pen);
        SelectObject(dc, prev_brush);
    }
}





StateMachineWindow::StateMachineWindow(HINSTANCE instance, HWND parent, StateMachineWindowEvents* event_handler)
: instance_(instance)
, handle_(NULL)
, current_tool_(T_SELECT_NODE)
, double_buffer_dc_(NULL)
, double_buffer_bitmap_(NULL)
, drawing_event_line_(false)
, items_were_dragged_(false)
, event_handler_(event_handler)
, unique_id_(0)
{
    Register();
    Create(parent);

    event_line_pen_ = CreatePen(PS_DOT, 0, RGB(0, 0, 0));

    entry_cursor_   = (HCURSOR)LoadImage(instance_, MAKEINTRESOURCE(IDC_INSERT_ENTRY_CURSOR), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE);
    exit_cursor_    = (HCURSOR)LoadImage(instance_, MAKEINTRESOURCE(IDC_INSERT_EXIT_CURSOR), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE);
    state_cursor_   = (HCURSOR)LoadImage(instance_, MAKEINTRESOURCE(IDC_INSERT_STATE_CURSOR), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE);
    event_cursor_   = (HCURSOR)LoadImage(instance_, MAKEINTRESOURCE(IDC_INSERT_EVENT_CURSOR), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE);
}

StateMachineWindow::~StateMachineWindow()
{
    DestroyCursor(entry_cursor_);
    DestroyCursor(exit_cursor_);
    DestroyCursor(state_cursor_);
    DestroyCursor(event_cursor_);


    DeletePen(event_line_pen_);
    Delete();
    Unregister();
}





void StateMachineWindow::Register()
{
    WNDCLASS wc;
    wc.style            = CS_OWNDC | CS_DBLCLKS;
    wc.lpfnWndProc      = StateMachineWindowProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = instance_;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_CROSS);
    wc.hbrBackground    = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = class_name_;

    if(!RegisterClass(&wc))
    {
        throw std::runtime_error("Unable to register the StateMachineWindow's window class");
    }
}

void StateMachineWindow::Unregister()
{
    UnregisterClass(class_name_, instance_);
}

void StateMachineWindow::Create(HWND parent)
{
    handle_ = CreateWindowEx(WS_EX_CLIENTEDGE, class_name_, window_title_, WS_CHILD | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parent, NULL, instance_,
        reinterpret_cast<void*>(this));

    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to register StateMachineWindow");
    }

    CreateDoubleBuffer();
}

void StateMachineWindow::Delete()
{
    if(handle_)
    {
        DeleteDoubleBuffer();

        DestroyWindow(handle_);
        handle_ = NULL;
    }
}

void StateMachineWindow::CreateDoubleBuffer()
{
    RECT rect;
    GetClientRect(handle_, &rect);

    HDC dc                  = GetDC(handle_);
    double_buffer_dc_       = CreateCompatibleDC(dc);
    double_buffer_bitmap_   = CreateCompatibleBitmap(dc, rect.right, rect.bottom);

    SelectObject(double_buffer_dc_, double_buffer_bitmap_);
    ReleaseDC(handle_, dc);
}

void StateMachineWindow::DeleteDoubleBuffer()
{
    DeleteBitmap(double_buffer_bitmap_);
    DeleteDC(double_buffer_dc_);
}

void StateMachineWindow::SetUniqueIdToHighestId()
{
    unique_id_ = 0;
    DrawingElementList::iterator itor;
    for(itor = de_list_.begin(); itor != de_list_.end(); ++itor)
    {
        if((*itor)->Id() > unique_id_) unique_id_ = (*itor)->Id();
    }
    ++unique_id_;
}

bool StateMachineWindow::InsertEntryNode(int id, const POINT& position, bool notify, bool repaint)
{
    HDC dc = GetDC(handle_);

    DrawingElementPtr de(new EntryNodeDrawingElement(id, position, dc));
    de_list_.push_back(de);

    ReleaseDC(handle_, dc);

    if(notify)
    {
        event_handler_->OnStateMachineContentsModified();
        event_handler_->OnStateMachineEntryNodeInserted(id, position);
    }
    if(repaint)
    {
        InvalidateRect(handle_, NULL, FALSE);
    }
    return true;
}

bool StateMachineWindow::UpdateEntryNode(int id, const POINT& position, bool notify, bool repaint)
{
    DrawingElementList::iterator itor;
    for(itor = de_list_.begin(); itor != de_list_.end(); ++itor)
    {
        if((*itor)->Id() == id)
        {
            (*itor)->Position(position);

            if(notify)
            {
                event_handler_->OnStateMachineContentsModified();
                event_handler_->OnStateMachineEntryNodeUpdated(id, position);
            }
            if(repaint)
            {
                InvalidateRect(handle_, NULL, FALSE);
            }
            return true;
        }
    }
    return false;
}

bool StateMachineWindow::InsertExitNode(int id, const POINT& position, bool notify, bool repaint)
{
    HDC dc = GetDC(handle_);

    DrawingElementPtr de(new ExitNodeDrawingElement(id, position, dc));
    de_list_.push_back(de);

    ReleaseDC(handle_, dc);

    if(notify)
    {
        event_handler_->OnStateMachineContentsModified();
        event_handler_->OnStateMachineExitNodeInserted(id, position);
    }
    if(repaint)
    {
        InvalidateRect(handle_, NULL, FALSE);
    }
    return true;
}

bool StateMachineWindow::UpdateExitNode(int id, const POINT& position, bool notify, bool repaint)
{
    DrawingElementList::iterator itor;
    for(itor = de_list_.begin(); itor != de_list_.end(); ++itor)
    {
        if((*itor)->Id() == id)
        {
            (*itor)->Position(position);

            if(notify)
            {
                event_handler_->OnStateMachineContentsModified();
                event_handler_->OnStateMachineExitNodeUpdated(id, position);
            }
            if(repaint)
            {
                InvalidateRect(handle_, NULL, FALSE);
            }
            return true;
        }
    }
    return false;
}

bool StateMachineWindow::InsertStateNode(int id, const POINT& position, const std::string& name, bool notify, bool repaint)
{
    HDC dc = GetDC(handle_);

    DrawingElementPtr de(new StateDrawingElement(id, position, dc));
    StateDrawingElement* state = static_cast<StateDrawingElement*>(de.get());
    if(name.empty())
    {
        std::ostringstream oss;
        oss << "State" << id;
        state->Text(oss.str());
    }
    else
    {
        state->Text(name);
    }
    de_list_.push_back(de);

    ReleaseDC(handle_, dc);

    if(notify)
    {
        event_handler_->OnStateMachineContentsModified();
        event_handler_->OnStateMachineStateNodeInserted(id, position, name);
    }
    if(repaint)
    {
        InvalidateRect(handle_, NULL, FALSE);
    }
    return true;
}

bool StateMachineWindow::UpdateStateNode(int id, const POINT& position, const std::string& name, bool notify, bool repaint)
{
    DrawingElementList::iterator itor;
    for(itor = de_list_.begin(); itor != de_list_.end(); ++itor)
    {
        if((*itor)->Id() == id)
        {
            StateDrawingElement* state = dynamic_cast<StateDrawingElement*>(itor->get());
            if(state)
            {
                state->Position(position);
                state->Text(name);

                if(notify)
                {
                    event_handler_->OnStateMachineContentsModified();
                    event_handler_->OnStateMachineStateNodeUpdated(id, position, name);
                }
                if(repaint)
                {
                    InvalidateRect(handle_, NULL, FALSE);
                }
                return true;
            }
            return false;
        }
    }
    return false;
}

bool StateMachineWindow::InsertEventNode(int id, const POINT& position, const std::string& name, int source_id, int dest_id, bool notify, bool repaint)
{
    DrawingElement* source_ptr = NULL;
    DrawingElement* dest_ptr = NULL;

    DrawingElementList::iterator itor;
    for(itor = de_list_.begin(); itor != de_list_.end(); ++itor)
    {
        if((*itor)->Id() == source_id)
        {
            source_ptr = itor->get();
            break;
        }
    }
    if(source_ptr == NULL)
    {
        return false;
    }

    for(itor = de_list_.begin(); itor != de_list_.end(); ++itor)
    {
        if((*itor)->Id() == dest_id)
        {
            dest_ptr = itor->get();
            break;
        }
    }
    if(dest_ptr == NULL)
    {
        return false;
    }

    HDC dc = GetDC(handle_);

    DrawingElementPtr event_de(new EventDrawingElement(id, dc));
    EventDrawingElement* event = static_cast<EventDrawingElement*>(event_de.get());
    event->Position(position);
    event->SourceDrawingElement(source_ptr);
    event->DestinationDrawingElement(dest_ptr);
    if(name.empty())
    {
        std::ostringstream oss;
        oss << "Event" << id;
        event->Text(oss.str());
    }
    else
    {
        event->Text(name);
    }
    de_list_.push_front(event_de);      // Add event lines to the front so they're drawn obscured by states

    ReleaseDC(handle_, dc);

    if(notify)
    {
        event_handler_->OnStateMachineContentsModified();
        event_handler_->OnStateMachineEventNodeInserted(id, position, name, source_id, dest_id);
    }
    if(repaint)
    {
        InvalidateRect(handle_, NULL, FALSE);
    }
    return true;
}

bool StateMachineWindow::UpdateEventNode(int id, const POINT& position, const std::string& name, int source_id, int dest_id, bool notify, bool repaint)
{
    DrawingElementList::iterator itor;
    for(itor = de_list_.begin(); itor != de_list_.end(); ++itor)
    {
        if((*itor)->Id() == id)
        {
            break;
        }
    }
    if(itor == de_list_.end())
    {
        return false;
    }

    DrawingElement* source_ptr = NULL;
    DrawingElement* dest_ptr = NULL;

    DrawingElementList::iterator temp_itor;
    for(temp_itor = de_list_.begin(); temp_itor != de_list_.end(); ++temp_itor)
    {
        if((*temp_itor)->Id() == source_id)
        {
            source_ptr = temp_itor->get();
            break;
        }
    }
    if(source_ptr == NULL)
    {
        return false;
    }

    for(temp_itor = de_list_.begin(); temp_itor != de_list_.end(); ++temp_itor)
    {
        if((*temp_itor)->Id() == dest_id)
        {
            dest_ptr = temp_itor->get();
            break;
        }
    }
    if(dest_ptr == NULL)
    {
        return false;
    }

    EventDrawingElement* event = dynamic_cast<EventDrawingElement*>(itor->get());
    if(event)
    {
        event->Position(position);
        event->SourceDrawingElement(source_ptr);
        event->DestinationDrawingElement(dest_ptr);
        event->Text(name);

        if(notify)
        {
            event_handler_->OnStateMachineContentsModified();
            event_handler_->OnStateMachineEventNodeUpdated(id, position, name, source_id, dest_id);
        }
        if(repaint)
        {
            InvalidateRect(handle_, NULL, FALSE);
        }
        return true;
    }
    return false;
}

bool StateMachineWindow::RemoveNode(int id, bool notify, bool repaint)
{
    DrawingElementList::iterator itor;
    for(itor = de_list_.begin(); itor != de_list_.end(); ++itor)
    {
        if((*itor)->Id() == id)
        {
            if(notify)
            {
                if(dynamic_cast<EntryNodeDrawingElement*>(itor->get()))
                {
                    event_handler_->OnStateMachineEntryNodeRemoved((*itor)->Id());
                }
                else if(dynamic_cast<ExitNodeDrawingElement*>(itor->get()))
                {
                    event_handler_->OnStateMachineExitNodeRemoved((*itor)->Id());
                }
                else if(dynamic_cast<StateDrawingElement*>(itor->get()))
                {
                    event_handler_->OnStateMachineStateNodeRemoved((*itor)->Id());
                }
                else //if(dynamic_cast<EventDrawingElement*>(itor->get()))
                {
                    event_handler_->OnStateMachineEventNodeRemoved((*itor)->Id());
                }
            }
            de_list_.erase(itor);

            if(repaint)
            {
                InvalidateRect(handle_, NULL, FALSE);
            }
            return true;
        }
    }

    return false;
}

void StateMachineWindow::DeleteSelected()
{
    DrawingElementList::iterator de_itor;
    SelectedDrawingElementList::iterator sel_itor;
    for(sel_itor = selected_des_.begin(); sel_itor != selected_des_.end(); ++sel_itor)
    {
        // This element might be pointed to by one or more EventDrawingElement objects.
        if(dynamic_cast<EntryNodeDrawingElement*>(*sel_itor) ||
           dynamic_cast<ExitNodeDrawingElement*>(*sel_itor) ||
           dynamic_cast<StateDrawingElement*>(*sel_itor))
        {
            for(de_itor = de_list_.begin(); de_itor != de_list_.end(); ++de_itor)
            {
                EventDrawingElement* event = dynamic_cast<EventDrawingElement*>(de_itor->get());
                if(event)
                {
                    if(*sel_itor == event->SourceDrawingElement())
                    {
                        event->SourceDrawingElement(NULL);
                    }
                    if(*sel_itor == event->DestinationDrawingElement())
                    {
                        event->DestinationDrawingElement(NULL);
                    }
                }
            }
        }

        // Find this node and mark it for erasure
        for(de_itor = de_list_.begin(); de_itor != de_list_.end(); ++de_itor)
        {
            if(de_itor->get() == *sel_itor)
            {
                if(dynamic_cast<EntryNodeDrawingElement*>(de_itor->get()))
                {
                    event_handler_->OnStateMachineEntryNodeRemoved((*de_itor)->Id());
                }
                else if(dynamic_cast<ExitNodeDrawingElement*>(de_itor->get()))
                {
                    event_handler_->OnStateMachineExitNodeRemoved((*de_itor)->Id());
                }
                else if(dynamic_cast<StateDrawingElement*>(de_itor->get()))
                {
                    event_handler_->OnStateMachineStateNodeRemoved((*de_itor)->Id());
                }
                else //if(dynamic_cast<EventDrawingElement*>(de_itor->get()))
                {
                    event_handler_->OnStateMachineEventNodeRemoved((*de_itor)->Id());
                }
                (*de_itor)->Mark(true);
                break;
            }
        }
    }

    // Delete anything that's been marked
    de_itor = de_list_.begin();
    while(de_itor != de_list_.end())
    {
        if((*de_itor)->Marked())
        {
            de_list_.erase(de_itor);
            de_itor = de_list_.begin();
        }
        else
        {
            ++de_itor;
        }
    }

    // If we just removed an Entry, Exit or State node and this node was connected to
    // one or more EventDrawingElement objects, then those EventDrawingElement objects
    // will have one end of their lines pointing to nothing.  We'll delete these
    // EventDrawingElement objects too.
    de_itor = de_list_.begin();
    while(de_itor != de_list_.end())
    {
        EventDrawingElement* event = dynamic_cast<EventDrawingElement*>(de_itor->get());
        if(event && (event->SourceDrawingElement() == NULL || event->DestinationDrawingElement() == NULL))
        {
            event_handler_->OnStateMachineEventNodeRemoved((*de_itor)->Id());
            de_list_.erase(de_itor);
            de_itor = de_list_.begin();
        }
        else
        {
            ++de_itor;
        }
    }

    selected_des_.clear();
    InvalidateRect(handle_, NULL, FALSE);
}

void StateMachineWindow::Clear()
{
    unique_id_ = 0;
    drawing_event_line_ = false;
    selected_des_.clear();
    de_list_.clear();
    InvalidateRect(handle_, NULL, FALSE);
}

void StateMachineWindow::RepositionWithinParent()
{
    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);

    SetWindowPos(handle_, NULL, 200, 0, parent_client_rect.right-350, parent_client_rect.bottom, SWP_NOZORDER);

    DeleteDoubleBuffer();
    CreateDoubleBuffer();
}

bool StateMachineWindow::GetFirstSelectedDrawingElement(EntryNodeDrawingElement** entry_node,
                                                        ExitNodeDrawingElement** exit_node,
                                                        StateDrawingElement** state_node,
                                                        EventDrawingElement** event_node) const
{
    if(selected_des_.empty())
    {
        return false;
    }

    *entry_node = dynamic_cast<EntryNodeDrawingElement*>(selected_des_.front());
    *exit_node  = dynamic_cast<ExitNodeDrawingElement*>(selected_des_.front());
    *state_node = dynamic_cast<StateDrawingElement*>(selected_des_.front());
    *event_node = dynamic_cast<EventDrawingElement*>(selected_des_.front());

    return true;
}

DrawingElement* StateMachineWindow::GetDrawingElementAtPoint(const POINT& point) const
{
    DrawingElementList::const_reverse_iterator itor;
    for(itor = de_list_.rbegin(); itor != de_list_.rend(); ++itor)
    {
        if((*itor)->HitTest(point))
        {
            return itor->get();
        }
    }

    return NULL;
}

void StateMachineWindow::RemoveDrawingElement(DrawingElement* de)
{
    DrawingElementList::iterator itor;

    // This element might be pointed to by one or more EventDrawingElement objects.
    if(dynamic_cast<EntryNodeDrawingElement*>(de) || dynamic_cast<ExitNodeDrawingElement*>(de) || dynamic_cast<StateDrawingElement*>(de))
    {
        for(itor = de_list_.begin(); itor != de_list_.end(); ++itor)
        {
            EventDrawingElement* event = dynamic_cast<EventDrawingElement*>(itor->get());
            if(event)
            {
                if(de == event->SourceDrawingElement())
                {
                    event->SourceDrawingElement(NULL);
                }
                if(de == event->DestinationDrawingElement())
                {
                    event->DestinationDrawingElement(NULL);
                }
            }
        }
    }

    // Free it's memory
    for(itor = de_list_.begin(); itor != de_list_.end(); ++itor)
    {
        if(itor->get() == de)
        {
            if(dynamic_cast<EntryNodeDrawingElement*>(itor->get()))
            {
                event_handler_->OnStateMachineEntryNodeRemoved((*itor)->Id());
            }
            else if(dynamic_cast<ExitNodeDrawingElement*>(itor->get()))
            {
                event_handler_->OnStateMachineExitNodeRemoved((*itor)->Id());
            }
            else if(dynamic_cast<StateDrawingElement*>(itor->get()))
            {
                event_handler_->OnStateMachineStateNodeRemoved((*itor)->Id());
            }
            else //if(dynamic_cast<EventDrawingElement*>(itor->get()))
            {
                event_handler_->OnStateMachineEventNodeRemoved((*itor)->Id());
            }
            de_list_.erase(itor);
            break;
        }
    }

    // If we just removed an Entry, Exit or State node and this node was connected to
    // one or more EventDrawingElement objects, then those EventDrawingElement objects
    // will have one end of their lines pointing to nothing.  We'll delete these
    // EventDrawingElement objects too.
    itor = de_list_.begin();
    while(itor != de_list_.end())
    {
        EventDrawingElement* event = dynamic_cast<EventDrawingElement*>(itor->get());
        if(event && (event->SourceDrawingElement() == NULL || event->DestinationDrawingElement() == NULL))
        {
            event_handler_->OnStateMachineEventNodeRemoved((*itor)->Id());
            de_list_.erase(itor);
            itor = de_list_.begin();
        }
        else
        {
            ++itor;
        }
    }
}

void StateMachineWindow::SelectNone()
{
    SelectedDrawingElementList::iterator itor;
    for(itor = selected_des_.begin(); itor != selected_des_.end(); ++itor)
    {
        (*itor)->Select(false);
    }
    selected_des_.clear();
}

void StateMachineWindow::Select(DrawingElement* de)
{
    SelectedDrawingElementList::iterator itor = std::find(selected_des_.begin(), selected_des_.end(), de);
    if(itor == selected_des_.end())
    {
        de->Select(true);
        selected_des_.push_back(de);
    }
}

void StateMachineWindow::Deselect(DrawingElement* de)
{
    SelectedDrawingElementList::iterator itor = std::find(selected_des_.begin(), selected_des_.end(), de);
    if(itor != selected_des_.end())
    {
        de->Select(false);
        selected_des_.erase(itor);
    }
}





LRESULT CALLBACK StateMachineWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(window, GWL_USERDATA);
    StateMachineWindow* this_ = reinterpret_cast<StateMachineWindow*>(data);

    switch(msg)
    {
    case WM_CREATE:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(window, GWL_USERDATA, reinterpret_cast<LONG>(create_struct->lpCreateParams));
            this_ = reinterpret_cast<StateMachineWindow*>(create_struct->lpCreateParams);
            break;
        }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(window, &ps);
            this_->OnWindowPaint(dc);
            EndPaint(window, &ps);
            break;
        }
    case WM_LBUTTONDOWN:
        {
            POINT point;
            point.x = GET_X_LPARAM(lparam);
            point.y = GET_Y_LPARAM(lparam);
            this_->OnLmButtonDown(point, (wparam & MK_CONTROL) != 0, (wparam & MK_SHIFT) != 0);
            break;
        }
    case WM_LBUTTONUP:
        {
            POINT point;
            point.x = GET_X_LPARAM(lparam);
            point.y = GET_Y_LPARAM(lparam);
            this_->OnLmButtonUp(point, (wparam & MK_CONTROL) != 0, (wparam & MK_SHIFT) != 0);
            break;
        }
    case WM_LBUTTONDBLCLK:
        this_->event_handler_->OnStateMachineDoubleClick();
        break;
    case WM_RBUTTONDOWN:
        {
            POINT point;
            point.x = GET_X_LPARAM(lparam);
            point.y = GET_Y_LPARAM(lparam);
            this_->OnRmButtonDown(point, (wparam & MK_CONTROL) != 0, (wparam & MK_SHIFT) != 0);
            break;
        }
    case WM_RBUTTONUP:
        {
            POINT point;
            point.x = GET_X_LPARAM(lparam);
            point.y = GET_Y_LPARAM(lparam);
            this_->OnRmButtonUp(point, (wparam & MK_CONTROL) != 0, (wparam & MK_SHIFT) != 0);
            break;
        }
    case WM_MOUSEMOVE:
        {
            POINT point;
            point.x = GET_X_LPARAM(lparam);
            point.y = GET_Y_LPARAM(lparam);
            this_->OnMouseMove(point, (wparam & MK_CONTROL) != 0, (wparam & MK_SHIFT) != 0);
            break;
        }
    case WM_SETCURSOR:
        switch(this_->current_tool_)
        {
        case StateMachineWindow::T_INSERT_ENTRY:
            SetCursor(this_->entry_cursor_);
            return TRUE;
        case StateMachineWindow::T_INSERT_EXIT:
            SetCursor(this_->exit_cursor_);
            return TRUE;
        case StateMachineWindow::T_INSERT_STATE:
            SetCursor(this_->state_cursor_);
            return TRUE;
        case StateMachineWindow::T_INSERT_EVENT:
            SetCursor(this_->event_cursor_);
            return TRUE;
        }
        break;
    case WM_COMMAND:
        switch(LOWORD(wparam))
        {
        case ID_EDIT_SELECT:
            this_->current_tool_ = StateMachineWindow::T_SELECT_NODE;
            break;
        case ID_INSERT_ENTRYNODE40043:
            this_->current_tool_ = StateMachineWindow::T_INSERT_ENTRY;
            break;
        case ID_INSERT_EXITNODE40044:
            this_->current_tool_ = StateMachineWindow::T_INSERT_EXIT;
            break;
        case ID_INSERT_STATENODE40045:
            this_->current_tool_ = StateMachineWindow::T_INSERT_STATE;
            break;
        case ID_INSERT_EVENT40046:
            this_->current_tool_ = StateMachineWindow::T_INSERT_EVENT;
            break;
        case ID_EDIT_REMOVE:
            this_->DeleteSelected();
            break;
        case ID_EDIT_PROPERTIES40042:
            this_->event_handler_->OnStateMachineDoubleClick();
            break;
        }
        break;
    case WM_KEYDOWN:
        switch(wparam)
        {
        case VK_ESCAPE:
            this_->current_tool_ = StateMachineWindow::T_SELECT_NODE;
            break;
        case VK_DELETE:
            this_->DeleteSelected();
            break;
        }
        break;
    }

    return DefWindowProc(window, msg, wparam, lparam);
}





void StateMachineWindow::OnWindowPaint(HDC dc)
{
    RECT rect;
    GetClientRect(handle_, &rect);

    FillRect(double_buffer_dc_, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

    std::for_each(de_list_.begin(), de_list_.end(), boost::bind(&DrawingElement::Draw, _1, double_buffer_dc_));

    if(drawing_event_line_ && anchor_de_)
    {
        POINT cursor_pos;
        GetCursorPos(&cursor_pos);
        ScreenToClient(handle_, &cursor_pos);

        HPEN prev_pen = SelectPen(double_buffer_dc_, event_line_pen_);

        MoveToEx(double_buffer_dc_, anchor_de_->Position().x, anchor_de_->Position().y, NULL);
        LineTo(double_buffer_dc_, cursor_pos.x, cursor_pos.y);

        SelectObject(double_buffer_dc_, prev_pen);
    }

    BitBlt(dc, 0, 0, rect.right, rect.bottom, double_buffer_dc_, 0, 0, SRCCOPY);
}

void StateMachineWindow::OnLmButtonDown(const POINT& point, bool control_held, bool shift_held)
{
    SetCapture(handle_);
    SetFocus(handle_);
    SetCursor(LoadCursor(NULL, IDC_ARROW));

    drawing_event_line_ = false;

    switch(current_tool_)
    {
    case T_SELECT_NODE:
        {
            clicked_on_de_ = GetDrawingElementAtPoint(point);
            if(clicked_on_de_)
            {
                if(shift_held)
                {
                    Select(clicked_on_de_);
                    InvalidateRect(handle_, NULL, FALSE);
                }
                else if(control_held)
                {
                    anchor_de_ = clicked_on_de_;
                }
                else
                {
                    SelectNone();
                    Select(clicked_on_de_);
                    InvalidateRect(handle_, NULL, FALSE);
                }
            }
            else
            {
                if(!control_held && !shift_held)
                {
                    SelectNone();
                    InvalidateRect(handle_, NULL, FALSE);
                }
            }

            SelectedDrawingElementList::iterator itor;
            for(itor = selected_des_.begin(); itor != selected_des_.end(); ++itor)
            {
                POINT offset;
                offset.x = point.x - (*itor)->Position().x;
                offset.y = point.y - (*itor)->Position().y;
                (*itor)->DragOffset(offset);
            }
            break;
        }
    case T_INSERT_ENTRY:
        current_tool_ = T_SELECT_NODE;
        InsertEntryNode(unique_id_++, point);
        break;
    case T_INSERT_EXIT:
        current_tool_ = T_SELECT_NODE;
        InsertExitNode(unique_id_++, point);
        break;
    case T_INSERT_STATE:
        {
            current_tool_ = T_SELECT_NODE;
            SetFocus(handle_);
            InsertStateNode(unique_id_++, point, std::string());
            break;
        }
    case T_INSERT_EVENT:
        {
            clicked_on_de_ = GetDrawingElementAtPoint(point);
            if(clicked_on_de_ && (dynamic_cast<EntryNodeDrawingElement*>(clicked_on_de_) || dynamic_cast<ExitNodeDrawingElement*>(clicked_on_de_) || dynamic_cast<StateDrawingElement*>(clicked_on_de_)))
            {
                drawing_event_line_ = true;
                anchor_de_ = clicked_on_de_;
            }
            else
            {
                MessageBox(handle_, "Click and drag from an Entry, Exit or State node", "Information", MB_OK | MB_ICONINFORMATION);
            }
            break;
        }
    }
}

void StateMachineWindow::OnLmButtonUp(const POINT& point, bool control_held, bool shift_held)
{
    if(GetCapture() == handle_)
    {
        bool repaint = false;

        switch(current_tool_)
        {
        case T_SELECT_NODE:
            {
                DrawingElement* de = GetDrawingElementAtPoint(point);
                if(control_held && de && !items_were_dragged_)
                {
                    de->Selected() ? Deselect(de) : Select(de);
                    InvalidateRect(handle_, NULL, FALSE);
                }
                break;
            }
        case T_INSERT_EVENT:
            {
                current_tool_ = T_SELECT_NODE;
                drawing_event_line_ = false;
                repaint = true;

                DrawingElement* cursor_over_de = GetDrawingElementAtPoint(point);
                if(cursor_over_de)
                {
                    if(cursor_over_de != anchor_de_)
                    {
                        SetFocus(handle_);

                        POINT center_point;
                        center_point.x = anchor_de_->Position().x + ((cursor_over_de->Position().x - anchor_de_->Position().x)/2);
                        center_point.y = anchor_de_->Position().y + ((cursor_over_de->Position().y - anchor_de_->Position().y)/2);

                        InsertEventNode(
                            unique_id_++, center_point, std::string(),
                            anchor_de_->Id(),
                            cursor_over_de->Id());

                        items_were_dragged_ = true;
                    }
                    else
                    {
                        MessageBox(handle_, "Connect the event line to a node other than the starting node", "Information", MB_OK | MB_ICONINFORMATION);
                    }
                }
                else
                {
                    MessageBox(handle_, "You must connect the other end of the line to an Entry, Exit or State node", "Information", MB_OK | MB_ICONINFORMATION);
                }
                break;
            }
        }

        SelectedDrawingElementList::iterator itor;
        for(itor = selected_des_.begin(); itor != selected_des_.end(); ++itor)
        {
            // Now that the drag has finished, send out the update notification
            if(dynamic_cast<EntryNodeDrawingElement*>(*itor))
            {
                EntryNodeDrawingElement* entry = dynamic_cast<EntryNodeDrawingElement*>(*itor);
                event_handler_->OnStateMachineEntryNodeUpdated(entry->Id(), entry->Position());
            }
            else if(dynamic_cast<ExitNodeDrawingElement*>(*itor))
            {
                ExitNodeDrawingElement* exit = dynamic_cast<ExitNodeDrawingElement*>(*itor);
                event_handler_->OnStateMachineExitNodeUpdated(exit->Id(), exit->Position());
            }
            else if(dynamic_cast<StateDrawingElement*>(*itor))
            {
                StateDrawingElement* state = dynamic_cast<StateDrawingElement*>(*itor);
                event_handler_->OnStateMachineStateNodeUpdated(state->Id(), state->Position(), state->Text());
            }
            else //if(dynamic_cast<EventDrawingElement*>(*itor))
            {
                EventDrawingElement* event = dynamic_cast<EventDrawingElement*>(*itor);
                event_handler_->OnStateMachineEventNodeUpdated(
                    event->Id(), event->Position(), event->Text(),
                    event->SourceDrawingElement()->Id(),
                    event->DestinationDrawingElement()->Id());
            }

            repaint = true;
        }

        if(items_were_dragged_)
        {
            event_handler_->OnStateMachineContentsModified();
            items_were_dragged_ = false;
        }
        if(repaint || items_were_dragged_)
        {
            InvalidateRect(handle_, NULL, FALSE);
        }

        clicked_on_de_  = NULL;
        anchor_de_      = NULL;
        ReleaseCapture();
    }
}

void StateMachineWindow::OnRmButtonDown(const POINT& point, bool control_held, bool shift_held)
{
    OnLmButtonDown(point, control_held, shift_held);
}

void StateMachineWindow::OnRmButtonUp(const POINT& point, bool control_held, bool shift_held)
{
    OnLmButtonUp(point, control_held, shift_held);

    HMENU menu = LoadMenu(instance_, MAKEINTRESOURCE(IDR_STATE_MACHINE_MENU));
    HMENU sub_menu = GetSubMenu(menu, 1);

    POINT cursor_pos;
    GetCursorPos(&cursor_pos);

    TrackPopupMenu(sub_menu, TPM_LEFTALIGN | TPM_TOPALIGN, cursor_pos.x, cursor_pos.y, 0, handle_, NULL);
}

void StateMachineWindow::OnMouseMove(const POINT& point, bool control_held, bool shift_held)
{
    if(GetCapture() == handle_)
    {
        if(drawing_event_line_)
        {
            InvalidateRect(handle_, NULL, FALSE);
        }
        else if(clicked_on_de_ && !selected_des_.empty())
        {
            if(control_held && anchor_de_ && !anchor_de_->Selected())
            {
                POINT offset;
                offset.x = point.x - anchor_de_->Position().x;
                offset.y = point.y - anchor_de_->Position().y;
                anchor_de_->DragOffset(offset);

                Select(anchor_de_);
                anchor_de_ = NULL;
            }

            SelectedDrawingElementList::iterator itor;
            for(itor = selected_des_.begin(); itor != selected_des_.end(); ++itor)
            {
                POINT temp;
                temp.x = point.x - (*itor)->DragOffset().x;
                temp.y = point.y - (*itor)->DragOffset().y;
                (*itor)->Position(temp);
            }
            items_were_dragged_ = true;
            InvalidateRect(handle_, NULL, FALSE);
        }
    }
}
