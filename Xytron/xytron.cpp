///////////////////////////////////////////////////////////////////////////////
//
//  PROJECT: Sage
//
//  AUTHOR: Damian Coventry
//
//  CREATION DATE: June-July 2007
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
#include "stdafx.h"
#include "xytron.h"
#include "resource.h"
#include "../util/log.h"

#include "statepregame.h"
#include "stateprewave.h"
#include "statewave.h"
#include "statepostwave.h"
#include "statepaused.h"
#include "stategetready.h"
#include "stategameover.h"
#include "stategamewon.h"

#include "basicshotentity.h"
#include "sitandshootenemyentity.h"
#include "railenemyentity.h"
#include "sprayshootenemyentity.h"

#include "statusbarleft.h"
#include "statusbarright.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <time.h>
#include <gl/gl.h>

#include <boost/lexical_cast.hpp>
#include <sstream>

const unsigned int Xytron::MAX_WAVES = 30;

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const int WM_MUSIC_TRACK_0      = WM_USER+0x0100;
    const int WM_MUSIC_TRACK_1      = WM_USER+0x0101;
    const int WM_MUSIC_TRACK_2      = WM_USER+0x0102;
    const int WM_MUSIC_TRACK_3      = WM_USER+0x0103;

    const std::string HIGHSCORES_FILENAME   = "Xytron.highscores";
    const std::string SETTINGS_FILENAME     = "Xytron.settings";
}

///////////////////////////////////////////////////////////////////////////////
Xytron::Xytron(HINSTANCE instance)
: instance_(instance)
, window_(instance)
, resources_(&aud_)
, invulnerable_(false)
, mouse_captured_(true)
, current_wave_(0)
, current_track_(-1)
, current_background_(0)
{
    Log::Open("Xytron.log");
    CoInitialize(NULL);

    srand(unsigned int(time(NULL)));
    Gfx::Window::Register(instance, IDI_GAME);
}

///////////////////////////////////////////////////////////////////////////////
Xytron::~Xytron()
{
    FreeMemory();

    if(window_.IsFullscreenStyle())
        Gfx::DisplayModes::SetModeFromRegistry();

    aud_.Shutdown();
    inp_.Shutdown();
    g_.Shutdown();
    window_.Delete();

    Gfx::Window::Unregister(instance_);
    CoUninitialize();
    Log::Close();
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::Run()
{
    highscores_.Load(HIGHSCORES_FILENAME);
    Load(SETTINGS_FILENAME);

    if(FullscreenGraphics())
    {
        if(SetFullscreen())
        {
            window_.CreateFullscreen(800, 600, "Xytron", this);
        }
        else
        {
            FullscreenGraphics(false);
            window_.CreateWindowed(800, 600, "Xytron", this);
        }
    }
    else
    {
        window_.CreateWindowed(800, 600, "Xytron", this);
    }
    window_.Show();

    RECT window_center;
    GetWindowRect(window_.Handle(), &window_center);
    window_center.left += (window_center.right - window_center.left) / 2;
    window_center.top  += (window_center.bottom - window_center.top) / 2;

    Gfx::Viewport vp(0, 0, 800, 600);
    vp.Set();

    g_.Initialise(window_.Handle(), 800.0f, 600.0f, 32, 24);
    g_.ClearBackBuffer();
    g_.PageFlip();
    g_.ClearBackBuffer();
    g_.PageFlip();
    glDisable(GL_DEPTH_TEST);

    inp_.Initialise(instance_, window_.Handle(), this);

    try
    {
        aud_.Initialise(window_.Handle(), NULL, 22050, 16, 1);
        SoundEnabled() ? aud_.Enable() : aud_.Disable();
    }
    catch(std::exception& e)
    {
        aud_.Disable();
        LOG("Caught the exception [" << e.what() << "], continuing without music.");
    }

    // Load the fonts now because they're used on the loading screen.
    fnt_main_ = resources_.LoadFont("Fonts/Trebuchet MS12Regular.font");
    resources_.LoadFont("Fonts/Impact18Regular.font");

    // Load an image for the loading screen
    Gfx::ImagePtr img_loading = Gfx::Image::CreateFromFile("Images/LoadingScreen.png", 0, 0, 800, 600);
    img_loading->DrawCentered(false);
    g_.Set2d();

    // This object wraps up the ability to display a loading screen in
    // one easy to use interface.
    LoadingScreen loading_screen(g_, img_loading, fnt_main_);
    loading_screen.Update(0);

    state_pre_game_.reset(new StatePreGame(this, this, &timer_manager_, this));
    state_pre_wave_.reset(new StatePreWave(this, this, &timer_manager_));
    state_wave_.reset(new StateWave(this, this, &timer_manager_));
    state_post_wave_.reset(new StatePostWave(this, this, &timer_manager_));
    state_paused_.reset(new StatePaused(this, this, &timer_manager_, this));
    state_get_ready_.reset(new StateGetReady(this, this, &timer_manager_));
    state_game_over_.reset(new StateGameOver(this, this, &timer_manager_));
    state_game_won_.reset(new StateGameWon(this, this, &timer_manager_));
    loading_screen.Update(5);

    LoadResources(loading_screen);

    loading_screen.Update(95);
    state_pre_game_->LoadResources(resources_);
    state_pre_wave_->LoadResources(resources_);
    state_wave_->LoadResources(resources_);
    state_post_wave_->LoadResources(resources_);
    state_paused_->LoadResources(resources_);
    state_get_ready_->LoadResources(resources_);
    state_game_over_->LoadResources(resources_);
    state_game_won_->LoadResources(resources_);

    loading_screen.Update(100);
    loading_screen.Release();

    current_state_ = state_pre_game_;
    current_state_->OnStateBegin();

    PlayNextMusicTrack();

    if(!FullscreenGraphics())
    {
        window_.SetTitle("Xytron - [Press Ctrl+Alt to release the mouse cursor]");
    }

    unsigned int fps = 0;
    unsigned long now, prev_time, time_stamp;
    unsigned long fade_delta;
    float time_delta;
    fps_string_ = "0";

    MSG msg;
    prev_time = time_stamp = timeGetTime();
    for( ; ; )
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        now = timeGetTime();
        time_delta = float(now - prev_time) / 1000.0f;

        inp_.FrameBegin();
        current_state_->FrameBegin();

        if(mouse_captured_ && GetForegroundWindow() == window_.Handle() && !IsIconic(window_.Handle()))
        {
            SetCursorPos(window_center.left, window_center.top);
        }

        timer_manager_.ProcessExpiredTimeouts();
        inp_.Poll();
        aud_.Update3dSounds();

        window_manager_.Think(time_delta);
        current_state_->Think(time_delta);
        current_state_->Move(time_delta);
        current_state_->Collide();

        g_.Set2d();
        current_state_->Draw2d(g_);
        window_manager_.Draw2d(g_);
        fnt_main_->DrawString(Math::Vector(750.0f, 575.0f, 0.0f), Gfx::Color(1.0f, 1.0f, 0.0f), fps_string_);

        if(screen_flash_alpha_ > 0.0f)
        {
            g_.DrawColoredQuad(Math::Vector(0.0f, 0.0f, 0.0f), Math::Vector(800.0f, 600.0f, 0.0f), screen_flash_color_, screen_flash_alpha_);

            fade_delta = timeGetTime() - alpha_fade_start_time_;
            screen_flash_alpha_ = 1.0f - (float(fade_delta) / float(alpha_fade_time_));

            std::ostringstream oss;
            oss << "fade_delta = [" << fade_delta << "], screen_flash_alpha_ = [" << screen_flash_alpha_ << "]\n";
            ::OutputDebugString(oss.str().c_str());
        }

        g_.PageFlip();

        current_state_->FrameEnd();
        inp_.FrameEnd();
        if(pending_state_)
        {
            current_state_->OnStateEnd();
            current_state_ = pending_state_;
            current_state_->OnStateBegin();
            pending_state_.reset();
        }

        fps++;
        if(now - time_stamp >= 1000)
        {
            fps_string_ = boost::lexical_cast<std::string>(fps);
            time_stamp = now;
            fps = 0;
        }
        prev_time = now;

        Sleep(1);
    }

    window_.Hide();

    img_loading.reset();
    FreeMemory();

    if (window_.IsFullscreenStyle())
        Gfx::DisplayModes::SetModeFromRegistry();

    aud_.Shutdown();
    inp_.Shutdown();
    g_.Shutdown();
    window_.Delete();

    Save(SETTINGS_FILENAME);
    highscores_.Save(HIGHSCORES_FILENAME);
}




