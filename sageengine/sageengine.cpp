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
#include "sageengine.h"
#include "resource.h"
#include "../util/log.h"
#include "../util/string.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <time.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "loadcellmapcommand.h"
#include "loadentitycommand.h"
#include "loadimageanimationcommand.h"
#include "loadimagecommand.h"
#include "loadmusiccommand.h"
#include "loadresourcecontextcommand.h"
#include "loadsoundcommand.h"
#include "unloadcellmapcommand.h"
#include "unloadentitycommand.h"
#include "unloadimageanimationcommand.h"
#include "unloadimagecommand.h"
#include "unloadmusiccommand.h"
#include "unloadresourcecontextcommand.h"
#include "unloadsoundcommand.h"
#include "drawcellmapcommand.h"
#include "thinkcellmapcommand.h"
#include "activatebindingscommand.h"
#include "deactivatebindingscommand.h"
#include "cameralookcommand.h"
#include "currentcamerafirstcommand.h"
#include "currentcameralastcommand.h"
#include "currentcameranextcommand.h"
#include "currentcameraprevcommand.h"
#include "currentcamerasetcommand.h"
#include "movecamerabackwardcommand.h"
#include "movecameradowncommand.h"
#include "movecameraforwardcommand.h"
#include "movecameraleftcommand.h"
#include "movecamerarightcommand.h"
#include "movecameraupcommand.h"
#include "cameratogglecollisioncommand.h"


///////////////////////////////////////////////////////////////////////////////
namespace
{
    const std::string SETTINGS_FILENAME = "SageEngine.settings";
}

///////////////////////////////////////////////////////////////////////////////
SageEngine::SageEngine(HINSTANCE instance)
: instance_(instance)
, window_(instance)
, resources_(&aud_)
, mouse_captured_(true)
, world_(&doc_)
{
    Log::Open("SageEngine.log");
    CoInitialize(NULL);

    srand(unsigned int(time(NULL)));
    Gfx::Window::Register(instance, IDI_SAGEENGINE);
}

///////////////////////////////////////////////////////////////////////////////
SageEngine::~SageEngine()
{
    aud_.Shutdown();
    inp_.Shutdown();
    g_.Shutdown();
    window_.Delete();

    Gfx::Window::Unregister(instance_);
    CoUninitialize();
    Log::Close();
}

///////////////////////////////////////////////////////////////////////////////
void SageEngine::Run(const std::string& cmd_line)
{
    char path[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, path);
    app_dir_ = path;

    world_.SetContentDir(app_dir_);

    cmd_line_ = cmd_line;
    Load(SETTINGS_FILENAME);

    if(FullscreenGraphics())
    {
        if(SetFullscreen())
        {
            window_.CreateFullscreen(800, 600, "SageEngine", this);
        }
        else
        {
            FullscreenGraphics(false);
            window_.CreateWindowed(800, 600, "SageEngine", this);
        }
    }
    else
    {
        window_.CreateWindowed(800, 600, "SageEngine", this);
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

    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_TEXTURE_2D);
    //glEnable(GL_TEXTURE_2D);
    //glEnable(GL_CULL_FACE);
    //glPolygonMode(GL_FRONT, GL_FILL);
    //glPolygonMode(GL_BACK, GL_FILL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

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

    if(!FullscreenGraphics())
    {
        window_.SetTitle("SageEngine - [Press Ctrl+Alt to release the mouse cursor]");
    }

    Inp::Bindings::BuildLookups();

    PreLoop();

    unsigned int fps = 0;
    unsigned long now, prev_time, time_stamp;

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
        world_.TimeDelta(float(now - prev_time) / 1000.0f);

        inp_.FrameBegin();

        if(mouse_captured_ && GetForegroundWindow() == window_.Handle() && !IsIconic(window_.Handle()))
        {
            SetCursorPos(window_center.left, window_center.top);
        }

        timer_manager_.ProcessExpiredTimeouts();
        inp_.Poll();
        aud_.Update3dSounds();

        state_machine_.CallOnThinkCommandLineSet(world_.TimeDelta());
        state_machine_.CallOnMoveCommandLineSet(world_.TimeDelta());

        g_.ClearBackBuffer();
        g_.Set3d();

        SetModelViewMatrix();
        state_machine_.CallOnDrawCommandLineSet();
        g_.PageFlip();

        inp_.FrameEnd();

        prev_time = now;

        Sleep(1);
        state_machine_.DoPendingStateChange();
    }

    PostLoop();
    window_.Hide();

    aud_.Shutdown();
    inp_.Shutdown();
    g_.Shutdown();
    window_.Delete();

    Save(SETTINGS_FILENAME);
}



