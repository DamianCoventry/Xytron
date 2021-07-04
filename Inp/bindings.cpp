#include "stdafx.h"
#include "bindings.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>


using namespace Inp;

std::map<std::string, int> Bindings::lookups_;

Bindings::Bindings()
: activated_(false)
{
    handlers_.resize(300);
}

void Bindings::Set(int binding, BindingEventsPtr handler)
{
    handlers_[binding] = handler;
}

void Bindings::Clear(int binding)
{
    handlers_[binding].reset();
}

void Bindings::ClearAll()
{
    handlers_.clear();
    handlers_.resize(300);
}

void Bindings::OnKeyPressed(int key)
{
    if(activated_ && handlers_[key])
    {
        handlers_[key]->OnBindingPressed();
    }
}

void Bindings::OnKeyHeld(int key)
{
    if(activated_ && handlers_[key])
    {
        handlers_[key]->OnBindingHeld();
    }
}

void Bindings::OnKeyReleased(int key)
{
    if(activated_ && handlers_[key])
    {
        handlers_[key]->OnBindingReleased();
    }
}

void Bindings::OnMouseButtonPressed(int button)
{
    if(activated_ && handlers_[MSE_BUTTON0+button])
    {
        handlers_[MSE_BUTTON0+button]->OnBindingPressed();
    }
}

void Bindings::OnMouseButtonHeld(int button)
{
    if(activated_ && handlers_[MSE_BUTTON0+button])
    {
        handlers_[MSE_BUTTON0+button]->OnBindingHeld();
    }
}

void Bindings::OnMouseButtonReleased(int button)
{
    if(activated_ && handlers_[MSE_BUTTON0+button])
    {
        handlers_[MSE_BUTTON0+button]->OnBindingReleased();
    }
}

void Bindings::OnMouseMoved(int x_delta, int y_delta)
{
    if(!activated_)
    {
        return;
    }

    if(x_delta && handlers_[MSE_MOVEX])
    {
        handlers_[MSE_MOVEX]->OnBindingMovedX(x_delta);
    }

    if(y_delta && handlers_[MSE_MOVEY])
    {
        handlers_[MSE_MOVEY]->OnBindingMovedY(y_delta);
    }

    if((x_delta || y_delta) && handlers_[MSE_MOVE])
    {
        handlers_[MSE_MOVE]->OnBindingMoved(x_delta, y_delta);
    }
}

void Bindings::OnMouseWheelMoved(int delta)
{
    if(!activated_)
    {
        return;
    }

    if(delta < 0 && handlers_[MSE_MWHEELUP])
    {
        handlers_[MSE_MWHEELUP]->OnBindingMovedUp(delta);
    }
    else if(delta > 0 && handlers_[MSE_MWHEELDOWN])
    {
        handlers_[MSE_MWHEELDOWN]->OnBindingMovedDown(delta);
    }
}