///////////////////////////////////////////////////////////////////////////////
void Xytron::Clear()
{
    ent_player_->Position(Math::Vector(-100.0f, -100.0f, 0.0f));
    ent_enemies_.clear();
    ent_player_shots_.clear();
    ent_enemy_shots_.clear();
    ent_explosions_.clear();
    ent_pickups_.clear();
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::FrameBegin()
{
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::Think(float time_delta)
{
    Gfx::EntityList::iterator itor;
    for(itor = ent_enemies_.begin(); itor != ent_enemies_.end(); ++itor)
    {
        (*itor)->Think(time_delta);
    }

    for(itor = ent_player_shots_.begin(); itor != ent_player_shots_.end(); ++itor)
    {
        (*itor)->Think(time_delta);
    }

    for(itor = ent_enemy_shots_.begin(); itor != ent_enemy_shots_.end(); ++itor)
    {
        (*itor)->Think(time_delta);
    }

    for(itor = ent_explosions_.begin(); itor != ent_explosions_.end(); ++itor)
    {
        (*itor)->Think(time_delta);
    }

    for(itor = ent_pickups_.begin(); itor != ent_pickups_.end(); ++itor)
    {
        (*itor)->Think(time_delta);
    }

    WeaponList::iterator wep;
    for(wep = wep_thinkable_.begin(); wep != wep_thinkable_.end(); ++wep)
    {
        (*wep)->Think(time_delta);
    }

    ent_player_->Think(time_delta);
    ent_statusbar_left_->Think(time_delta);
    ent_statusbar_right_->Think(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::Move(float time_delta)
{
    ent_starfield_.Move(time_delta);

    Gfx::EntityList::iterator itor;
    for(itor = ent_enemies_.begin(); itor != ent_enemies_.end(); ++itor)
    {
        (*itor)->Move(time_delta);
    }

    for(itor = ent_player_shots_.begin(); itor != ent_player_shots_.end(); ++itor)
    {
        (*itor)->Move(time_delta);
    }

    for(itor = ent_enemy_shots_.begin(); itor != ent_enemy_shots_.end(); ++itor)
    {
        (*itor)->Move(time_delta);
    }

    for(itor = ent_explosions_.begin(); itor != ent_explosions_.end(); ++itor)
    {
        (*itor)->Move(time_delta);
    }

    for(itor = ent_pickups_.begin(); itor != ent_pickups_.end(); ++itor)
    {
        (*itor)->Move(time_delta);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::Collide()
{
    Gfx::EntityList::iterator itor;
    PickupEntity* pickup;

    for(itor = ent_player_shots_.begin(); itor != ent_player_shots_.end(); ++itor)
    {
        (*itor)->CollideWithEnemyEntities(ent_enemies_);
    }

    PlayerEntity* player = static_cast<PlayerEntity*>(ent_player_.get());

    if(!invulnerable_ && !player->Dead() && player->Shootable())
    {
        for(itor = ent_enemies_.begin(); itor != ent_enemies_.end(); ++itor)
        {
            (*itor)->CollideWithPlayer(ent_player_);
        }

        for(itor = ent_enemy_shots_.begin(); itor != ent_enemy_shots_.end(); ++itor)
        {
            (*itor)->CollideWithPlayer(ent_player_);
        }
    }

    if(!player->Dead())
    {
        for(itor = ent_pickups_.begin(); itor != ent_pickups_.end(); ++itor)
        {
            pickup = static_cast<PickupEntity*>(itor->get());
            pickup->CollideWithPlayer(player);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::Draw()
{
    if(!img_backgrounds_[0])
    {
        return;
    }

    img_backgrounds_[current_background_]->Draw2d(g_, Math::Vector(100.0f, 0.0f, 0.0f));
    ent_starfield_.Draw2d(g_);

    Gfx::EntityList::iterator itor;
    for(itor = ent_player_shots_.begin(); itor != ent_player_shots_.end(); ++itor)
    {
        (*itor)->Draw2d(g_);
    }

    for(itor = ent_enemies_.begin(); itor != ent_enemies_.end(); ++itor)
    {
        (*itor)->Draw2d(g_);
    }

    for(itor = ent_enemy_shots_.begin(); itor != ent_enemy_shots_.end(); ++itor)
    {
        (*itor)->Draw2d(g_);
    }

    for(itor = ent_explosions_.begin(); itor != ent_explosions_.end(); ++itor)
    {
        (*itor)->Draw2d(g_);
    }

    for(itor = ent_pickups_.begin(); itor != ent_pickups_.end(); ++itor)
    {
        (*itor)->Draw2d(g_);
    }

    if(ent_player_) ent_player_->Draw2d(g_);
    if(ent_statusbar_left_) ent_statusbar_left_->Draw2d(g_);
    if(ent_statusbar_right_) ent_statusbar_right_->Draw2d(g_);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::FrameEnd()
{
    Gfx::EntityList::iterator prev;
    Gfx::EntityList::iterator current = ent_enemies_.begin();
    while(current != ent_enemies_.end())
    {
        prev = current;
        ++current;

        if((*prev)->Dead())
        {
            Gfx::IEntityPtr entity = *prev;
            ent_enemies_.erase(prev);
            entity->OnFree();
            entity.reset();
        }
    }
    current = ent_player_shots_.begin();
    while(current != ent_player_shots_.end())
    {
        prev = current;
        ++current;

        if((*prev)->Dead())
        {
            Gfx::IEntityPtr entity = *prev;
            ent_player_shots_.erase(prev);
            entity->OnFree();
            entity.reset();
        }
    }
    current = ent_enemy_shots_.begin();
    while(current != ent_enemy_shots_.end())
    {
        prev = current;
        ++current;

        if((*prev)->Dead())
        {
            Gfx::IEntityPtr entity = *prev;
            ent_enemy_shots_.erase(prev);
            entity->OnFree();
            entity.reset();
        }
    }
    current = ent_explosions_.begin();
    while(current != ent_explosions_.end())
    {
        prev = current;
        ++current;

        if((*prev)->Dead())
        {
            Gfx::IEntityPtr entity = *prev;
            ent_explosions_.erase(prev);
            entity->OnFree();
            entity.reset();
        }
    }
    current = ent_pickups_.begin();
    while(current != ent_pickups_.end())
    {
        prev = current;
        ++current;

        if((*prev)->Dead())
        {
            Gfx::IEntityPtr entity = *prev;
            ent_pickups_.erase(prev);
            entity->OnFree();
            entity.reset();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Xytron::AllEnemyEntitiesDead() const
{
    return ent_enemies_.empty();
}

///////////////////////////////////////////////////////////////////////////////
unsigned int Xytron::NumEnemyEntities() const
{
    return unsigned int(ent_enemies_.size());
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::InitialiseNewGameState()
{
    current_background_ = rand()%6;
    ent_starfield_.RandomiseStarsDirection();

    current_wave_ = 0;
    PlayerEntity* player = static_cast<PlayerEntity*>(ent_player_.get());
    player->Reset();
}

///////////////////////////////////////////////////////////////////////////////
bool Xytron::IncrementWave()
{
    if(++current_background_ >= 6)
    {
        current_background_ = 0;
    }
    return (++current_wave_ >= MAX_WAVES);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::SetWave(unsigned int wave)
{
    current_wave_ = wave;
    if(current_wave_ >= MAX_WAVES)
    {
        current_wave_ = MAX_WAVES-1;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::RespawnPlayer()
{
    Player()->SpawnNewShip();
    SetPlayerPosition(Math::Vector(400.0f, 550.0f, 0.0f));
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::SetPlayerPosition(const Math::Vector& position)
{
    //POINT point;
    //point.x = int(position.x_);
    //point.y = int(position.y_);
    //ClientToScreen(window_.Handle(), &point);
    //SetCursorPos(point.x, point.y);

    ent_player_->Position(Math::Vector(position.x_, position.y_, 0.0f));
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::StartScreenFlash(const Gfx::Color& color, unsigned long fade_ms)
{
    screen_flash_color_     = color;
    screen_flash_alpha_     = 1.0f;
    alpha_fade_start_time_  = timeGetTime();
    alpha_fade_time_        = fade_ms;
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::PlacePlayerAtStartPosition()
{
    Player()->MakeTempInvulnerable();
    SetPlayerPosition(Math::Vector(400.0f, 550.0f, 0.0f));
}

///////////////////////////////////////////////////////////////////////////////
PlayerEntity* Xytron::Player() const
{
    return static_cast<PlayerEntity*>(ent_player_.get());
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::OnPlayerDeath()
{
    current_state_->OnPlayerDeath();
}





///////////////////////////////////////////////////////////////////////////////
void Xytron::ShowMainDialog()
{
    window_manager_.HideAllWindows();
    window_manager_.ShowWindowTopMost(main_dialog_);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::ShowOptionsDialog()
{
    window_manager_.ShowWindowTopMost(options_dialog_);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::ShowScoresDialog(bool new_highscore, unsigned int level, unsigned long score)
{
    highscores_dialog_->EnterNewHighscore(new_highscore, level, score);
    window_manager_.ShowWindowTopMost(highscores_dialog_);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::ShowHelpDialog()
{
    window_manager_.ShowWindowTopMost(help_dialog_);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::ShowInGameDialog()
{
    window_manager_.ShowWindowTopMost(ingame_dialog_);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::HideAllWindows()
{
    window_manager_.HideAllWindows();
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::StartNewGame()
{
    InitialiseNewGameState();
    ChangeToStatePreWave(false);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::ResumeCurrentGame()
{
    ChangeToStateGetReady(false);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::EndCurrentGame()
{
    ChangeToStatePreGame();
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::ExitApplication()
{
    PostQuitMessage(0);
}





///////////////////////////////////////////////////////////////////////////////
void Xytron::SkipToNextWave()
{
    if(IncrementWave())
    {
        ChangeToStateGameWon();
    }
    else
    {
        ChangeToStatePreWave(true);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::SkipToBoss()
{
    if(current_wave_ ==  4 || current_wave_ ==  9 || current_wave_ == 14 ||
       current_wave_ == 19 || current_wave_ == 24 || current_wave_ == 29)
    {
        ent_enemies_.clear();
        CurrentState()->SkipToBoss();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::SkipToWave(unsigned int wave)
{
    if(wave >= 0 && wave < MAX_WAVES)
    {
        current_wave_ = wave;
        ChangeToStatePreWave(true);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::ChangeToStatePreGame(bool new_highscore, unsigned int level, unsigned long score)
{
    StatePreGame* state = static_cast<StatePreGame*>(state_pre_game_.get());
    state->EnterNewHighscore(new_highscore, level, score);
    pending_state_ = state_pre_game_;
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::ChangeToStatePreWave(bool warping)
{
    pending_state_ = state_pre_wave_;
    StatePreWave* wave = static_cast<StatePreWave*>(state_pre_wave_.get());
    wave->Warping(warping);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::ChangeToStateWave(bool resumed)
{
    pending_state_ = state_wave_;
    StateWave* wave = static_cast<StateWave*>(pending_state_.get());
    wave->Resumed(resumed);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::ChangeToStatePostWave()
{
    pending_state_ = state_post_wave_;
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::ChangeToStatePaused()
{
    pending_state_ = state_paused_;
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::ChangeToStateGetReady(bool respawn_player)
{
    pending_state_ = state_get_ready_;
    StateGetReady* state = static_cast<StateGetReady*>(pending_state_.get());
    state->RespawnPlayer(respawn_player);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::ChangeToStateGameOver()
{
    pending_state_ = state_game_over_;
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::ChangeToStateGameWon()
{
    pending_state_ = state_game_won_;
}







///////////////////////////////////////////////////////////////////////////////
void Xytron::OnKeyPressed(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(!window_.IsFullscreenStyle() && mouse_captured_)
    {
        if((key == DIK_LMENU || key == DIK_RMENU) && ctrl_held)
        {
            mouse_captured_ = false;

            while(ShowCursor(TRUE) < 0) ;
            ClipCursor(NULL);

            window_.SetTitle("Xytron - [Left click in this window to capture the mouse cursor]");
        }
    }

    window_manager_.OnKeyPressed(key, shift_held, ctrl_held, alt_held, win_held);
    current_state_->OnKeyPressed(key, shift_held, ctrl_held, alt_held, win_held);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::OnKeyHeld(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    window_manager_.OnKeyHeld(key, shift_held, ctrl_held, alt_held, win_held);
    current_state_->OnKeyHeld(key, shift_held, ctrl_held, alt_held, win_held);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::OnKeyReleased(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    window_manager_.OnKeyReleased(key, shift_held, ctrl_held, alt_held, win_held);
    current_state_->OnKeyReleased(key, shift_held, ctrl_held, alt_held, win_held);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::OnMouseMoved(int x_delta, int y_delta)
{
    if(mouse_captured_)
    {
        window_manager_.OnMouseMoved(x_delta, y_delta);
        current_state_->OnMouseMoved(x_delta, y_delta);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::OnMouseButtonPressed(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(mouse_captured_)
    {
        window_manager_.OnMouseButtonPressed(button, shift_held, ctrl_held, alt_held, win_held);
        current_state_->OnMouseButtonPressed(button, shift_held, ctrl_held, alt_held, win_held);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::OnMouseButtonHeld(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(mouse_captured_)
    {
        window_manager_.OnMouseButtonHeld(button, shift_held, ctrl_held, alt_held, win_held);
        current_state_->OnMouseButtonHeld(button, shift_held, ctrl_held, alt_held, win_held);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::OnMouseButtonReleased(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(mouse_captured_)
    {
        window_manager_.OnMouseButtonReleased(button, shift_held, ctrl_held, alt_held, win_held);
        current_state_->OnMouseButtonReleased(button, shift_held, ctrl_held, alt_held, win_held);
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Xytron::OnWindowClosed(Gfx::Window* window)
{
    PostQuitMessage(0);
    return true;    // Let DefWindowProc() destroy the window
}

///////////////////////////////////////////////////////////////////////////////
bool Xytron::OnWindowEraseBackground(Gfx::Window* window, HDC dc)
{
    Draw();
    return false;   // Don't let DefWindowProc() erase the background
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::OnWindowOtherMessage(UINT msg, WPARAM wparam, LPARAM lparam)
{
    Aud::Music* track;
    switch(msg)
    {
    case WM_MUSIC_TRACK_0: track = &mus_tracks_[0]; break;
    case WM_MUSIC_TRACK_1: track = &mus_tracks_[1]; break;
    case WM_MUSIC_TRACK_2: track = &mus_tracks_[2]; break;
    case WM_MUSIC_TRACK_3: track = &mus_tracks_[3]; break;
    default:
        return;
        break;
    }

    if(track->OnGraphEvent())
    {
        PlayNextMusicTrack();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::OnWindowMouseLPressed(Gfx::Window* window)
{
    if(!window_.IsFullscreenStyle() && !mouse_captured_)
    {
        mouse_captured_ = true;

        while(ShowCursor(FALSE) >= 0) ;

        RECT rect;
        GetClientRect(window_.Handle(), &rect);

        ClientToScreen(window_.Handle(), (POINT*)&rect.left);
        ClientToScreen(window_.Handle(), (POINT*)&rect.right);
        ClipCursor(&rect);

        window_.SetTitle("Xytron - [Press Ctrl+Alt to release the mouse cursor]");
    }
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::OnWindowActivated(Gfx::Window* window)
{
    if(mouse_captured_)
    {
        while(ShowCursor(FALSE) >= 0) ;

        RECT rect;
        GetClientRect(window_.Handle(), &rect);

        ClientToScreen(window_.Handle(), (POINT*)&rect.left);
        ClientToScreen(window_.Handle(), (POINT*)&rect.right);
        ClipCursor(&rect);
    }

    inp_.Activate();

    if(MusicEnabled())
    {
        mus_tracks_[current_track_].Resume();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::OnWindowDeactived(Gfx::Window* window)
{
    while(ShowCursor(TRUE) < 0) ;
    ClipCursor(NULL);

    inp_.Deactivate();

    if(MusicEnabled())
    {
        mus_tracks_[current_track_].Pause();
    }
}







///////////////////////////////////////////////////////////////////////////////
void Xytron::LoadResources(LoadingScreen& loading_screen)
{
    for(int i = 0; i < 6; i++)
    {
        std::ostringstream oss;
        oss << "Images/Space" << i << ".png";
        img_backgrounds_[i] = resources_.LoadImage(oss.str(), 0, 0, 600, 600);
        img_backgrounds_[i]->DrawCentered(false);
    }

    loading_screen.Update(10);

    resources_.LoadImageAnim("Images/EnemyShot.tga", 0, 0, 16, 16, 16);
    resources_.LoadImageAnim("Images/MissileShotUp.tga", 0, 0, 16, 24, 8);
    resources_.LoadImageAnim("Images/MissileShotDown.tga", 0, 0, 16, 24, 8);
    resources_.LoadImageAnim("Images/1000Points.tga", 0, 0, 64, 24, 16);
    resources_.LoadImage("Images/LaserShotSmall.tga", 0, 0, 15, 40);
    resources_.LoadImage("Images/LaserShotLarge.tga", 0, 0, 50, 50);
    resources_.LoadImage("Images/Smoke0.tga", 0, 0, 128, 128);
    resources_.LoadImage("Images/Smoke1.tga", 0, 0, 128, 128);
    resources_.LoadImage("Images/Smoke2.tga", 0, 0, 128, 128);
    resources_.LoadImage("Images/Smoke3.tga", 0, 0, 128, 128);
    resources_.LoadImage("Images/RailShot0.tga", 0, 0, 20, 125);
    resources_.LoadImage("Images/RailShot1.tga", 0, 0, 20, 125);
    resources_.LoadImage("Images/RailShot2.tga", 0, 0, 20, 125);
    resources_.LoadImage("Images/RailShot3.tga", 0, 0, 25, 125);
    resources_.LoadImage("Images/PlasmaShot0.tga", 0, 0, 25, 25);
    resources_.LoadImage("Images/PlasmaShot1.tga", 0, 0, 35, 35);
    resources_.LoadImage("Images/PlasmaShot2.tga", 0, 0, 30, 30);
    resources_.LoadImage("Images/PlasmaShot3.tga", 0, 0, 40, 40);
    resources_.LoadImage("Images/MuzzleFlash0.tga", 0, 0, 40, 40);
    resources_.LoadImage("Images/MuzzleFlash1.tga", 0, 0, 40, 40);
    resources_.LoadImage("Images/MuzzleFlash2.tga", 0, 0, 40, 40);
    resources_.LoadImage("Images/ExplosionMedium0.tga", 0, 0, 128, 128);
    resources_.LoadImage("Images/ExplosionMedium1.tga", 0, 0, 128, 128);
    resources_.LoadImage("Images/ExplosionMedium2.tga", 0, 0, 128, 128);
    resources_.LoadImage("Images/ExplosionMedium3.tga", 0, 0, 128, 128);
    resources_.LoadImage("Images/ExplosionSmall0.tga", 0, 0, 32, 32);
    resources_.LoadImage("Images/ExplosionSmall1.tga", 0, 0, 32, 32);
    resources_.LoadImage("Images/ExplosionSmall2.tga", 0, 0, 32, 32);
    resources_.LoadImage("Images/ExplosionSmall3.tga", 0, 0, 32, 32);
    resources_.LoadImageAnim("Images/PlayerShip.tga", 0, 0, 64, 64, 17);
    resources_.LoadImageAnim("Images/PlayerShip_P0.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/PlayerShip_P1.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/PlayerShip_P2.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/PlayerShip_P3.tga", 0, 0, 32, 32, 16);
    resources_.LoadImage("Images/SparkSmall.tga", 0, 0, 16, 16);
    resources_.LoadImage("Images/SparkMedium.tga", 0, 0, 24, 24);
    resources_.LoadImage("Images/SparkLarge.tga", 0, 0, 32, 32);
    resources_.LoadImageAnim("Images/StockDebris_P0.tga", 0, 0, 24, 24, 16);
    resources_.LoadImageAnim("Images/StockDebris_P1.tga", 0, 0, 24, 24, 16);
    resources_.LoadImageAnim("Images/StockDebris_P2.tga", 0, 0, 24, 24, 16);
    resources_.LoadImageAnim("Images/StockDebris_P3.tga", 0, 0, 24, 24, 16);
    resources_.LoadImage("Images/WakeRed.tga", 0, 0, 128, 128);
    resources_.LoadImage("Images/WakeGreen.tga", 0, 0, 128, 128);
    resources_.LoadImage("Images/WakeBlue.tga", 0, 0, 128, 128);
    resources_.LoadImage("Images/WakeEmp.tga", 0, 0, 256, 256);
    loading_screen.Update(20);
    resources_.LoadImageAnim("Images/PickupGunCannon.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/PickupGunLaser.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/PickupGunMissiles.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/PickupGunPlasma.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/PickupGunRail.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/PickupBombNuke.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/PickupBombScatter.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/PickupBombFunky.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/PickupBombEmp.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/PickupBonus100points.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/PickupBonus1000points.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/PickupBonusFullhitpoints.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/PickupBonusExtraship.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/NukeShot.tga", 0, 0, 40, 40, 16);
    resources_.LoadImage("Images/NukeWake.tga", 0, 0, 512, 512);
    resources_.LoadImage("Images/NukeExplosion.tga", 0, 0, 512, 512);
    resources_.LoadImage("Images/Fireball0.tga", 0, 0, 64, 64);
    resources_.LoadImage("Images/Fireball1.tga", 0, 0, 64, 64);
    resources_.LoadImage("Images/Fireball2.tga", 0, 0, 64, 64);
    resources_.LoadImage("Images/Fireball3.tga", 0, 0, 64, 64);
    resources_.LoadImage("Images/Fireball4.tga", 0, 0, 64, 64);
    resources_.LoadImage("Images/Fireball5.tga", 0, 0, 64, 64);
    resources_.LoadImageAnim("Images/ScatterShot.tga", 0, 0, 40, 40, 16);
    resources_.LoadImage("Images/StatusBarLeft.tga", 0, 0, 120, 600);
    resources_.LoadImage("Images/StatusBarRight.tga", 0, 0, 120, 600);
    resources_.LoadImageAnim("Images/AnimIconCannonGun.png", 0, 0, 64, 64, 32);
    resources_.LoadImageAnim("Images/AnimIconLaserGun.png", 0, 0, 64, 64, 32);
    resources_.LoadImageAnim("Images/AnimIconRailGun.png", 0, 0, 64, 64, 32);
    resources_.LoadImageAnim("Images/AnimIconPlasmaGun.png", 0, 0, 64, 64, 32);
    resources_.LoadImageAnim("Images/AnimIconMissilesGun.png", 0, 0, 64, 64, 32);
    resources_.LoadImageAnim("Images/AnimIconFunkyBomb.png", 0, 0, 64, 64, 32);
    resources_.LoadImageAnim("Images/AnimIconNukeBomb.png", 0, 0, 64, 64, 32);
    resources_.LoadImageAnim("Images/AnimIconScatterBomb.png", 0, 0, 64, 64, 32);
    resources_.LoadImageAnim("Images/AnimIconEmpBomb.png", 0, 0, 64, 64, 32);
    resources_.LoadImageAnim("Images/StaticBombIcons.png", 0, 0, 32, 32, 4);
    resources_.LoadImage("Images/GlassHighlight.tga", 0, 0, 128, 128);
    resources_.LoadImage("Images/SmallGlassHighlight.tga", 0, 0, 52, 52);
    resources_.LoadImage("Images/FunkyBombOverlay.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/ScatterBombOverlay.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/NukeBombOverlay.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/EmpBombOverlay.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/CannonGunOverlay0.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/CannonGunOverlay1.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/CannonGunOverlay2.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/CannonGunOverlay3.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/LaserGunOverlay0.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/LaserGunOverlay1.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/LaserGunOverlay2.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/LaserGunOverlay3.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/MissilesGunOverlay0.tga", 0, 0, 100, 100);
    loading_screen.Update(30);
    resources_.LoadImage("Images/MissilesGunOverlay1.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/MissilesGunOverlay2.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/MissilesGunOverlay3.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/RailGunOverlay0.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/RailGunOverlay1.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/RailGunOverlay2.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/RailGunOverlay3.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/PlasmaGunOverlay0.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/PlasmaGunOverlay1.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/PlasmaGunOverlay2.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/PlasmaGunOverlay3.tga", 0, 0, 100, 100);
    resources_.LoadImage("Images/PlayerShipIcon.png", 0, 0, 32, 32);
    resources_.LoadImage("Images/RoundedRectHighlight.tga", 0, 0, 86, 40);
    resources_.LoadImage("Images/HitpointsHighlight.tga", 0, 0, 105, 105);
    resources_.LoadImage("Images/Hitpoints.tga", 0, 0, 95, 105);
    resources_.LoadImageAnim("Images/EnemyType0.tga", 0, 0, 64, 64, 36);
    resources_.LoadImageAnim("Images/EnemyType1.tga", 0, 0, 64, 64, 36);
    resources_.LoadImageAnim("Images/ThrusterFlame.tga", 0, 0, 64, 64, 26);
    resources_.LoadImageAnim("Images/EnemyType2_0.tga", 0, 0, 64, 64, 36);
    resources_.LoadImageAnim("Images/EnemyType2_1.tga", 0, 0, 64, 64, 36);
    resources_.LoadImageAnim("Images/EnemyType2_2.tga", 0, 0, 64, 64, 36);
    resources_.LoadImageAnim("Images/EnemyType2_3.tga", 0, 0, 64, 64, 36);
    resources_.LoadImageAnim("Images/EnemyType2_4.tga", 0, 0, 64, 64, 36);
    resources_.LoadImageAnim("Images/EnemyType2_5.tga", 0, 0, 64, 64, 36);
    resources_.LoadImageAnim("Images/EnemyType2_6.tga", 0, 0, 64, 64, 36);
    resources_.LoadImageAnim("Images/EnemyType2_7.tga", 0, 0, 64, 64, 36);
    resources_.LoadImageAnim("Images/EnemyDebris0.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/EnemyDebris1.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/EnemyDebris2.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/EnemyDebris3.tga", 0, 0, 32, 32, 16);
    resources_.LoadImageAnim("Images/EnemyType3_0.tga", 0, 0, 48, 80, 16);
    resources_.LoadImageAnim("Images/EnemyType3_1.tga", 0, 0, 48, 80, 16);
    resources_.LoadImageAnim("Images/EnemyType3_2.tga", 0, 0, 48, 80, 16);
    resources_.LoadImageAnim("Images/EnemyType3_3.tga", 0, 0, 48, 80, 16);
    resources_.LoadImageAnim("Images/EnemyType3_4.tga", 0, 0, 48, 80, 16);
    resources_.LoadImageAnim("Images/EnemyType3_5.tga", 0, 0, 48, 80, 16);
    resources_.LoadImageAnim("Images/EnemyType3_6.tga", 0, 0, 48, 80, 16);
    resources_.LoadImageAnim("Images/EnemyType3_7.tga", 0, 0, 48, 80, 16);
    resources_.LoadImageAnim("Images/EnemyType3_8.tga", 0, 0, 48, 80, 16);
    resources_.LoadImageAnim("Images/EnemyType4.tga", 0, 0, 64, 64, 36);
    resources_.LoadImageAnim("Images/EnemyType5.tga", 0, 0, 48, 64, 16);
    resources_.LoadImage("Images/MiniBoss0.tga", 0, 0, 70, 80);
    resources_.LoadImageAnim("Images/MiniBoss0_TopGunLeft.tga", 0, 0, 20, 18, 8);
    resources_.LoadImageAnim("Images/MiniBoss0_TopGunMiddle.tga", 0, 0, 15, 18, 8);
    resources_.LoadImageAnim("Images/MiniBoss0_TopGunRight.tga", 0, 0, 20, 18, 8);
    resources_.LoadImageAnim("Images/MiniBoss0_BotGunLeft.tga", 0, 0, 12, 16, 8);
    loading_screen.Update(40);
    resources_.LoadImageAnim("Images/MiniBoss0_BotGunMiddle.tga", 0, 0, 10, 16, 8);
    resources_.LoadImageAnim("Images/MiniBoss0_BotGunRight.tga", 0, 0, 10, 16, 8);
    resources_.LoadImage("Images/Boss0.tga", 0, 0, 140, 90);
    unsigned int i;
    for(i = 0; i < 36; i++)
    {
        std::ostringstream oss;
        oss << "Images/Boss0_TopGun" << i << ".tga";
        resources_.LoadImageAnim(oss.str(), 0, 0, 64, 64, 16);
    }
    loading_screen.Update(50);
    resources_.LoadImageAnim("Images/MiniBoss1.tga", 0, 0, 128, 128, 36);
    resources_.LoadImageAnim("Images/Boss1_EndGunLeft.tga", 0, 0, 36, 100, 16);
    resources_.LoadImageAnim("Images/Boss1_EndGunRight.tga", 0, 0, 36, 100, 16);
    resources_.LoadImageAnim("Images/Boss1_FlipGun.tga", 0, 0, 200, 100, 16);
    resources_.LoadImageAnim("Images/MiniBoss2.tga", 0, 0, 64, 128, 16);
    resources_.LoadImageAnim("Images/Boss2.tga", 0, 0, 160, 160, 20);
    resources_.LoadImageAnim("Images/CancelButton.tga", 0, 0, 64, 64, 4);
    resources_.LoadImageAnim("Images/ApplyButton.tga", 0, 0, 64, 64, 4);
    resources_.LoadImageAnim("Images/CloseButton.tga", 0, 0, 64, 64, 4);
    resources_.LoadImageAnim("Images/ExitButton.tga", 0, 0, 64, 64, 4);
    resources_.LoadImageAnim("Images/GameButton.tga", 0, 0, 64, 64, 4);
    resources_.LoadImageAnim("Images/HelpButton.tga", 0, 0, 64, 64, 4);
    resources_.LoadImageAnim("Images/HighscoresButton.tga", 0, 0, 64, 64, 4);
    resources_.LoadImageAnim("Images/NextButton.tga", 0, 0, 64, 64, 4);
    resources_.LoadImageAnim("Images/OkButton.tga", 0, 0, 64, 64, 4);
    resources_.LoadImageAnim("Images/OptionsButton.tga", 0, 0, 64, 64, 4);
    resources_.LoadImageAnim("Images/PrevButton.tga", 0, 0, 64, 64, 4);
    resources_.LoadImageAnim("Images/ResetButton.tga", 0, 0, 64, 64, 4);
    resources_.LoadImageAnim("Images/ResumeButton.tga", 0, 0, 64, 64, 4);
    resources_.LoadImageAnim("Images/StopButton.tga", 0, 0, 64, 64, 4);
    resources_.LoadImageAnim("Images/TestButton.tga", 0, 0, 64, 64, 4);
    resources_.LoadImageAnim("Images/MouseCursor.tga", 0, 0, 32, 32, 20);
    resources_.LoadImageAnim("Images/MainDialog.tga", 0, 0, 400, 400, 1);
    resources_.LoadImageAnim("Images/OptionsDialog.tga", 0, 0, 400, 400, 1);
    resources_.LoadImageAnim("Images/HighscoresDialog.tga", 0, 0, 500, 500, 1);
    resources_.LoadImageAnim("Images/HelpDialog.tga", 0, 0, 500, 400, 1);
    resources_.LoadImageAnim("Images/InGameDialog.tga", 0, 0, 300, 400, 1);
    loading_screen.Update(60);
    resources_.LoadImageAnim("Images/OverviewText.tga", 0, 0, 400, 200, 1);
    resources_.LoadImageAnim("Images/PlayerShipText.tga", 0, 0, 400, 200, 1);
    resources_.LoadImageAnim("Images/WeaponSystemText.tga", 0, 0, 400, 200, 1);
    resources_.LoadImageAnim("Images/CannonGunText.tga", 0, 0, 400, 200, 1);
    resources_.LoadImageAnim("Images/LaserGunText.tga", 0, 0, 400, 200, 1);
    resources_.LoadImageAnim("Images/MissilesText.tga", 0, 0, 400, 200, 1);
    resources_.LoadImageAnim("Images/PlasmaGunText.tga", 0, 0, 400, 200, 1);
    resources_.LoadImageAnim("Images/RailGunText.tga", 0, 0, 400, 200, 1);
    resources_.LoadImageAnim("Images/BombSystemText.tga", 0, 0, 400, 200, 1);
    resources_.LoadImageAnim("Images/FunkyBombText.tga", 0, 0, 400, 200, 1);
    resources_.LoadImageAnim("Images/EmpBombText.tga", 0, 0, 400, 200, 1);
    resources_.LoadImageAnim("Images/NukeBombText.tga", 0, 0, 400, 200, 1);
    resources_.LoadImageAnim("Images/ScatterBombText.tga", 0, 0, 400, 200, 1);
    resources_.LoadImageAnim("Images/ItemSystemText.tga", 0, 0, 400, 200, 1);
    resources_.LoadImageAnim("Images/HighscoresListView.tga", 0, 0, 400, 300, 1);
    resources_.LoadImageAnim("Images/NameEditBox.tga", 0, 0, 256, 30, 1);
    resources_.LoadImageAnim("Images/EnterNameInstructions.tga", 0, 0, 256, 30, 1);
    resources_.LoadImageAnim("Images/CheckboxFullscreenGraphics.tga", 0, 0, 240, 30, 6);
    resources_.LoadImageAnim("Images/CheckboxEnableMusic.tga", 0, 0, 240, 30, 6);
    resources_.LoadImageAnim("Images/CheckboxEnableSound.tga", 0, 0, 240, 30, 6);
    resources_.LoadImageAnim("Images/SliderBack.tga", 0, 0, 240, 30, 2);
    resources_.LoadImageAnim("Images/SliderBox.tga", 0, 0, 24, 24, 2);
    resources_.LoadImageAnim("Images/MainDialogShadow.tga", 0, 0, 400, 400, 1);
    resources_.LoadImageAnim("Images/OptionsDialogShadow.tga", 0, 0, 400, 400, 1);
    resources_.LoadImageAnim("Images/HighscoresDialogShadow.tga", 0, 0, 500, 500, 1);
    resources_.LoadImageAnim("Images/HelpDialogShadow.tga", 0, 0, 500, 400, 1);
    resources_.LoadImageAnim("Images/InGameDialogShadow.tga", 0, 0, 300, 400, 1);
    loading_screen.Update(70);

    resources_.LoadSound("Sounds/Alert.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/CannonHit0.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/CannonHit1.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/CannonShot.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/CollectWeapon0.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/CollectWeapon1.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/CollectWeapon2.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/EmpShoot.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/FunkyShoot.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/GameOver.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/GameWon.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/LaserHit.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/LaserShot.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/MissilesHit.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/MissilesShot0.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/MissilesShot1.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/NukeExplosion.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/NukeShoot.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/Pickup100Points.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/Pickup1000Points.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/PickupExtraShip.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/PickupFullHitPoints.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/PlasmaHit.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/PlasmaShot.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/RailShot.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/ShipExplosion0.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/ShipExplosion1.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/ShipExplosion2.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/ShipExplosion3.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/ShipExplosion4.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/Teleport.wav", 1.0f, 10000.0f);
    resources_.LoadSound("Sounds/Test.wav", 1.0f, 10000.0f);
    loading_screen.Update(80);

    mus_tracks_[0].Load("Music/Track0.mp3", window_.Handle(), WM_MUSIC_TRACK_0);
    mus_tracks_[1].Load("Music/Track1.mp3", window_.Handle(), WM_MUSIC_TRACK_1);
    mus_tracks_[2].Load("Music/Track2.mp3", window_.Handle(), WM_MUSIC_TRACK_2);
    mus_tracks_[3].Load("Music/Track3.mp3", window_.Handle(), WM_MUSIC_TRACK_3);

    mus_tracks_[0].Volume(MusicVolume());
    mus_tracks_[1].Volume(MusicVolume());
    mus_tracks_[2].Volume(MusicVolume());
    mus_tracks_[3].Volume(MusicVolume());

    ent_player_.reset(new PlayerEntity(this, this));
    PlayerEntity* player = static_cast<PlayerEntity*>(ent_player_.get());
    player->BindResources(&resources_);
    player->Position(Math::Vector(-100.0f, -100.0f, 0.0f));

    ent_statusbar_left_.reset(new StatusBarLeft(this));
    StatusBarLeft* sb_left = static_cast<StatusBarLeft*>(ent_statusbar_left_.get());
    sb_left->BindResources(&resources_);

    ent_statusbar_right_.reset(new StatusBarRight(this));
    StatusBarRight* sb_right = static_cast<StatusBarRight*>(ent_statusbar_right_.get());
    sb_right->BindResources(&resources_);

    window_manager_.GetMouseCursor().BindResources(&resources_);

    main_dialog_.reset(new MainDialog(&window_manager_, this, resources_));
    options_dialog_.reset(new OptionsDialog(&window_manager_, this, resources_, *this));
    highscores_dialog_.reset(new HighscoresDialog(&window_manager_, this, resources_, highscores_));
    help_dialog_.reset(new HelpDialog(&window_manager_, this, resources_));
    ingame_dialog_.reset(new InGameDialog(&window_manager_, this, resources_));

    resources_.ApplySoundVolume(SoundVolume());
    loading_screen.Update(90);
}

///////////////////////////////////////////////////////////////////////////////
void Xytron::PlayNextMusicTrack()
{
    try
    {
        if(MusicEnabled())
        {
            if(current_track_ == -1)
            {
                current_track_ = rand()%4;
            }
            else
            {
                mus_tracks_[current_track_].Stop();
                if(++current_track_ >= 4)
                {
                    current_track_ = 0;
                }
            }
            mus_tracks_[current_track_].Play();
        }
    }
    catch(std::exception& e)
    {
        MusicEnabled(false);
        LOG("Caught the exception [" << e.what() << "], continuing without music.");
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Xytron::SetFullscreen()
{
    int frequencies[] = { 75, 72, 70, 80, 85, 65, 60, 0 };
    for(int i = 0; frequencies[i]; i++)
    {
        Gfx::DisplayMode mode(800, 600, 32, frequencies[i]);
        if(mode.Set())
        {
            return true;
        }
    }

    LOG("Fullscreen graphics mode not supported by this graphics adapter.  Resorting to windowed mode.");
    return false;
}





///////////////////////////////////////////////////////////////////////////////
void Xytron::FreeMemory()
{
    main_dialog_.reset();
    options_dialog_.reset();
    highscores_dialog_.reset();
    help_dialog_.reset();
    ingame_dialog_.reset();

    mus_tracks_[0].Stop();
    mus_tracks_[1].Stop();
    mus_tracks_[2].Stop();
    mus_tracks_[3].Stop();
    mus_tracks_[0].Unload();
    mus_tracks_[1].Unload();
    mus_tracks_[2].Unload();
    mus_tracks_[3].Unload();

    img_backgrounds_[0].reset();
    img_backgrounds_[1].reset();
    img_backgrounds_[2].reset();
    img_backgrounds_[3].reset();

    current_state_.reset();
    pending_state_.reset();
    state_pre_game_.reset();
    state_pre_wave_.reset();
    state_wave_.reset();
    state_post_wave_.reset();
    state_paused_.reset();
    state_get_ready_.reset();
    state_game_over_.reset();
    state_game_won_.reset();
    fnt_main_.reset();
    ent_player_.reset();
    ent_statusbar_left_.reset();
    ent_statusbar_right_.reset();

    ent_enemies_.clear();
    ent_player_shots_.clear();
    ent_enemy_shots_.clear();
    ent_explosions_.clear();
    ent_pickups_.clear();
    wep_thinkable_.clear();

    resources_.UnloadAll();
}


void Xytron::OnFullscreenGraphicsSettingChanged()
{
    if(FullscreenGraphics())
    {
        if(!window_.IsFullscreenStyle())
        {
            if(SetFullscreen())
            {
                window_.ToggleBetweenStyles();
                window_.SetTitle("Xytron");
            }
            else
            {
                FullscreenGraphics(false);
            }
        }
    }
    else
    {
        if(window_.IsFullscreenStyle())
        {
            Gfx::DisplayModes::SetModeFromRegistry();
            window_.ToggleBetweenStyles();
            window_.SetTitle("Xytron - [Press Ctrl+Alt to release the mouse cursor]");
        }
    }
}

void Xytron::OnSoundEnabledSettingChanged()
{
    SoundEnabled() ? aud_.Enable() : aud_.Disable();
}

void Xytron::OnMusicEnabledSettingChanged()
{
    MusicEnabled() ? mus_tracks_[current_track_].Play() : mus_tracks_[current_track_].Stop();
}

void Xytron::OnSoundVolumeSettingChanged()
{
    resources_.ApplySoundVolume(SoundVolume());
}

void Xytron::OnMusicVolumeSettingChanged()
{
    mus_tracks_[0].Volume(MusicVolume());
    mus_tracks_[1].Volume(MusicVolume());
    mus_tracks_[2].Volume(MusicVolume());
    mus_tracks_[3].Volume(MusicVolume());
}