///////////////////////////////////////////////////////////////////////////////
void SageEngine::PreLoop()
{
    // Add all the standard commands.
    CommandPtr cmd(new LoadCellMapCommand(&world_));
    cmd_repo_.Add("CellMap.Load", cmd);
    cmd.reset(new LoadEntityCommand(&world_));
    cmd_repo_.Add("Entity.Load", cmd);
    cmd.reset(new LoadImageAnimationCommand(&world_, &(doc_.ImageAnimationDocuments())));
    cmd_repo_.Add("ImageAnim.Load", cmd);
    cmd.reset(new LoadImageCommand(&world_, &(doc_.ImageDocuments())));
    cmd_repo_.Add("Image.Load", cmd);
    cmd.reset(new LoadMusicCommand(&world_, window_.Handle(), 1234));
    cmd_repo_.Add("Music.Load", cmd);
    cmd.reset(new LoadResourceContextCommand(&world_, &doc_, &aud_, window_.Handle(), 1234));
    cmd_repo_.Add("ResCxt.Load", cmd);
    cmd.reset(new LoadSoundCommand(&world_));
    cmd_repo_.Add("Sound.Load", cmd);
    cmd.reset(new UnloadCellMapCommand(&world_));
    cmd_repo_.Add("CellMap.Unload", cmd);
    cmd.reset(new UnloadEntityCommand(&world_));
    cmd_repo_.Add("Entity.Unload", cmd);
    cmd.reset(new UnloadImageAnimationCommand(&world_));
    cmd_repo_.Add("ImageAnim.Unload", cmd);
    cmd.reset(new UnloadImageCommand(&world_));
    cmd_repo_.Add("Image.Unload", cmd);
    cmd.reset(new UnloadMusicCommand(&world_));
    cmd_repo_.Add("Music.Unload", cmd);
    cmd.reset(new UnloadResourceContextCommand(&world_, &(doc_.ResourceContextDocuments())));
    cmd_repo_.Add("ResCxt.Unload", cmd);
    cmd.reset(new UnloadSoundCommand(&world_));
    cmd_repo_.Add("Sound.Unload", cmd);
    cmd.reset(new DrawCellMapCommand(&world_));
    cmd_repo_.Add("CellMap.Draw", cmd);
    cmd.reset(new ThinkCellMapCommand(&world_));
    cmd_repo_.Add("CellMap.Think", cmd);
    cmd.reset(new ActivateBindingsCommand(&world_, &bindings_));
    cmd_repo_.Add("Bindings.Activate", cmd);
    cmd.reset(new DeactivateBindingsCommand(&world_, &bindings_));
    cmd_repo_.Add("Bindings.Deactivate", cmd);
    cmd.reset(new CameraLookCommand(&world_));
    cmd_repo_.Add("Camera.Look", cmd);
    cmd.reset(new CurrentCameraFirstCommand(&world_));
    cmd_repo_.Add("Camera.Current.First", cmd);
    cmd.reset(new CurrentCameraLastCommand(&world_));
    cmd_repo_.Add("Camera.Current.Last", cmd);
    cmd.reset(new CurrentCameraNextCommand(&world_));
    cmd_repo_.Add("Camera.Current.Next", cmd);
    cmd.reset(new CurrentCameraPrevCommand(&world_));
    cmd_repo_.Add("Camera.Current.Prev", cmd);
    cmd.reset(new CurrentCameraSetCommand(&world_));
    cmd_repo_.Add("Camera.Current.Set", cmd);
    cmd.reset(new MoveCameraBackwardCommand(&world_));
    cmd_repo_.Add("Camera.Move.Backward", cmd);
    cmd.reset(new MoveCameraDownCommand(&world_));
    cmd_repo_.Add("Camera.Move.Down", cmd);
    cmd.reset(new MoveCameraForwardCommand(&world_));
    cmd_repo_.Add("Camera.Move.Forward", cmd);
    cmd.reset(new MoveCameraLeftCommand(&world_));
    cmd_repo_.Add("Camera.Move.Left", cmd);
    cmd.reset(new MoveCameraRightCommand(&world_));
    cmd_repo_.Add("Camera.Move.Right", cmd);
    cmd.reset(new MoveCameraUpCommand(&world_));
    cmd_repo_.Add("Camera.Move.Up", cmd);
    cmd.reset(new CameraToggleCollisionCommand(&world_));
    cmd_repo_.Add("Camera.ToggleCollision", cmd);

    switch(doc_.Open(cmd_line_, app_dir_))
    {
    case GameDocument::OE_OK:
        break;
    case GameDocument::OE_OPEN_FAILURE:
        throw std::runtime_error(std::string("Couldn't open the file [") + cmd_line_ + "] (OE_OPEN_FAILURE)");
    case GameDocument::OE_INVALID_FILE:
        throw std::runtime_error(std::string("Couldn't open the file [") + cmd_line_ + "] (OE_INVALID_FILE)");
    }

    // Load in all the cameras
    CameraDocumentMap& cdm = doc_.CameraDocuments();
    CameraMap* cm = world_.GetCameraMap();
    CameraDocumentMap::iterator cdm_itor;
    for(cdm_itor = cdm.begin(); cdm_itor != cdm.end(); ++cdm_itor)
    {
        Camera c;
        c.Position(cdm_itor->second.Position());
        c.Rotation(cdm_itor->second.Rotation());
        cm->insert(std::make_pair(cdm_itor->first, c));
    }

    // Apply the bindings.
    BindingsFile& bind_docs = doc_.BindingsDocument();
    BindingsFile::Iterator bind_itor;
    for(bind_itor = bind_docs.Begin(); bind_itor != bind_docs.End(); ++bind_itor)
    {
        // bind_itor->first is the binding as a string, ie. "KEY_A", "MSE_MOVE" etc
        // bind_itor->second is the command line that needs to be resolved to a 'BindingEvents' pointer

        int binding = Inp::Bindings::ToBinding(bind_itor->first);
        if(binding != -1)
        {
            std::vector<std::string> cl = Util::Split(bind_itor->second);

            CommandPtr cmd = cmd_repo_.Find(cl[0]);
            if(!cmd)
            {
                throw std::runtime_error(std::string("Invalid command [") + cl[0] + "] specified in a bind command line");
            }

            CommandInvokeContextPtr inv_context = cmd->CreateCommandInvocation(cl);
            Inp::BindingEventsPtr bind_handler = boost::shared_polymorphic_cast<Inp::BindingEvents, CommandInvokeContext>(inv_context);
            bindings_.Set(binding, bind_handler);
        }
    }

    // Extract the game's state machine from the document and setup the "state_machine_" object with
    // the data found.
    if(doc_.StateMachine().empty())
    {
        throw std::runtime_error(std::string("The game from the file [") + cmd_line_ + "] has no assigned state machine");
    }

    StateMachineDocumentMap sm_docs = doc_.StateMachineDocuments();
    StateMachineDocumentMap::iterator sm_itor = sm_docs.find(doc_.StateMachine());
    if(sm_itor == sm_docs.end())
    {
        throw std::runtime_error(std::string("The state machine [") + doc_.StateMachine() + "] from the game [" + cmd_line_ + "] could not be found in the game's list of state machines");
    }

    StateMachineDocument& sm = sm_itor->second;
    SmStateDocumentList& state_docs = sm.StateDocuments();
    SmStateDocumentList::iterator state_itor;
    for(state_itor = state_docs.begin(); state_itor != state_docs.end(); ++state_itor)
    {
        StatePtr new_state(new State(&state_machine_));

        DocCommandLineSets& ecls = state_itor->EventCommandLineSets();
        DocCommandLineSets::iterator ecls_itor = ecls.find("OnBegin");
        if(ecls_itor != ecls.end())
        {
            DocCommandLineSet& cls = ecls_itor->second;
            DocCommandLineSet::iterator cls_itor;
            for(cls_itor = cls.begin(); cls_itor != cls.end(); ++cls_itor)
            {
                DocCommandLine& cl = *cls_itor;
                CommandPtr cmd = cmd_repo_.Find(cl[0]);
                if(!cmd)
                {
                    throw std::runtime_error(std::string("Invalid command [") + cl[0] + "] specified in a command line");
                }

                CommandInvokeContextPtr inv_context = cmd->CreateCommandInvocation(cl);
                new_state->OnBeginCommandLineSet().GetCommandInvocations().push_back(inv_context);
            }
        }

        ecls_itor = ecls.find("OnEnd");
        if(ecls_itor != ecls.end())
        {
            DocCommandLineSet& cls = ecls_itor->second;
            DocCommandLineSet::iterator cls_itor;
            for(cls_itor = cls.begin(); cls_itor != cls.end(); ++cls_itor)
            {
                DocCommandLine& cl = *cls_itor;
                CommandPtr cmd = cmd_repo_.Find(cl[0]);
                if(!cmd)
                {
                    throw std::runtime_error(std::string("Invalid command [") + cl[0] + "] specified in a command line");
                }

                CommandInvokeContextPtr inv_context = cmd->CreateCommandInvocation(cl);
                new_state->OnEndCommandLineSet().GetCommandInvocations().push_back(inv_context);
            }
        }

        ecls_itor = ecls.find("OnThink");
        if(ecls_itor != ecls.end())
        {
            DocCommandLineSet& cls = ecls_itor->second;
            DocCommandLineSet::iterator cls_itor;
            for(cls_itor = cls.begin(); cls_itor != cls.end(); ++cls_itor)
            {
                DocCommandLine& cl = *cls_itor;
                CommandPtr cmd = cmd_repo_.Find(cl[0]);
                if(!cmd)
                {
                    throw std::runtime_error(std::string("Invalid command [") + cl[0] + "] specified in a command line");
                }

                CommandInvokeContextPtr inv_context = cmd->CreateCommandInvocation(cl);
                new_state->OnThinkCommandLineSet().GetCommandInvocations().push_back(inv_context);
            }
        }

        ecls_itor = ecls.find("OnMove");
        if(ecls_itor != ecls.end())
        {
            DocCommandLineSet& cls = ecls_itor->second;
            DocCommandLineSet::iterator cls_itor;
            for(cls_itor = cls.begin(); cls_itor != cls.end(); ++cls_itor)
            {
                DocCommandLine& cl = *cls_itor;
                CommandPtr cmd = cmd_repo_.Find(cl[0]);
                if(!cmd)
                {
                    throw std::runtime_error(std::string("Invalid command [") + cl[0] + "] specified in a command line");
                }

                CommandInvokeContextPtr inv_context = cmd->CreateCommandInvocation(cl);
                new_state->OnMoveCommandLineSet().GetCommandInvocations().push_back(inv_context);
            }
        }

        ecls_itor = ecls.find("OnDraw");
        if(ecls_itor != ecls.end())
        {
            DocCommandLineSet& cls = ecls_itor->second;
            DocCommandLineSet::iterator cls_itor;
            for(cls_itor = cls.begin(); cls_itor != cls.end(); ++cls_itor)
            {
                DocCommandLine& cl = *cls_itor;
                CommandPtr cmd = cmd_repo_.Find(cl[0]);
                if(!cmd)
                {
                    throw std::runtime_error(std::string("Invalid command [") + cl[0] + "] specified in a command line");
                }

                CommandInvokeContextPtr inv_context = cmd->CreateCommandInvocation(cl);
                new_state->OnDrawCommandLineSet().GetCommandInvocations().push_back(inv_context);
            }
        }

        ecls_itor = ecls.find("OnTimer");
        if(ecls_itor != ecls.end())
        {
            DocCommandLineSet& cls = ecls_itor->second;
            DocCommandLineSet::iterator cls_itor;
            for(cls_itor = cls.begin(); cls_itor != cls.end(); ++cls_itor)
            {
                DocCommandLine& cl = *cls_itor;
                CommandPtr cmd = cmd_repo_.Find(cl[0]);
                if(!cmd)
                {
                    throw std::runtime_error(std::string("Invalid command [") + cl[0] + "] specified in a command line");
                }

                CommandInvokeContextPtr inv_context = cmd->CreateCommandInvocation(cl);
                new_state->OnTimerCommandLineSet().GetCommandInvocations().push_back(inv_context);
            }
        }

        state_machine_.AddState(state_itor->Name(), new_state);
    }

    // Get the "entry node", get the entry node's event, and determine the
    // state at the other end of the event.
    SmEntryDocumentList& entry_docs = sm.EntryDocuments();
    if(entry_docs.empty())
    {
        throw std::runtime_error(std::string("The state machine [") + doc_.StateMachine() + "] from the game [" + cmd_line_ + "] has no Entry Nodes");
    }

    SmEventDocumentList& event_docs = sm.EventDocuments();
    if(event_docs.empty())
    {
        throw std::runtime_error(std::string("The state machine [") + doc_.StateMachine() + "] from the game [" + cmd_line_ + "] has no Event Nodes");
    }

    // Which event instance is connected to this entry node?
    SmEventDocumentList::iterator event_itor;
    for(event_itor = event_docs.begin(); event_itor != event_docs.end(); ++event_itor)
    {
        if(event_itor->SourceNodeId() == entry_docs.begin()->Id())
        {
            break;
        }
    }

    if(event_itor == event_docs.end())
    {
        throw std::runtime_error(std::string("No event is connected to the Entry Node in the state machine [") + doc_.StateMachine() + "] from the game [" + cmd_line_ + "]");
    }

    // Finally, get the name of state connected to this event, so that we can make
    // this state the current state of the engine.
    for(state_itor = state_docs.begin(); state_itor != state_docs.end(); ++state_itor)
    {
        if(event_itor->DestNodeId() == state_itor->Id())
        {
            break;
        }
    }

    if(event_itor == event_docs.end())
    {
        throw std::runtime_error(std::string("There is no state connected to the event that starts the state machine [") + doc_.StateMachine() + "] from the game [" + cmd_line_ + "]");
    }

    state_machine_.SetCurrentState(state_itor->Name());

    // For now, place the camera in the first cell in the list.
    Camera* camera = world_.CurrentCamera();
    CellMapDocument* map_doc = world_.GetCellMap();
    if(camera && map_doc)
    {
        CellContainer::iterator i = map_doc->GetCellMap().Cells_.begin();
        camera->Position(Math::Vector(
            i->second.min_x_ + (map_doc->CellSize()/2.0f),
            -map_doc->CellSize() + 80.0f,
            i->second.min_z_ + (map_doc->CellSize()/2.0f)));
    }

    g_.SetClearFlags(doc_.ClearBackBuffer(), true);
}