void Bindings::BuildLookups()
{
    lookups_["KEY_ESCAPE"]                = DIK_ESCAPE;
    lookups_["KEY_1"]                     = DIK_1;
    lookups_["KEY_2"]                     = DIK_2;
    lookups_["KEY_3"]                     = DIK_3;
    lookups_["KEY_4"]                     = DIK_4;
    lookups_["KEY_5"]                     = DIK_5;
    lookups_["KEY_6"]                     = DIK_6;
    lookups_["KEY_7"]                     = DIK_7;
    lookups_["KEY_8"]                     = DIK_8;
    lookups_["KEY_9"]                     = DIK_9;
    lookups_["KEY_0"]                     = DIK_0;
    lookups_["KEY_MINUS"]                 = DIK_MINUS;
    lookups_["KEY_EQUALS"]                = DIK_EQUALS;
    lookups_["KEY_BACK"]                  = DIK_BACK;
    lookups_["KEY_TAB"]                   = DIK_TAB;
    lookups_["KEY_Q"]                     = DIK_Q;
    lookups_["KEY_W"]                     = DIK_W;
    lookups_["KEY_E"]                     = DIK_E;
    lookups_["KEY_R"]                     = DIK_R;
    lookups_["KEY_T"]                     = DIK_T;
    lookups_["KEY_Y"]                     = DIK_Y;
    lookups_["KEY_U"]                     = DIK_U;
    lookups_["KEY_I"]                     = DIK_I;
    lookups_["KEY_O"]                     = DIK_O;
    lookups_["KEY_P"]                     = DIK_P;
    lookups_["KEY_LBRACKET"]              = DIK_LBRACKET;
    lookups_["KEY_RBRACKET"]              = DIK_RBRACKET;
    lookups_["KEY_RETURN"]                = DIK_RETURN;
    lookups_["KEY_LCONTROL"]              = DIK_LCONTROL;
    lookups_["KEY_A"]                     = DIK_A;
    lookups_["KEY_S"]                     = DIK_S;
    lookups_["KEY_D"]                     = DIK_D;
    lookups_["KEY_F"]                     = DIK_F;
    lookups_["KEY_G"]                     = DIK_G;
    lookups_["KEY_H"]                     = DIK_H;
    lookups_["KEY_J"]                     = DIK_J;
    lookups_["KEY_K"]                     = DIK_K;
    lookups_["KEY_L"]                     = DIK_L;
    lookups_["KEY_SEMICOLON"]             = DIK_SEMICOLON;
    lookups_["KEY_APOSTROPHE"]            = DIK_APOSTROPHE;
    lookups_["KEY_GRAVE"]                 = DIK_GRAVE;
    lookups_["KEY_LSHIFT"]                = DIK_LSHIFT;
    lookups_["KEY_BACKSLASH"]             = DIK_BACKSLASH;
    lookups_["KEY_Z"]                     = DIK_Z;
    lookups_["KEY_X"]                     = DIK_X;
    lookups_["KEY_C"]                     = DIK_C;
    lookups_["KEY_V"]                     = DIK_V;
    lookups_["KEY_B"]                     = DIK_B;
    lookups_["KEY_N"]                     = DIK_N;
    lookups_["KEY_M"]                     = DIK_M;
    lookups_["KEY_COMMA"]                 = DIK_COMMA;
    lookups_["KEY_PERIOD"]                = DIK_PERIOD;
    lookups_["KEY_SLASH"]                 = DIK_SLASH;
    lookups_["KEY_RSHIFT"]                = DIK_RSHIFT;
    lookups_["KEY_MULTIPLY"]              = DIK_MULTIPLY;
    lookups_["KEY_LMENU"]                 = DIK_LMENU;
    lookups_["KEY_SPACE"]                 = DIK_SPACE;
    lookups_["KEY_CAPITAL"]               = DIK_CAPITAL;
    lookups_["KEY_F1"]                    = DIK_F1;
    lookups_["KEY_F2"]                    = DIK_F2;
    lookups_["KEY_F3"]                    = DIK_F3;
    lookups_["KEY_F4"]                    = DIK_F4;
    lookups_["KEY_F5"]                    = DIK_F5;
    lookups_["KEY_F6"]                    = DIK_F6;
    lookups_["KEY_F7"]                    = DIK_F7;
    lookups_["KEY_F8"]                    = DIK_F8;
    lookups_["KEY_F9"]                    = DIK_F9;
    lookups_["KEY_F10"]                   = DIK_F10;
    lookups_["KEY_NUMLOCK"]               = DIK_NUMLOCK;
    lookups_["KEY_SCROLL"]                = DIK_SCROLL;
    lookups_["KEY_NUMPAD7"]               = DIK_NUMPAD7;
    lookups_["KEY_NUMPAD8"]               = DIK_NUMPAD8;
    lookups_["KEY_NUMPAD9"]               = DIK_NUMPAD9;
    lookups_["KEY_SUBTRACT"]              = DIK_SUBTRACT;
    lookups_["KEY_NUMPAD4"]               = DIK_NUMPAD4;
    lookups_["KEY_NUMPAD5"]               = DIK_NUMPAD5;
    lookups_["KEY_NUMPAD6"]               = DIK_NUMPAD6;
    lookups_["KEY_ADD"]                   = DIK_ADD;
    lookups_["KEY_NUMPAD1"]               = DIK_NUMPAD1;
    lookups_["KEY_NUMPAD2"]               = DIK_NUMPAD2;
    lookups_["KEY_NUMPAD3"]               = DIK_NUMPAD3;
    lookups_["KEY_NUMPAD0"]               = DIK_NUMPAD0;
    lookups_["KEY_DECIMAL"]               = DIK_DECIMAL;
    lookups_["KEY_OEM_102"]               = DIK_OEM_102;
    lookups_["KEY_F11"]                   = DIK_F11;
    lookups_["KEY_F12"]                   = DIK_F12;
    lookups_["KEY_F13"]                   = DIK_F13;
    lookups_["KEY_F14"]                   = DIK_F14;
    lookups_["KEY_F15"]                   = DIK_F15;
    lookups_["KEY_KANA"]                  = DIK_KANA;
    lookups_["KEY_ABNT_C1"]               = DIK_ABNT_C1;
    lookups_["KEY_CONVERT"]               = DIK_CONVERT;
    lookups_["KEY_NOCONVERT"]             = DIK_NOCONVERT;
    lookups_["KEY_YEN"]                   = DIK_YEN;
    lookups_["KEY_ABNT_C2"]               = DIK_ABNT_C2;
    lookups_["KEY_NUMPADEQUALS"]          = DIK_NUMPADEQUALS;
    lookups_["KEY_PREVTRACK"]             = DIK_PREVTRACK;
    lookups_["KEY_AT"]                    = DIK_AT;
    lookups_["KEY_COLON"]                 = DIK_COLON;
    lookups_["KEY_UNDERLINE"]             = DIK_UNDERLINE;
    lookups_["KEY_KANJI"]                 = DIK_KANJI;
    lookups_["KEY_STOP"]                  = DIK_STOP;
    lookups_["KEY_AX"]                    = DIK_AX;
    lookups_["KEY_UNLABELED"]             = DIK_UNLABELED;
    lookups_["KEY_NEXTTRACK"]             = DIK_NEXTTRACK;
    lookups_["KEY_NUMPADENTER"]           = DIK_NUMPADENTER;
    lookups_["KEY_RCONTROL"]              = DIK_RCONTROL;
    lookups_["KEY_MUTE"]                  = DIK_MUTE;
    lookups_["KEY_CALCULATOR"]            = DIK_CALCULATOR;
    lookups_["KEY_PLAYPAUSE"]             = DIK_PLAYPAUSE;
    lookups_["KEY_MEDIASTOP"]             = DIK_MEDIASTOP;
    lookups_["KEY_VOLUMEDOWN"]            = DIK_VOLUMEDOWN;
    lookups_["KEY_VOLUMEUP"]              = DIK_VOLUMEUP;
    lookups_["KEY_WEBHOME"]               = DIK_WEBHOME;
    lookups_["KEY_NUMPADCOMMA"]           = DIK_NUMPADCOMMA;
    lookups_["KEY_DIVIDE"]                = DIK_DIVIDE;
    lookups_["KEY_SYSRQ"]                 = DIK_SYSRQ;
    lookups_["KEY_RMENU"]                 = DIK_RMENU;
    lookups_["KEY_PAUSE"]                 = DIK_PAUSE;
    lookups_["KEY_HOME"]                  = DIK_HOME;
    lookups_["KEY_UP"]                    = DIK_UP;
    lookups_["KEY_PRIOR"]                 = DIK_PRIOR;
    lookups_["KEY_LEFT"]                  = DIK_LEFT;
    lookups_["KEY_RIGHT"]                 = DIK_RIGHT;
    lookups_["KEY_END"]                   = DIK_END;
    lookups_["KEY_DOWN"]                  = DIK_DOWN;
    lookups_["KEY_NEXT"]                  = DIK_NEXT;
    lookups_["KEY_INSERT"]                = DIK_INSERT;
    lookups_["KEY_DELETE"]                = DIK_DELETE;
    lookups_["KEY_LWIN"]                  = DIK_LWIN;
    lookups_["KEY_RWIN"]                  = DIK_RWIN;
    lookups_["KEY_APPS"]                  = DIK_APPS;
    lookups_["KEY_POWER"]                 = DIK_POWER;
    lookups_["KEY_SLEEP"]                 = DIK_SLEEP;
    lookups_["KEY_WAKE"]                  = DIK_WAKE;
    lookups_["KEY_WEBSEARCH"]             = DIK_WEBSEARCH;
    lookups_["KEY_WEBFAVORITES"]          = DIK_WEBFAVORITES;
    lookups_["KEY_WEBREFRESH"]            = DIK_WEBREFRESH;
    lookups_["KEY_WEBSTOP"]               = DIK_WEBSTOP;
    lookups_["KEY_WEBFORWARD"]            = DIK_WEBFORWARD;
    lookups_["KEY_WEBBACK"]               = DIK_WEBBACK;
    lookups_["KEY_MYCOMPUTER"]            = DIK_MYCOMPUTER;
    lookups_["KEY_MAIL"]                  = DIK_MAIL;
    lookups_["KEY_MEDIASELECT"]           = DIK_MEDIASELECT;
    lookups_["MSE_MOVE"]                  = MSE_MOVE;
    lookups_["MSE_MOVEX"]                 = MSE_MOVEX;
    lookups_["MSE_MOVEY"]                 = MSE_MOVEY;
    lookups_["MSE_BUTTON0"]               = MSE_BUTTON0;
    lookups_["MSE_BUTTON1"]               = MSE_BUTTON1;
    lookups_["MSE_BUTTON2"]               = MSE_BUTTON2;
    lookups_["MSE_MWHEELUP"]              = MSE_MWHEELUP;
    lookups_["MSE_MWHEELDOWN"]            = MSE_MWHEELDOWN;
}

