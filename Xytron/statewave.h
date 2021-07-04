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
#ifndef INCLUDED_STATEWAVE
#define INCLUDED_STATEWAVE

#include "state.h"
#include "basicenemyentity.h"

#include "../math/vector.h"
#include "../aud/aud.h"

#include <string>

///////////////////////////////////////////////////////////////////////////////
class StateWave
    : public State
    , public BasicEnemyEntityEvents
{
public:
    StateWave(IStateController* state_controller, IWorld* world, Timer::Manager* timer_manager);
    ~StateWave();

    void LoadResources(Util::ResourceContext& resources);

    void OnStateBegin();
    void OnStateEnd();

    void Think(float time_delta);
    void Move(float time_delta);
    void Collide();
    void Draw2d(const Gfx::Graphics& g);
    void FrameEnd();

    void Resumed(bool wave_resumed) { wave_resumed_ = wave_resumed; }

    void OnTimeout(int id);

    void OnKeyPressed(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnMouseButtonPressed(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnMouseButtonHeld(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held);
    void OnMouseMoved(int x_delta, int y_delta);

    void OnPlayerDeath();

    void SkipToBoss();

    // Implementation of BasicEnemyEntityEvents
    void OnEnemyDestroyed(Gfx::IEntityPtr enemy_ship);
    void OnEnemyExpired(Gfx::IEntityPtr enemy_ship);
    void OnEnemyFreed(Gfx::IEntityPtr enemy_ship);

private:
    void InitialiseEnemyGeneration();
    void GenerateEnemies(float time_delta);
    void GenerateEnemyBatch();

    Math::Vector ChooseOffscreenPoint() const;
    Math::Vector ChooseOnscreenPoint() const;
    Math::Vector ChooseTopOffscreenPoint() const;
    Math::Vector ChooseTopOnscreenPoint() const;
    Math::VectorList GenerateOnScreenWaypoints(unsigned int num_waypoints) const;
    Math::VectorList GenerateTopOnScreenWaypoints(unsigned int num_waypoints) const;

    enum EntityType { ET_BASIC, ET_SITANDSHOOT, ET_RAIL, ET_SPRAY, ET_PLASMASITANDSHOOT, ET_ROCKETHOVER };
    void InsertEntities(float min_percent, float max_percent, unsigned int min_wave, unsigned int max_wave, EntityType type, unsigned int& remaining);

    void CreateBoss();

private:
    std::string level_warp_;
    unsigned int num_entities_dead_;

    bool level_warp_active_;
    bool bomb_;
    bool wave_resumed_;
    bool boss_spawned_;
    bool show_boss_intro_;

    float generate_time_;

    unsigned int num_remaining_batches_;
    unsigned int ideal_num_enemies_;
    unsigned int num_enemies_in_batch_;
    unsigned int num_simault_batches_;
    unsigned int follow_modulus_;
    unsigned int follow_counter_;

    std::vector<EntityType> batch_types_;
    Aud::SoundPtr snd_alert_;
    Gfx::ImagePtr img_miniboss_;
    Gfx::ImagePtr img_boss_;
    Gfx::ImagePtr img_boss_intro_;
};

#endif  // INCLUDED_STATEWAVE