///////////////////////////////////////////////////////////////////////////////
void SageEngine::PostLoop()
{
    doc_.Close();
}

///////////////////////////////////////////////////////////////////////////////
void SageEngine::SetModelViewMatrix()
{
    Camera* camera = world_.CurrentCamera();
    if(camera)
    {
        glRotatef(-camera->Rotation().x_, 1.0f, 0.0f, 0.0f );
        glRotatef(-camera->Rotation().y_, 0.0f, 1.0f, 0.0f );
        glTranslatef(-camera->Position().x_, -camera->Position().y_, -camera->Position().z_);
    }
}





///////////////////////////////////////////////////////////////////////////////
void SageEngine::OnKeyPressed(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(!window_.IsFullscreenStyle() && mouse_captured_)
    {
        if((key == DIK_LMENU || key == DIK_RMENU) && ctrl_held)
        {
            mouse_captured_ = false;

            while(ShowCursor(TRUE) < 0) ;
            ClipCursor(NULL);

            window_.SetTitle("SageEngine - [Left click in this window to capture the mouse cursor]");
        }
    }

    switch(key)
    {
    case DIK_ESCAPE:
        PostQuitMessage(0);     // Temporary!
        break;
    }

    bindings_.OnKeyPressed(key);
}

///////////////////////////////////////////////////////////////////////////////
void SageEngine::OnKeyHeld(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    bindings_.OnKeyHeld(key);
}