std::string Bindings::ToString(int binding)
{
    switch(binding)
    {
    case DIK_ESCAPE: return "KEY_ESCAPE";
    case DIK_1: return "KEY_1";
    case DIK_2: return "KEY_2";
    case DIK_3: return "KEY_3";
    case DIK_4: return "KEY_4";
    case DIK_5: return "KEY_5";
    case DIK_6: return "KEY_6";
    case DIK_7: return "KEY_7";
    case DIK_8: return "KEY_8";
    case DIK_9: return "KEY_9";
    case DIK_0: return "KEY_0";
    case DIK_MINUS: return "KEY_MINUS";
    case DIK_EQUALS: return "KEY_EQUALS";
    case DIK_BACK: return "KEY_BACK";
    case DIK_TAB: return "KEY_TAB";
    case DIK_Q: return "KEY_Q";
    case DIK_W: return "KEY_W";
    case DIK_E: return "KEY_E";
    case DIK_R: return "KEY_R";
    case DIK_T: return "KEY_T";
    case DIK_Y: return "KEY_Y";
    case DIK_U: return "KEY_U";
    case DIK_I: return "KEY_I";
    case DIK_O: return "KEY_O";
    case DIK_P: return "KEY_P";
    case DIK_LBRACKET: return "KEY_LBRACKET";
    case DIK_RBRACKET: return "KEY_RBRACKET";
    case DIK_RETURN: return "KEY_RETURN";
    case DIK_LCONTROL: return "KEY_LCONTROL";
    case DIK_A: return "KEY_A";
    case DIK_S: return "KEY_S";
    case DIK_D: return "KEY_D";
    case DIK_F: return "KEY_F";
    case DIK_G: return "KEY_G";
    case DIK_H: return "KEY_H";
    case DIK_J: return "KEY_J";
    case DIK_K: return "KEY_K";
    case DIK_L: return "KEY_L";
    case DIK_SEMICOLON: return "KEY_SEMICOLON";
    case DIK_APOSTROPHE: return "KEY_APOSTROPHE";
    case DIK_GRAVE: return "KEY_GRAVE";
    case DIK_LSHIFT: return "KEY_LSHIFT";
    case DIK_BACKSLASH: return "KEY_BACKSLASH";
    case DIK_Z: return "KEY_Z";
    case DIK_X: return "KEY_X";
    case DIK_C: return "KEY_C";
    case DIK_V: return "KEY_V";
    case DIK_B: return "KEY_B";
    case DIK_N: return "KEY_N";
    case DIK_M: return "KEY_M";
    case DIK_COMMA: return "KEY_COMMA";
    case DIK_PERIOD: return "KEY_PERIOD";
    case DIK_SLASH: return "KEY_SLASH";
    case DIK_RSHIFT: return "KEY_RSHIFT";
    case DIK_MULTIPLY: return "KEY_MULTIPLY";
    case DIK_LMENU: return "KEY_LMENU";
    case DIK_SPACE: return "KEY_SPACE";
    case DIK_CAPITAL: return "KEY_CAPITAL";
    case DIK_F1: return "KEY_F1";
    case DIK_F2: return "KEY_F2";
    case DIK_F3: return "KEY_F3";
    case DIK_F4: return "KEY_F4";
    case DIK_F5: return "KEY_F5";
    case DIK_F6: return "KEY_F6";
    case DIK_F7: return "KEY_F7";
    case DIK_F8: return "KEY_F8";
    case DIK_F9: return "KEY_F9";
    case DIK_F10: return "KEY_F10";
    case DIK_NUMLOCK: return "KEY_NUMLOCK";
    case DIK_SCROLL: return "KEY_SCROLL";
    case DIK_NUMPAD7: return "KEY_NUMPAD7";
    case DIK_NUMPAD8: return "KEY_NUMPAD8";
    case DIK_NUMPAD9: return "KEY_NUMPAD9";
    case DIK_SUBTRACT: return "KEY_SUBTRACT";
    case DIK_NUMPAD4: return "KEY_NUMPAD4";
    case DIK_NUMPAD5: return "KEY_NUMPAD5";
    case DIK_NUMPAD6: return "KEY_NUMPAD6";
    case DIK_ADD: return "KEY_ADD";
    case DIK_NUMPAD1: return "KEY_NUMPAD1";
    case DIK_NUMPAD2: return "KEY_NUMPAD2";
    case DIK_NUMPAD3: return "KEY_NUMPAD3";
    case DIK_NUMPAD0: return "KEY_NUMPAD0";
    case DIK_DECIMAL: return "KEY_DECIMAL";
    case DIK_OEM_102: return "KEY_OEM_102";
    case DIK_F11: return "KEY_F11";
    case DIK_F12: return "KEY_F12";
    case DIK_F13: return "KEY_F13";
    case DIK_F14: return "KEY_F14";
    case DIK_F15: return "KEY_F15";
    case DIK_KANA: return "KEY_KANA";
    case DIK_ABNT_C1: return "KEY_ABNT_C1";
    case DIK_CONVERT: return "KEY_CONVERT";
    case DIK_NOCONVERT: return "KEY_NOCONVERT";
    case DIK_YEN: return "KEY_YEN";
    case DIK_ABNT_C2: return "KEY_ABNT_C2";
    case DIK_NUMPADEQUALS: return "KEY_NUMPADEQUALS";
    case DIK_PREVTRACK: return "KEY_PREVTRACK";
    case DIK_AT: return "KEY_AT";
    case DIK_COLON: return "KEY_COLON";
    case DIK_UNDERLINE: return "KEY_UNDERLINE";
    case DIK_KANJI: return "KEY_KANJI";
    case DIK_STOP: return "KEY_STOP";
    case DIK_AX: return "KEY_AX";
    case DIK_UNLABELED: return "KEY_UNLABELED";
    case DIK_NEXTTRACK: return "KEY_NEXTTRACK";
    case DIK_NUMPADENTER: return "KEY_NUMPADENTER";
    case DIK_RCONTROL: return "KEY_RCONTROL";
    case DIK_MUTE: return "KEY_MUTE";
    case DIK_CALCULATOR: return "KEY_CALCULATOR";
    case DIK_PLAYPAUSE: return "KEY_PLAYPAUSE";
    case DIK_MEDIASTOP: return "KEY_MEDIASTOP";
    case DIK_VOLUMEDOWN: return "KEY_VOLUMEDOWN";
    case DIK_VOLUMEUP: return "KEY_VOLUMEUP";
    case DIK_WEBHOME: return "KEY_WEBHOME";
    case DIK_NUMPADCOMMA: return "KEY_NUMPADCOMMA";
    case DIK_DIVIDE: return "KEY_DIVIDE";
    case DIK_SYSRQ: return "KEY_SYSRQ";
    case DIK_RMENU: return "KEY_RMENU";
    case DIK_PAUSE: return "KEY_PAUSE";
    case DIK_HOME: return "KEY_HOME";
    case DIK_UP: return "KEY_UP";
    case DIK_PRIOR: return "KEY_PRIOR";
    case DIK_LEFT: return "KEY_LEFT";
    case DIK_RIGHT: return "KEY_RIGHT";
    case DIK_END: return "KEY_END";
    case DIK_DOWN: return "KEY_DOWN";
    case DIK_NEXT: return "KEY_NEXT";
    case DIK_INSERT: return "KEY_INSERT";
    case DIK_DELETE: return "KEY_DELETE";
    case DIK_LWIN: return "KEY_LWIN";
    case DIK_RWIN: return "KEY_RWIN";
    case DIK_APPS: return "KEY_APPS";
    case DIK_POWER: return "KEY_POWER";
    case DIK_SLEEP: return "KEY_SLEEP";
    case DIK_WAKE: return "KEY_WAKE";
    case DIK_WEBSEARCH: return "KEY_WEBSEARCH";
    case DIK_WEBFAVORITES: return "KEY_WEBFAVORITES";
    case DIK_WEBREFRESH: return "KEY_WEBREFRESH";
    case DIK_WEBSTOP: return "KEY_WEBSTOP";
    case DIK_WEBFORWARD: return "KEY_WEBFORWARD";
    case DIK_WEBBACK: return "KEY_WEBBACK";
    case DIK_MYCOMPUTER: return "KEY_MYCOMPUTER";
    case DIK_MAIL: return "KEY_MAIL";
    case DIK_MEDIASELECT: return "KEY_MEDIASELECT";
    case MSE_MOVE: return "MSE_MOVE";
    case MSE_MOVEX: return "MSE_MOVEX";
    case MSE_MOVEY: return "MSE_MOVEY";
    case MSE_BUTTON0: return "MSE_BUTTON0";
    case MSE_BUTTON1: return "MSE_BUTTON1";
    case MSE_BUTTON2: return "MSE_BUTTON2";
    case MSE_MWHEELUP: return "MSE_MWHEELUP";
    case MSE_MWHEELDOWN: return "MSE_MWHEELDOWN";
    }
    return std::string();
}

int Bindings::ToBinding(const std::string& text)
{
    std::map<std::string, int>::iterator i = lookups_.find(text);
    return i == lookups_.end() ? -1 : i->second;
}
