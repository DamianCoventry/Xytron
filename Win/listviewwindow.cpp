#include "stdafx.h"
#include "listviewwindow.h"
#include "windowmanager.h"

using namespace Win;

ListViewWindow::ListViewWindow(WindowManager* manager, Window* parent)
: Window(manager, parent)
, header_text_color_(1.00f, 1.00f, 0.00f)
, default_row_color_(0.75f, 0.75f, 0.75f)
, row_hover_color_(1.00f, 0.00f, 0.00f)
, top_margin_(8.0f)
, left_margin_(8.0f)
, hover_index_(-1)
{
}

ListViewWindow::~ListViewWindow()
{
}

void ListViewWindow::Enabled(bool enabled)
{
    Window::Enabled(enabled);
    CurrentFrame(enabled ? 0 : 1);
}

void ListViewWindow::SetRowTextColor(int row_index, const Gfx::Color& color)
{
    if(row_index >= 0 && row_index < static_cast<int>(rows_.size()))
    {
        rows_[row_index].color_ = color;
    }
}

int ListViewWindow::AddColumn(const std::string& name, float pixel_width)
{
    HeaderColumn column;
    column.text_        = name;
    column.pixel_width_ = pixel_width;
    int col_index = static_cast<int>(header_columns_.size());
    header_columns_.push_back(column);
    return col_index;
}

int ListViewWindow::AddItem(const std::string& text)
{
    if(header_columns_.empty())
    {
        return -1;
    }
    Row row;
    row.text_.resize(header_columns_.size());
    row.text_[0]    = text;
    row.color_      = default_row_color_;
    int row_index = static_cast<int>(rows_.size());
    rows_.push_back(row);
    return row_index;
}

void ListViewWindow::SetSubItemText(int row_index, int col_index, const std::string& text)
{
    if(row_index >= 0 && row_index < static_cast<int>(rows_.size()))
    {
        if(col_index >= 0 && col_index < static_cast<int>(rows_[row_index].text_.size()))
        {
            rows_[row_index].text_[col_index] = text;
        }
    }
}

void ListViewWindow::Draw2d(const Gfx::Graphics& g)
{
    if(!Visible())
    {
        return;
    }

    Window::Draw2d(g);

    Math::Vector offset(Position());

    Window* current = Parent();
    while(current)
    {
        offset += current->Position();
        current = current->Parent();
    }

    Rows::iterator row_itor;
    HeaderColumns::iterator header_itor;

    // Draw the header text
    if(header_font_)
    {
        Math::Vector position(offset);
        position.x_ += left_margin_;
        position.y_ += top_margin_;

        HeaderColumns::iterator header_itor;
        for(header_itor = header_columns_.begin(); header_itor != header_columns_.end(); ++header_itor)
        {
            header_font_->DrawString(position, header_text_color_, header_itor->text_);
            position.x_ += header_itor->pixel_width_;
        }
    }

    // Draw all the row text
    if(row_font_)
    {
        Math::Vector position(offset);
        position.y_ += header_font_->CharHeight() + top_margin_;

        int row_index = 0;
        for(row_itor = rows_.begin(); row_itor != rows_.end(); ++row_itor, ++row_index)
        {
            position.x_ = offset.x_ + left_margin_;
            int col_index = 0;
            for(header_itor = header_columns_.begin(); header_itor != header_columns_.end(); ++header_itor, ++col_index)
            {
                int num_chars = row_font_->NumChars(row_itor->text_[col_index], header_itor->pixel_width_);
                std::string sub_string(row_itor->text_[col_index].substr(0, num_chars));

                if(row_index == hover_index_)
                {
                    row_font_->DrawString(position, row_hover_color_, sub_string);
                }
                else
                {
                    row_font_->DrawString(position, row_itor->color_, sub_string);
                }

                position.x_ += header_itor->pixel_width_;
            }

            position.y_ += row_font_->CharHeight();
        }
    }
}

bool ListViewWindow::OnMouseMoved(int x_delta, int y_delta)
{
    if(!Window::Enabled() || !Visible())
    {
        return false;
    }

    if(row_font_)
    {
        Math::Vector cursor_pos(ToClientCoords(Manager()->GetMouseCursor().Position()));
        hover_index_ = int((cursor_pos.y_ - (header_font_->CharHeight() + top_margin_)) / row_font_->CharHeight());
        return true;
    }

    return false;
}

void ListViewWindow::OnMouseLeave()
{
    if(!Window::Enabled() || !Visible())
    {
        return;
    }

    hover_index_ = -1;
}