///////////////////////////////////////////////////////////////////////////////
void SageEngine::OnKeyReleased(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    bindings_.OnKeyReleased(key);
}

///////////////////////////////////////////////////////////////////////////////
void SageEngine::OnMouseMoved(int x_delta, int y_delta)
{
    if(mouse_captured_)
    {
        bindings_.OnMouseMoved(x_delta, y_delta);
    }
}

///////////////////////////////////////////////////////////////////////////////
void SageEngine::OnMouseButtonPressed(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(mouse_captured_)
    {
        bindings_.OnMouseButtonPressed(button);
    }
}

///////////////////////////////////////////////////////////////////////////////
void SageEngine::OnMouseButtonHeld(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(mouse_captured_)
    {
        bindings_.OnMouseButtonHeld(button);
    }
}

///////////////////////////////////////////////////////////////////////////////
void SageEngine::OnMouseButtonReleased(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(mouse_captured_)
    {
        bindings_.OnMouseButtonReleased(button);
    }
}

///////////////////////////////////////////////////////////////////////////////
bool SageEngine::OnWindowClosed(Gfx::Window* window)
{
    PostQuitMessage(0);
    return true;    // Let DefWindowProc() destroy the window
}

///////////////////////////////////////////////////////////////////////////////
bool SageEngine::OnWindowEraseBackground(Gfx::Window* window, HDC dc)
{
    state_machine_.CallOnDrawCommandLineSet();
    return false;   // Don't let DefWindowProc() erase the background
}

