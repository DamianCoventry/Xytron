#ifndef INCLUDED_STATEMACHINEWINDOW
#define INCLUDED_STATEMACHINEWINDOW

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>
#include <boost/shared_ptr.hpp>
#include <list>
#include <vector>
#include "arrowline.h"

class DrawingElement
{
public:
    typedef std::list<DrawingElement*> DeConnections;

public:
    DrawingElement(int id, const POINT& position = POINT(), const SIZE& size = SIZE());
    virtual ~DrawingElement();

    void Position(const POINT& position)    { position_ = position; }
    void DragOffset(const POINT& position)  { drag_offset_ = position; }
    void Select(bool select)                { selected_ = select; }
    void Mark(bool mark)                    { mark_ = mark; }

    int Id() const                      { return id_; }
    const POINT& Position() const       { return position_; }
    const POINT& DragOffset() const     { return drag_offset_; }
    bool Selected() const               { return selected_; }
    HPEN SelectedPen() const            { return selected_pen_; }
    HBRUSH SelectedBrush() const        { return selected_brush_; }
    bool Marked() const                 { return mark_; }

    virtual void Draw(HDC dc) = 0;
    virtual bool HitTest(const POINT& point);

protected:
    const SIZE& Size() const        { return size_; }
    void Size(const SIZE& size)     { size_ = size; }

private:
    int id_;
    POINT position_;
    POINT drag_offset_;
    SIZE size_;
    bool selected_;
    bool mark_;
    HPEN selected_pen_;
    HBRUSH selected_brush_;
};

typedef boost::shared_ptr<DrawingElement> DrawingElementPtr;
typedef std::list<DrawingElementPtr> DrawingElementList;
typedef std::list<DrawingElement*> SelectedDrawingElementList;



class EntryNodeDrawingElement
    : public DrawingElement
{
public:
    EntryNodeDrawingElement(int id, const POINT& position, HDC dc);
    ~EntryNodeDrawingElement();
    void Draw(HDC dc);
private:
    HPEN outline_pen_;
    HBRUSH fill_brush_;
    HFONT font_;
    static const std::string entry_text_;
};



class ExitNodeDrawingElement
    : public DrawingElement
{
public:
    ExitNodeDrawingElement(int id, const POINT& position, HDC dc);
    ~ExitNodeDrawingElement();
    void Draw(HDC dc);
private:
    HPEN outline_pen_;
    HBRUSH fill_brush_;
    HFONT font_;
    static const std::string exit_text_;
};



class StateDrawingElement
    : public DrawingElement
{
public:
    StateDrawingElement(int id, const POINT& position, HDC dc);
    ~StateDrawingElement();
    void Text(const std::string& text)  { text_ = text; }
    const std::string& Text() const     { return text_; }
    void Draw(HDC dc);
private:
    HPEN outline_pen_;
    HBRUSH fill_brush_;
    HFONT font_;
    std::string text_;
};



class EventDrawingElement
    : public DrawingElement
{
public:
    EventDrawingElement(int id, HDC dc);
    ~EventDrawingElement();

    void SourceDrawingElement(DrawingElement* source)       { source_ = source; }
    void DestinationDrawingElement(DrawingElement* dest)    { dest_ = dest; }

    DrawingElement* SourceDrawingElement() const        { return source_; }
    DrawingElement* DestinationDrawingElement() const   { return dest_; }

    void Text(const std::string& text)  { text_ = text; }
    const std::string& Text() const     { return text_; }

    void Draw(HDC dc);

private:
    HPEN line_pen_;
    HBRUSH fill_brush_;
    HFONT font_;
    DrawingElement* source_;
    DrawingElement* dest_;
    std::string text_;
};



struct StateMachineWindowEvents
{
    virtual void OnStateMachineContentsModified() = 0;
    virtual void OnStateMachineDoubleClick() = 0;

    virtual void OnStateMachineEntryNodeInserted(int id, const POINT& position) = 0;
    virtual void OnStateMachineEntryNodeRemoved(int id) = 0;
    virtual void OnStateMachineEntryNodeUpdated(int id, const POINT& position) = 0;

    virtual void OnStateMachineExitNodeInserted(int id, const POINT& position) = 0;
    virtual void OnStateMachineExitNodeRemoved(int id) = 0;
    virtual void OnStateMachineExitNodeUpdated(int id, const POINT& position) = 0;

    virtual void OnStateMachineStateNodeInserted(int id, const POINT& position, const std::string& name) = 0;
    virtual void OnStateMachineStateNodeRemoved(int id) = 0;
    virtual void OnStateMachineStateNodeUpdated(int id, const POINT& position, const std::string& name) = 0;

