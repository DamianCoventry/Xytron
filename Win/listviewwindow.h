#ifndef INCLUDED_WIN_LISTVIEWWINDOW
#define INCLUDED_WIN_LISTVIEWWINDOW

#include "../gfx/gfx.h"
#include "window.h"

#include <string>
#include <vector>

namespace Win
{

class ListViewWindow
    : public Window
{
public:
    ListViewWindow(WindowManager* manager, Window* parent = NULL);
    ~ListViewWindow();

    void SetHeaderTextColor(const Gfx::Color& color)                { header_text_color_ = color; }
    void SetRowTextColor(int row_index, const Gfx::Color& color);
    void SetRowHoverColor(int row_index, const Gfx::Color& color)   { row_hover_color_ = color; }
    void SetRowDefaultColor(const Gfx::Color& color)                { default_row_color_ = color; }

    void SetHeaderFont(Gfx::FontPtr font)       { header_font_ = font; }
    void SetRowFont(Gfx::FontPtr font)          { row_font_ = font; }

    void SetTopMargin(float top_margin)     { top_margin_ = top_margin; }
    void SetLeftMargin(float left_margin)   { left_margin_ = left_margin; }

    int AddColumn(const std::string& name, float pixel_width);

    void Enabled(bool enabled);

    int AddItem(const std::string& text);
    void SetSubItemText(int row_index, int col_index, const std::string& text);

    void ClearItems() { rows_.clear(); }

    void Draw2d(const Gfx::Graphics& g);

    bool OnMouseMoved(int x_delta, int y_delta);
    void OnMouseLeave();

private:
    Gfx::Color header_text_color_;
    Gfx::Color default_row_color_;
    Gfx::Color row_hover_color_;

    float top_margin_;
    float left_margin_;
    int hover_index_;

    Gfx::FontPtr header_font_;
    Gfx::FontPtr row_font_;

    struct HeaderColumn
    {
        std::string text_;
        float pixel_width_;
    };
    typedef std::vector<HeaderColumn> HeaderColumns;
    HeaderColumns header_columns_;

    struct Row
    {
        std::vector<std::string> text_;
        Gfx::Color color_;
    };
    typedef std::vector<Row> Rows;
    Rows rows_;
};

typedef boost::shared_ptr<ListViewWindow> ListViewWindowPtr;

}       // namespace Win

#endif  // INCLUDED_WIN_LISTVIEWWINDOW