///////////////////////////////////////////////////////////////////////////////
void SageEngine::OnWindowOtherMessage(UINT msg, WPARAM wparam, LPARAM lparam)
{
}

///////////////////////////////////////////////////////////////////////////////
void SageEngine::OnWindowMouseLPressed(Gfx::Window* window)
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

        window_.SetTitle("SageEngine - [Press Ctrl+Alt to release the mouse cursor]");
    }
}

///////////////////////////////////////////////////////////////////////////////
void SageEngine::OnMouseWheelRotatedBackwards(short delta)
{
    bindings_.OnMouseWheelMoved(delta);
}

///////////////////////////////////////////////////////////////////////////////
void SageEngine::OnMouseWheelRotatedForwards(short delta)
{
    bindings_.OnMouseWheelMoved(delta);
}

///////////////////////////////////////////////////////////////////////////////
void SageEngine::OnWindowActivated(Gfx::Window* window)
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

    // ...
}

///////////////////////////////////////////////////////////////////////////////
void SageEngine::OnWindowDeactived(Gfx::Window* window)
{
    while(ShowCursor(TRUE) < 0) ;
    ClipCursor(NULL);

    inp_.Deactivate();

    // ...
}







///////////////////////////////////////////////////////////////////////////////
bool SageEngine::SetFullscreen()
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
void SageEngine::OnFullscreenGraphicsSettingChanged()
{
    if(FullscreenGraphics())
    {
        if(!window_.IsFullscreenStyle())
        {
            if(SetFullscreen())
            {
                window_.ToggleBetweenStyles();
                window_.SetTitle("SageEngine");
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
            window_.SetTitle("SageEngine - [Press Ctrl+Alt to release the mouse cursor]");
        }
    }
}

void SageEngine::OnSoundEnabledSettingChanged()
{
    SoundEnabled() ? aud_.Enable() : aud_.Disable();
}

void SageEngine::OnMusicEnabledSettingChanged()
{
    //MusicEnabled() ? mus_tracks_[current_track_].Play() : mus_tracks_[current_track_].Stop();
}

void SageEngine::OnSoundVolumeSettingChanged()
{
    //resources_.ApplySoundVolume(SoundVolume());
}

void SageEngine::OnMusicVolumeSettingChanged()
{
    //mus_tracks_[0].Volume(MusicVolume());
    //mus_tracks_[1].Volume(MusicVolume());
    //mus_tracks_[2].Volume(MusicVolume());
    //mus_tracks_[3].Volume(MusicVolume());
}
