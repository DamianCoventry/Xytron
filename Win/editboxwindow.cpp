#include "stdafx.h"
#include "editboxwindow.h"
#include "../inp/input.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

using namespace Win;

EditBoxWindow::EditBoxWindow(WindowManager* manager, Window* parent)
: Window(manager, parent)
, color_(1.0f, 1.0f, 1.0f)
, first_char_(0)
, cursor_index_(0)
, cursor_blink_(false)
, cursor_time_(0.0f)
, top_margin_(4.0f)
, horiz_margin_(10.0f)
{
}

void EditBoxWindow::Enabled(bool enabled)
{
    Window::Enabled(enabled);
    CurrentFrame(enabled ? 0 : 1);
}

void EditBoxWindow::Think(float time_delta)
{
    cursor_time_ += time_delta;
    if(cursor_time_ >= 0.25f)
    {
        cursor_time_ = 0.0f;
        cursor_blink_ = !cursor_blink_;
    }
}

void EditBoxWindow::Draw2d(const Gfx::Graphics& g)
{
    Window::Draw2d(g);

    if(Visible() && font_ && ClientImage())
    {
        Math::Vector offset(Position());

        Window* current = Parent();
        while(current)
        {
            offset += current->Position();
            current = current->Parent();
        }

        offset.x_ += horiz_margin_;
        offset.y_ += top_margin_;

        float pixel_width = ClientImage()->FrameWidth() - (2*horiz_margin_);

        unsigned int num_chars = font_->NumChars(text_.substr(first_char_), pixel_width);
        font_->DrawString(offset, color_, text_.substr(first_char_, num_chars));

        if(cursor_blink_ && Parent()->GetFocus() == shared_from_this())
        {
            offset.x_ += font_->PixelWidth(text_.substr(first_char_, cursor_index_-first_char_));
            font_->DrawString(offset, color_, "_");
        }
    }
}

bool EditBoxWindow::OnKeyPressed(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(Window::Enabled() && Visible())
    {
        switch(key)
        {
        case DIK_ESCAPE:
            text_.clear();
            cursor_index_ = 0;
            UpdateFirstChar();
            break;
        case DIK_RETURN:
            if(return_pressed_handler_)
            {
                return_pressed_handler_();
            }
            break;
        case DIK_BACK:
            if(cursor_index_ > 0)
            {
                cursor_index_--;
                text_.erase(cursor_index_, 1);
                UpdateFirstChar();
            }
            return true;
        case DIK_DELETE:
            text_.erase(cursor_index_, 1);
            return true;
        case DIK_HOME:
            cursor_index_ = 0;
            UpdateFirstChar();
            return true;
        case DIK_END:
            cursor_index_ = text_.size();
            UpdateFirstChar();
            return true;
        case DIK_LEFT:
            if(ctrl_held)
            {
                MoveCursorToPrevWord();
            }
            else
            {
                if(cursor_index_ > 0)
                {
                    cursor_index_--;
                }
            }
            UpdateFirstChar();
            return true;
        case DIK_RIGHT:
            if(ctrl_held)
            {
                MoveCursorToNextWord();
            }
            else
            {
                if(++cursor_index_ > text_.size())
                {
                    cursor_index_ = text_.size();
                }
            }
            UpdateFirstChar();
            return true;
        default:
            {
                char c = Inp::Input::KeyToChar(key, shift_held);
                if(c)
                {
                    text_.insert(cursor_index_, 1, c);
                    cursor_index_++;
                    UpdateFirstChar();
                    return true;
                }
                break;
            }
        }
    }
    return false;
}

void EditBoxWindow::UpdateFirstChar()
{
    if(cursor_index_ < first_char_)
    {
        first_char_ = unsigned int(cursor_index_);
    }
    else
    {
        float pixel_width = ClientImage()->FrameWidth() - (2*horiz_margin_);
        unsigned int difference = unsigned int(cursor_index_) - first_char_;
        unsigned int num_chars = font_->NumChars(text_.substr(first_char_), pixel_width);
        if(difference >= num_chars)
        {
            first_char_ = unsigned int(cursor_index_) - num_chars;
        }
    }
}

void EditBoxWindow::MoveCursorToNextWord()
{
    char c = text_[cursor_index_];
    if(c == ' ' || c == '\t')
    {
        // Move to the next non-whitespace char
        while(c == ' ' || c == '\t')
        {
            cursor_index_++;
            if(cursor_index_ > text_.size())
            {
                cursor_index_ = text_.size();
                return;
            }
            c = text_[cursor_index_];
        }
    }
    else
    {
        // Move to the next whitespace char
        while(c != ' ' && c != '\t')
        {
            cursor_index_++;
            if(cursor_index_ > text_.size())
            {
                cursor_index_ = text_.size();
                return;
            }
            c = text_[cursor_index_];
        }

        // Move to the next non-whitespace char
        while(c == ' ' || c == '\t')
        {
            cursor_index_++;
            if(cursor_index_ > text_.size())
            {
                cursor_index_ = text_.size();
                return;
            }
            c = text_[cursor_index_];
        }
    }
}

void EditBoxWindow::MoveCursorToPrevWord()
{
    if(cursor_index_ == text_.size())
    {
        // Move to the prev whitespace char
        char c;
        do
        {
            if(cursor_index_ == 0)
            {
                return;
            }
            cursor_index_--;
            c = text_[cursor_index_];
        }
        while(c != ' ' && c != '\t');

        cursor_index_++;
        return;
    }

    char c = text_[cursor_index_];
    if(c == ' ' || c == '\t')
    {
        // Move to the prev non-whitespace char
        while(c == ' ' || c == '\t')
        {
            if(cursor_index_ == 0)
            {
                return;
            }
            cursor_index_--;
            c = text_[cursor_index_];
        }

        // Move to the prev whitespace char
        while(c != ' ' && c != '\t')
        {
            if(cursor_index_ == 0)
            {
                return;
            }
            cursor_index_--;
            c = text_[cursor_index_];
        }

        cursor_index_++;
    }
    else
    {
        // Move to the prev whitespace char
        while(c != ' ' && c != '\t')
        {
            if(cursor_index_ == 0)
            {
                return;
            }
            cursor_index_--;
            c = text_[cursor_index_];
        }

        // Move to the prev non-whitespace char
        while(c == ' ' || c == '\t')
        {
            if(cursor_index_ == 0)
            {
                return;
            }
            cursor_index_--;
            c = text_[cursor_index_];
        }

        // Move to the prev whitespace char
        while(c != ' ' && c != '\t')
        {
            if(cursor_index_ == 0)
            {
                return;
            }
            cursor_index_--;
            c = text_[cursor_index_];
        }

        cursor_index_++;
    }
}