    virtual void OnStateMachineEventNodeInserted(int id, const POINT& position, const std::string& name, int source_id, int dest_id) = 0;
    virtual void OnStateMachineEventNodeRemoved(int id) = 0;
    virtual void OnStateMachineEventNodeUpdated(int id, const POINT& position, const std::string& name, int source_id, int dest_id) = 0;
};


class StateMachineWindow
{
public:
    StateMachineWindow(HINSTANCE instance, HWND parent, StateMachineWindowEvents* event_handler);
    ~StateMachineWindow();

    void SetUniqueIdToHighestId();

    enum Tool { T_SELECT_NODE,  T_INSERT_ENTRY, T_INSERT_EXIT, T_INSERT_STATE, T_INSERT_EVENT };
    void CurrentTool(Tool tool) { current_tool_ = tool; }
    Tool CurrentTool() const    { return current_tool_; }

    bool InsertEntryNode(int id, const POINT& position, bool notify = true, bool repaint = true);
    bool UpdateEntryNode(int id, const POINT& position, bool notify = true, bool repaint = true);

    bool InsertExitNode(int id, const POINT& position, bool notify = true, bool repaint = true);
    bool UpdateExitNode(int id, const POINT& position, bool notify = true, bool repaint = true);

    bool InsertStateNode(int id, const POINT& position, const std::string& name, bool notify = true, bool repaint = true);
    bool UpdateStateNode(int id, const POINT& position, const std::string& name, bool notify = true, bool repaint = true);

    bool InsertEventNode(int id, const POINT& position, const std::string& name, int source_id, int dest_id, bool notify = true, bool repaint = true);
    bool UpdateEventNode(int id, const POINT& position, const std::string& name, int source_id, int dest_id, bool notify = true, bool repaint = true);

    bool RemoveNode(int id, bool notify = true, bool repaint = true);

    void DeleteSelected();

    void Clear();
    void RepositionWithinParent();

    void Enable()   { EnableWindow(handle_, TRUE); }
    void Disable()  { EnableWindow(handle_, FALSE); }
    void Show()     { ShowWindow(handle_, SW_SHOW); }
    void Hide()     { ShowWindow(handle_, SW_HIDE); }
    void Repaint()  { InvalidateRect(handle_, NULL, FALSE); }

    DrawingElementList& DrawingElements() const { return (DrawingElementList&)de_list_; }

    bool GetFirstSelectedDrawingElement(EntryNodeDrawingElement** entry_node,
                                        ExitNodeDrawingElement** exit_node,
                                        StateDrawingElement** state_node,
                                        EventDrawingElement** event_node) const;

private:
    void Register();
    void Unregister();
    void Create(HWND parent);
    void Delete();

    void CreateDoubleBuffer();
    void DeleteDoubleBuffer();

    DrawingElement* GetDrawingElementAtPoint(const POINT& point) const;
    void RemoveDrawingElement(DrawingElement* de);

    void SelectNone();
    void Select(DrawingElement* de);
    void Deselect(DrawingElement* de);

    friend LRESULT CALLBACK StateMachineWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnWindowPaint(HDC dc);
    void OnLmButtonDown(const POINT& point, bool control_held, bool shift_held);
    void OnLmButtonUp(const POINT& point, bool control_held, bool shift_held);
    void OnRmButtonDown(const POINT& point, bool control_held, bool shift_held);
    void OnRmButtonUp(const POINT& point, bool control_held, bool shift_held);
    void OnMouseMove(const POINT& point, bool control_held, bool shift_held);

private:
    static const char* window_title_;
    static const char* class_name_;

    StateMachineWindowEvents* event_handler_;

    int unique_id_;

    HINSTANCE   instance_;
    HWND        handle_;
    HPEN        event_line_pen_;
    Tool        current_tool_;

    HCURSOR     entry_cursor_;
    HCURSOR     exit_cursor_;
    HCURSOR     state_cursor_;
    HCURSOR     event_cursor_;

    HDC double_buffer_dc_;
    HBITMAP double_buffer_bitmap_;

    bool drawing_event_line_;
    bool items_were_dragged_;

    POINT drag_offset_;
    SelectedDrawingElementList selected_des_;
    DrawingElement* clicked_on_de_;
    DrawingElement* anchor_de_;
    DrawingElementList de_list_;
};

typedef boost::shared_ptr<StateMachineWindow> StateMachineWindowPtr;

#endif  // INCLUDED_STATEMACHINEWINDOW
