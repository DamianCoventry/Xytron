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
#ifndef INCLUDED_XYTRON
#define INCLUDED_XYTRON

#include "../gfx/gfx.h"
#include "../inp/inp.h"
#include "../aud/aud.h"
#include "../timer/manager.h"
#include "../util/resourcecontext.h"
#include "../win/windowmanager.h"

#include "istatecontroller.h"
#include "iworld.h"
#include "playerentity.h"
#include "starfieldentity.h"
#include "loadingscreen.h"

#include "idialogcontrol.h"
#include "maindialog.h"
#include "optionsdialog.h"
#include "highscoresdialog.h"
#include "helpdialog.h"
#include "ingamedialog.h"
#include "highscores.h"
#include "settings.h"

class State;
typedef boost::shared_ptr<State> StatePtr;

///////////////////////////////////////////////////////////////////////////////
class Xytron
    : public Gfx::IWindowEvents
    , public Inp::InputEvents
    , public IStateController
    , public IWorld
    , public PlayerEntityEvents
    , public IDialogControl
    , public Settings
{
public:
    static const unsigned int MAX_WAVES;

public:
    Xytron(HINSTANCE instance);
    ~Xytron();

    void Run();

    // Implementation of IWorld
    void Clear();
    void FrameBegin();
    void Think(float time_delta);
    void Move(float time_delta);
    void Collide();
    void Draw();
    void FrameEnd();
    void ToggleInvulnerability() { invulnerable_ = !invulnerable_; }
    void SkipToNextWave();
    void SkipToBoss();
    void SkipToWave(unsigned int wave);
    bool AllEnemyEntitiesDead() const;
    unsigned int NumEnemyEntities() const;
    void InitialiseNewGameState();
    bool IncrementWave();
    unsigned int GetWave() const { return current_wave_; }
    void SetWave(unsigned int wave);
    void RespawnPlayer();
    void SetPlayerPosition(const Math::Vector& position);
    void PlacePlayerAtStartPosition();
    void StartScreenFlash(const Gfx::Color& color, unsigned long fade_ms);
    State* CurrentState() const                     { return current_state_.get(); }
    Win::WindowManager* WindowManager() const       { return (Win::WindowManager*)&window_manager_; };
    Highscores& GetHighscores() const               { return (Highscores&)highscores_; }
    Settings& GetSettings() const                   { return (Settings&)*this; }
    PlayerEntity* Player() const;
    Util::ResourceContext* ResourceContext() const  { return (Util::ResourceContext*)&resources_; }
    Gfx::EntityList& PickupEntityList() const       { return (Gfx::EntityList&)ent_pickups_; }
    Gfx::EntityList& ExplosionsEntityList() const   { return (Gfx::EntityList&)ent_explosions_; }
    Gfx::EntityList& EnemyEntityList() const        { return (Gfx::EntityList&)ent_enemies_; }
    Gfx::EntityList& EnemyShotsEntityList() const   { return (Gfx::EntityList&)ent_enemy_shots_; }
    Gfx::EntityList& EnemyShipsEntityList() const   { return (Gfx::EntityList&)ent_enemies_; }
    Gfx::EntityList& PlayerShotsEntityList() const  { return (Gfx::EntityList&)ent_player_shots_; }
    Gfx::FontPtr GetFont()                          { return fnt_main_; }
    WeaponList& GetThinkableWeaponList() const      { return (WeaponList&)wep_thinkable_; }

    // Implementation of PlayerEntityEvents
    void OnPlayerDeath();

    // Implementation of IDialogControl
    void ShowMainDialog();
    void ShowOptionsDialog();
    void ShowScoresDialog(bool new_highscore = false, unsigned int level = 0, unsigned long score = 0);
    void ShowHelpDialog();
    void ShowInGameDialog();
    void HideAllWindows();
    void StartNewGame();
    void ResumeCurrentGame();
    void EndCurrentGame();
    void ExitApplication();

private:
    // Implementation of Gfx::IWindowEvents
    bool OnWindowClosed(Gfx::Window* window);
    bool OnWindowEraseBackground(Gfx::Window* window, HDC dc);
    void OnWindowActivated(Gfx::Window* window);
    void OnWindowDeactived(Gfx::Window* window);
    void OnWindowOtherMessage(UINT msg, WPARAM wparam, LPARAM lparam);
    void OnWindowMouseLPressed(Gfx::Window* window);

    // Inp::InputEvents
    void OnKeyPressed(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnKeyHeld(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnKeyReleased(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnMouseButtonPressed(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnMouseButtonHeld(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnMouseButtonReleased(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnMouseMoved(int x_delta, int y_delta);

    // Implementation of IStateController
    void ChangeToStatePreGame(bool new_highscore = false, unsigned int level = 0, unsigned long score = 0);
    void ChangeToStatePreWave(bool warping);
    void ChangeToStateWave(bool resumed);
    void ChangeToStatePostWave();
    void ChangeToStatePaused();
    void ChangeToStateGetReady(bool respawn_player);
    void ChangeToStateGameOver();
    void ChangeToStateGameWon();

    // Implementation of Settings
    void OnFullscreenGraphicsSettingChanged();
    void OnSoundEnabledSettingChanged();
    void OnMusicEnabledSettingChanged();
    void OnSoundVolumeSettingChanged();
    void OnMusicVolumeSettingChanged();

private:
    void LoadResources(LoadingScreen& loading_screen);
    void PlayNextMusicTrack();
    bool SetFullscreen();
    void FreeMemory();

private:
    HINSTANCE instance_;

    StatePtr current_state_;
    StatePtr pending_state_;

    StatePtr state_pre_game_;
    StatePtr state_pre_wave_;
    StatePtr state_wave_;
    StatePtr state_post_wave_;
    StatePtr state_paused_;
    StatePtr state_get_ready_;
    StatePtr state_game_over_;
    StatePtr state_game_won_;

    Gfx::Graphics g_;
    Gfx::Window window_;
    Util::ResourceContext resources_;
    Timer::Manager timer_manager_;

    Inp::Input inp_;
    Aud::Device aud_;
    Aud::Music mus_tracks_[4];
    int current_track_;

    // World resources
    Gfx::ImagePtr       img_backgrounds_[6];
    Gfx::FontPtr        fnt_main_;
    int current_background_;

    Gfx::IEntityPtr ent_player_;
    Gfx::IEntityPtr ent_statusbar_left_;
    Gfx::IEntityPtr ent_statusbar_right_;
    bool invulnerable_;
    unsigned int current_wave_;

    std::string fps_string_;
    Gfx::EntityList ent_enemies_;
    Gfx::EntityList ent_player_shots_;
    Gfx::EntityList ent_enemy_shots_;
    Gfx::EntityList ent_explosions_;
    Gfx::EntityList ent_pickups_;
    WeaponList wep_thinkable_;
    StarfieldEntity ent_starfield_;

    Gfx::Color screen_flash_color_;
    float screen_flash_alpha_;
    unsigned long alpha_fade_start_time_;
    unsigned long alpha_fade_time_;

    Win::WindowManager window_manager_;
    MainDialogPtr main_dialog_;
    OptionsDialogPtr options_dialog_;
    HighscoresDialogPtr highscores_dialog_;
    HelpDialogPtr help_dialog_;
    InGameDialogPtr ingame_dialog_;
    Highscores highscores_;

    bool mouse_captured_;
};

#endif  // INCLUDED_XYTRON
