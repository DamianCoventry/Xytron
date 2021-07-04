///////////////////////////////////////////////////////////////////////////////
//
//  PROJECT: Sage
//
//  AUTHOR: Damian Coventry
//
//  CREATION DATE: August 2007
// 
//  COPYRIGHT NOTICE:
//
//      (C) Omenware
//      Created in 2007 as an unpublished copyright work.  All rights reserved.
//      This document and the information it contains is confidential and
//      proprietary to Omenware.  Hence, it may not be  used, copied, reproduced,
//      transmitted, or stored in any form or by any means, electronic,
//      recording, photocopying, mechanical or otherwise, without the prior
//      written permission of Omenware
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
#ifndef INCLUDED_SAGEENGINE
#define INCLUDED_SAGEENGINE

#include "../gfx/gfx.h"
#include "../inp/inp.h"
#include "../aud/aud.h"
#include "../math/vector.h"
#include "../timer/manager.h"
#include "../util/resourcecontext.h"
#include "../win/windowmanager.h"
#include "../sagedocuments/gamedocument.h"

#include "settings.h"
#include "commandrepository.h"
#include "statemachine.h"
#include "world.h"


///////////////////////////////////////////////////////////////////////////////
class SageEngine
    : public Gfx::IWindowEvents
    , public Inp::InputEvents
    , public Settings
{
public:
    SageEngine(HINSTANCE instance);
    ~SageEngine();

    void Run(const std::string& cmd_line);

private:
    // Implementation of Gfx::IWindowEvents
    bool OnWindowClosed(Gfx::Window* window);
    bool OnWindowEraseBackground(Gfx::Window* window, HDC dc);
    void OnWindowActivated(Gfx::Window* window);
    void OnWindowDeactived(Gfx::Window* window);
    void OnWindowOtherMessage(UINT msg, WPARAM wparam, LPARAM lparam);
    void OnWindowMouseLPressed(Gfx::Window* window);
    void OnMouseWheelRotatedBackwards(short delta);
    void OnMouseWheelRotatedForwards(short delta);

    // Inp::InputEvents
    void OnKeyPressed(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnKeyHeld(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnKeyReleased(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnMouseButtonPressed(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnMouseButtonHeld(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnMouseButtonReleased(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnMouseMoved(int x_delta, int y_delta);

    // Settings
    void OnFullscreenGraphicsSettingChanged();
    void OnSoundEnabledSettingChanged();
    void OnMusicEnabledSettingChanged();
    void OnSoundVolumeSettingChanged();
    void OnMusicVolumeSettingChanged();

    bool SetFullscreen();

    void PreLoop();
    void PostLoop();
    void SetModelViewMatrix();

private:
    HINSTANCE instance_;

    std::string app_dir_;

    Gfx::Graphics g_;
    Gfx::Window window_;
    Util::ResourceContext resources_;
    Timer::Manager timer_manager_;

    Inp::Input inp_;
    Inp::Bindings bindings_;

    Aud::Device aud_;
    //Aud::Music mus_tracks_[4];
    //int current_track_;

    bool mouse_captured_;
    std::string cmd_line_;

    //Math::Vector camera_rotation_;
    //Math::Vector camera_position_;

    GameDocument        doc_;
    CommandRepository   cmd_repo_;
    StateMachine        state_machine_;
    World               world_;
};

#endif  // INCLUDED_SAGEENGINE
