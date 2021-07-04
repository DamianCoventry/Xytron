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
#include "statewave.h"
#include "istatecontroller.h"
#include "iworld.h"
#include "xytron.h"
#include "playerentity.h"
#include "pickupentity.h"
#include "sitandshootenemyentity.h"
#include "sprayshootenemyentity.h"
#include "railenemyentity.h"
#include "plasmaenemyentity.h"
#include "rocketenemyentity.h"

#include "miniboss0enemyentity.h"
#include "miniboss1enemyentity.h"
#include "miniboss2enemyentity.h"

#include "boss0enemyentity.h"
#include "boss1enemyentity.h"
#include "boss2enemyentity.h"

#include <boost/lexical_cast.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace
{
    const int TIMER_ID_END_OF_WAVE  = 0;
    const int TIMER_SHOW_BOSS_INTRO = 1;
    const int TIMER_HIDE_BOSS_INTRO = 2;
}

///////////////////////////////////////////////////////////////////////////////
StateWave::StateWave(IStateController* state_controller, IWorld* world, Timer::Manager* timer_manager)
: State(state_controller, world, timer_manager)
, level_warp_active_(false)
, bomb_(false)
, wave_resumed_(false)
, boss_spawned_(false)
, num_entities_dead_(0)
, num_remaining_batches_(0)
, num_simault_batches_(0)
, follow_modulus_(0)
, follow_counter_(0)
, generate_time_(0.0f)
{
}

///////////////////////////////////////////////////////////////////////////////
StateWave::~StateWave()
{
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::LoadResources(Util::ResourceContext& resources)
{
    snd_alert_ = resources.FindSound("Sounds/Alert.wav");

    Gfx::TexturePtr tex = resources.LoadTexture("Images/MiniBossApproaching.tga");
    img_miniboss_ = Gfx::Image::CreateFromTexture(tex, 0, 0, 400, 200);

    tex = resources.LoadTexture("Images/BossApproaching.tga");
    img_boss_ = Gfx::Image::CreateFromTexture(tex, 0, 0, 400, 200);
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::OnStateBegin()
{
    level_warp_active_ = false;

    if(!wave_resumed_)
    {
        boss_spawned_       = false;
        bomb_               = false;
        show_boss_intro_    = false;
        InitialiseEnemyGeneration();
    }
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::OnStateEnd()
{
    State::OnStateEnd();
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::OnMouseButtonPressed(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(button == 1)
    {
        World()->Player()->ShootSecondary();
    }
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::OnMouseButtonHeld(int button, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    if(button == 0)
    {
        World()->Player()->ShootPrimary();
    }
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::Think(float time_delta)
{
    World()->Think(time_delta);

    GenerateEnemies(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::Move(float time_delta)
{
    World()->Move(time_delta);
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::Collide()
{
    World()->Collide();
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::Draw2d(const Gfx::Graphics& g)
{
    World()->Draw();
    if(show_boss_intro_)
    {
        img_boss_intro_->Draw2d(g, Math::Vector(400.0f, 300.0f, 0.0f));
    }
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::FrameEnd()
{
    World()->FrameEnd();
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::OnTimeout(int id)
{
    switch(id)
    {
    case TIMER_ID_END_OF_WAVE:
        // The level has ended.  Which state do we go to now?
        if(World()->Player()->RemainingShips() == 0)
        {
            // The player has no more ships left, the game is over.
            StateController()->ChangeToStateGameOver();
        }
        else if(World()->AllEnemyEntitiesDead() && num_remaining_batches_ == 0)
        {
            // All the enemies have been destroyed, move to the next level.
            StateController()->ChangeToStatePostWave();
        }
        else // The player must have died
        {
            StateController()->ChangeToStateGetReady(true);     // true = respawn player
        }
        break;
    case TIMER_SHOW_BOSS_INTRO:
        CreateBoss();
        break;
    case TIMER_HIDE_BOSS_INTRO:
        show_boss_intro_ = false;
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::OnKeyPressed(int key, bool shift_held, bool ctrl_held, bool alt_held, bool win_held)
{
    switch(key)
    {
    case DIK_ESCAPE:
        StateController()->ChangeToStatePaused();
        break;
    case DIK_I:
        World()->ToggleInvulnerability();
        break;
    case DIK_S:
        World()->SkipToNextWave();
        break;
    case DIK_END:
        World()->SetWave(29);
        StateController()->ChangeToStatePreWave(true);
        break;
    case DIK_L:
        level_warp_active_ = !level_warp_active_;
        level_warp_.clear();
        break;
    case DIK_P:
        World()->Player()->AwardPoints(10000);
        break;
    case DIK_W:
        World()->Player()->PrimaryWeapon()->IncUpgrade();
        break;
    case DIK_B:
        World()->SkipToBoss();
        break;
    case DIK_0: if(level_warp_active_) level_warp_ += '0'; break;
    case DIK_1: if(level_warp_active_) level_warp_ += '1'; break;
    case DIK_2: if(level_warp_active_) level_warp_ += '2'; break;
    case DIK_3: if(level_warp_active_) level_warp_ += '3'; break;
    case DIK_4: if(level_warp_active_) level_warp_ += '4'; break;
    case DIK_5: if(level_warp_active_) level_warp_ += '5'; break;
    case DIK_6: if(level_warp_active_) level_warp_ += '6'; break;
    case DIK_7: if(level_warp_active_) level_warp_ += '7'; break;
    case DIK_8: if(level_warp_active_) level_warp_ += '8'; break;
    case DIK_9: if(level_warp_active_) level_warp_ += '9'; break;
    case DIK_RETURN:
    case DIK_NUMPADENTER:
        if(level_warp_active_)
        {
            if(!level_warp_.empty())
            {
                World()->SkipToWave(boost::lexical_cast<unsigned int>(level_warp_));
            }
            level_warp_active_ = false;
        }
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::OnMouseMoved(int x_delta, int y_delta)
{
    World()->Player()->OnMouseMoved(x_delta, y_delta);
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::OnPlayerDeath()
{
    if(World()->AllEnemyEntitiesDead() && num_remaining_batches_ == 0)
    {
        if(((World()->GetWave()+1) % 5) == 0 && !boss_spawned_)
        {
            boss_spawned_ = true;
            AddTimeout(TIMER_SHOW_BOSS_INTRO, 1500);
        }
        else
        {
            num_entities_dead_ = 0;
            AddTimeout(TIMER_ID_END_OF_WAVE, 1000);
        }
    }
    else
    {
        AddTimeout(TIMER_ID_END_OF_WAVE, 1000);
    }
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::SkipToBoss()
{
    batch_types_.clear();
    num_remaining_batches_ = 0;
    boss_spawned_ = true;
    CreateBoss();
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::OnEnemyDestroyed(Gfx::IEntityPtr enemy_ship)
{
    BasicEnemyEntity* small_entity = static_cast<BasicEnemyEntity*>(enemy_ship.get());
    World()->Player()->AwardPoints(small_entity->Points());

    // Don't spawn a pickup if this is the last enemy
    if(!(World()->NumEnemyEntities() == 1 && num_remaining_batches_ == 0))
    {
        ++num_entities_dead_;
        if(num_entities_dead_ % 20 == 0)
        {
            PickupEntity::PickupType type;
            if(bomb_)
            {
                switch(rand()%4)
                {
                case 0: type = PickupEntity::PICKUP_BOMB_NUKE; break;
                case 1: type = PickupEntity::PICKUP_BOMB_SCATTER; break;
                case 2: type = PickupEntity::PICKUP_BOMB_FUNKY; break;
                case 3: type = PickupEntity::PICKUP_BOMB_EMP; break;
                }
            }
            else
            {
                switch(rand()%5)
                {
                case 0: type = PickupEntity::PICKUP_GUN_CANNON; break;
                case 1: type = PickupEntity::PICKUP_GUN_LASER; break;
                case 2: type = PickupEntity::PICKUP_GUN_MISSILES; break;
                case 3: type = PickupEntity::PICKUP_GUN_PLASMA; break;
                case 4: type = PickupEntity::PICKUP_GUN_RAIL; break;
                }
            }
            bomb_ = !bomb_;

            Gfx::IEntityPtr entity(new PickupEntity(type, World()));
            PickupEntity* pickup = static_cast<PickupEntity*>(entity.get());
            pickup->BindResources(World()->ResourceContext());
            pickup->Position(enemy_ship->Position());
            World()->PickupEntityList().push_back(entity);
        }
        else if(num_entities_dead_ % 30 == 0)
        {
            std::vector<PickupEntity::PickupType> types;
            types.push_back(PickupEntity::PICKUP_BONUS_100POINTS);
            types.push_back(PickupEntity::PICKUP_BONUS_1000POINTS);

            if(World()->Player()->RemainingShips() < PlayerEntity::MAX_SHIPS)
            {
                types.push_back(PickupEntity::PICKUP_BONUS_EXTRASHIP);
            }
            if(World()->Player()->HitPoints() < PlayerEntity::FULL_HITPOINTS)
            {
                types.push_back(PickupEntity::PICKUP_BONUS_FULLHITPOINTS);
            }

            Gfx::IEntityPtr entity(new PickupEntity(types[rand()%types.size()], World()));
            PickupEntity* pickup = static_cast<PickupEntity*>(entity.get());
            pickup->BindResources(World()->ResourceContext());
            pickup->Position(enemy_ship->Position());
            World()->PickupEntityList().push_back(entity);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::OnEnemyExpired(Gfx::IEntityPtr enemy_ship)
{
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::OnEnemyFreed(Gfx::IEntityPtr enemy_ship)
{
    if(World()->AllEnemyEntitiesDead() && num_remaining_batches_ == 0)
    {
        if(((World()->GetWave()+1) % 5) == 0 && !boss_spawned_)
        {
            boss_spawned_ = true;
            AddTimeout(TIMER_SHOW_BOSS_INTRO, 1500);
        }
        else
        {
            num_entities_dead_ = 0;
            AddTimeout(TIMER_ID_END_OF_WAVE, 1000);
        }
    }
}






///////////////////////////////////////////////////////////////////////////////
void StateWave::InitialiseEnemyGeneration()
{
    float percent_thru_game = float(World()->GetWave()) / float(Xytron::MAX_WAVES-1);

    batch_types_.clear();

    // Overall wave constants
    ideal_num_enemies_      = 10 + unsigned int(5.0f * percent_thru_game);
    num_remaining_batches_  = 10 + unsigned int(30.0f * percent_thru_game);
    num_enemies_in_batch_   = 4 + unsigned int(2.0f * percent_thru_game);
    num_simault_batches_    = 1 + unsigned int(2.0f * percent_thru_game);
    follow_modulus_         = 6 - unsigned int(4.0f * percent_thru_game);

    unsigned int remaining = num_remaining_batches_;
    switch(World()->GetWave()/6)
    {
    case 0:         // Levels 0-5
        InsertEntities(0.20f, 0.35f, 0, 5, ET_SITANDSHOOT, remaining);
        InsertEntities(0.05f, 0.20f, 0, 5, ET_SPRAY, remaining);
        InsertEntities(0.05f, 0.10f, 0, 5, ET_ROCKETHOVER, remaining);
        break;
    case 1:         // Levels 6-11
        InsertEntities(0.18f, 0.20f, 6, 11, ET_SITANDSHOOT, remaining);
        InsertEntities(0.16f, 0.30f, 6, 11, ET_SPRAY, remaining);
        InsertEntities(0.15f, 0.15f, 6, 11, ET_PLASMASITANDSHOOT, remaining);
        InsertEntities(0.17f, 0.18f, 6, 11, ET_ROCKETHOVER, remaining);
        break;
    case 2:         // Levels 12-17
        InsertEntities(0.05f, 0.10f, 12, 17, ET_SITANDSHOOT, remaining);
        InsertEntities(0.10f, 0.15f, 12, 17, ET_SPRAY, remaining);
        InsertEntities(0.20f, 0.25f, 12, 17, ET_RAIL, remaining);
        InsertEntities(0.12f, 0.15f, 12, 17, ET_PLASMASITANDSHOOT, remaining);
        InsertEntities(0.10f, 0.15f, 12, 17, ET_ROCKETHOVER, remaining);
        break;
    case 3:         // Levels 18-23
        InsertEntities(0.10f, 0.15f, 18, 23, ET_SITANDSHOOT, remaining);
        InsertEntities(0.17f, 0.20f, 18, 23, ET_SPRAY, remaining);
        InsertEntities(0.09f, 0.10f, 18, 23, ET_RAIL, remaining);
        InsertEntities(0.18f, 0.20f, 18, 23, ET_PLASMASITANDSHOOT, remaining);
        InsertEntities(0.18f, 0.20f, 18, 23, ET_ROCKETHOVER, remaining);
        break;
    case 4:         // Levels 24-29
        InsertEntities(0.10f, 0.12f, 24, 29, ET_SITANDSHOOT, remaining);
        InsertEntities(0.10f, 0.17f, 24, 29, ET_SPRAY, remaining);
        InsertEntities(0.15f, 0.17f, 24, 29, ET_RAIL, remaining);
        InsertEntities(0.15f, 0.20f, 24, 29, ET_PLASMASITANDSHOOT, remaining);
        InsertEntities(0.15f, 0.20f, 24, 29, ET_ROCKETHOVER, remaining);
        break;
    }

    // Make all the remaining entities the "basic" type
    if(remaining > 0)
    {
        std::vector<EntityType> temp(remaining, ET_BASIC);
        std::copy(temp.begin(), temp.end(), std::back_inserter(batch_types_));
    }

    World()->Clear();
    World()->PlacePlayerAtStartPosition();

    follow_counter_ = 0;
    generate_time_  = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::GenerateEnemies(float time_delta)
{
    if(num_remaining_batches_ == 0)
    {
        return;
    }

    if(World()->AllEnemyEntitiesDead())
    {
        GenerateEnemyBatch();
    }
    else if(World()->NumEnemyEntities() < ideal_num_enemies_)
    {
        // Wait 2 seconds before generating more enemies
        generate_time_ += time_delta;
        if(generate_time_ >= 2.0f)
        {
            generate_time_ = 0.0f;
            GenerateEnemyBatch();
        }
    }
    // else do nothing, there's already enough entities on the screen
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::GenerateEnemyBatch()
{
    float percent_thru_game = float(World()->GetWave()) / float(Xytron::MAX_WAVES-1);

    unsigned int min_shoot_ms           = 2500 - unsigned int(250.0f * percent_thru_game);
    unsigned int max_shoot_ms           = 3500 - unsigned int(250.0f * percent_thru_game);
    unsigned int min_sit_timeout_ms     = 4000 + unsigned int(250.0f * percent_thru_game);
    unsigned int max_sit_timeout_ms     = 8000 + unsigned int(500.0f * percent_thru_game);
    unsigned int hit_points             =    7 + unsigned int( 15.0f * percent_thru_game);
    unsigned int num_waypoints          =   10 + unsigned int(  5.0f * percent_thru_game);
    unsigned long points                =    5 + unsigned int( 15.0f * percent_thru_game);

    float shot_speed = 200.0f + (50.0f * percent_thru_game);
    float ship_speed = 150.0f + (50.0f * percent_thru_game);

    for(unsigned int batch_index = 0; batch_index < num_simault_batches_ && num_remaining_batches_ > 0; batch_index++)
    {
        num_remaining_batches_--;

        // Choose a random batch from the batch type list.
        unsigned int type_index = unsigned int(rand()%batch_types_.size());
        switch(batch_types_[type_index])
        {
        case ET_BASIC:
            {
                follow_counter_++;
                bool follow_same_list = (follow_counter_%follow_modulus_ != 0);

                Math::Vector start_position(ChooseOffscreenPoint());
                Math::VectorList way_points(GenerateOnScreenWaypoints(num_waypoints));

                for(unsigned int i = 0; i < num_enemies_in_batch_; i++)
                {
                    Gfx::IEntityPtr entity(new BasicEnemyEntity(this, World()));
                    BasicEnemyEntity* enemy = static_cast<BasicEnemyEntity*>(entity.get());

                    float start_timeout = 0.5f;
                    if(follow_same_list)
                    {
                        start_timeout += (float(i) * 0.5f);
                    }

                    enemy->BindResources(World()->ResourceContext());
                    enemy->StartTimeout(start_timeout);
                    enemy->MinShootTimeout(min_shoot_ms);
                    enemy->MaxShootTimeout(max_shoot_ms);
                    enemy->Position(follow_same_list ? start_position : ChooseOffscreenPoint());
                    enemy->Points(points);
                    enemy->ShotSpeed(shot_speed);
                    enemy->MaxSpeed(ship_speed);
                    enemy->HitPoints(hit_points);
                    enemy->WayPoints(follow_same_list ? way_points : GenerateOnScreenWaypoints(num_waypoints));

                    World()->EnemyEntityList().push_back(entity);
                }
                break;
            }
        case ET_SITANDSHOOT:
            {
                for(unsigned int i = 0; i < num_enemies_in_batch_; i++)
                {
                    Gfx::IEntityPtr entity(new SitAndShootEnemyEntity(this, World()));
                    SitAndShootEnemyEntity* enemy = static_cast<SitAndShootEnemyEntity*>(entity.get());

                    enemy->BindResources(World()->ResourceContext());
                    enemy->StartTimeout(0.5f);
                    enemy->MinShootTimeout(((min_shoot_ms / 4) < 100) ? 100 : (min_shoot_ms / 4));
                    enemy->MaxShootTimeout(((max_shoot_ms / 4) < 100) ? 100 : (max_shoot_ms / 4));
                    enemy->Position(ChooseOffscreenPoint());
                    enemy->Points(points);
                    enemy->ShotSpeed(shot_speed);
                    enemy->MaxSpeed(ship_speed*1.5f);
                    enemy->HitPoints(hit_points);
                    enemy->WayPoints(GenerateTopOnScreenWaypoints(num_waypoints));
                    enemy->SitMinTimeout(min_sit_timeout_ms);
                    enemy->SitMaxTimeout(max_sit_timeout_ms);

                    World()->EnemyEntityList().push_back(entity);
                }
                break;
            }
        case ET_SPRAY:
            {
                for(unsigned int i = 0; i < num_enemies_in_batch_; i++)
                {
                    Gfx::IEntityPtr entity(new SprayShootEnemyEntity(this, World()));
                    SprayShootEnemyEntity* enemy = static_cast<SprayShootEnemyEntity*>(entity.get());

                    enemy->BindResources(World()->ResourceContext());
                    enemy->StartTimeout(0.5f);
                    enemy->MinShootTimeout(((min_shoot_ms / 2) < 100) ? 100 : (min_shoot_ms / 2));
                    enemy->MaxShootTimeout(((max_shoot_ms / 2) < 100) ? 100 : (max_shoot_ms / 2));
                    enemy->Position(ChooseOffscreenPoint());
                    enemy->Points(points);
                    enemy->ShotSpeed(shot_speed);
                    enemy->MaxSpeed(ship_speed*1.5f);
                    enemy->HitPoints(hit_points);
                    enemy->WayPoints(GenerateTopOnScreenWaypoints(num_waypoints));
                    enemy->SitMinTimeout(min_sit_timeout_ms);
                    enemy->SitMaxTimeout(max_sit_timeout_ms);
                    enemy->NumShots(4+rand()%4);
                    enemy->SpreadAngle(50.0f + float(rand()%30));
                    enemy->ShotTimeout(0.1f + (float(rand()%100)/1000.0f));

                    World()->EnemyEntityList().push_back(entity);
                }
                break;
            }
        case ET_RAIL:
            {
                unsigned int count = num_enemies_in_batch_*7/10;
                for(unsigned int i = 0; i < count; i++)
                {
                    Gfx::IEntityPtr entity(new RailEnemyEntity(this, World()));
                    RailEnemyEntity* enemy = static_cast<RailEnemyEntity*>(entity.get());

                    enemy->BindResources(World()->ResourceContext());
                    enemy->StartTimeout(0.5f);
                    enemy->MinShootTimeout(((min_shoot_ms / 2) < 100) ? 100 : (min_shoot_ms / 2));
                    enemy->MaxShootTimeout(((max_shoot_ms / 2) < 100) ? 100 : (max_shoot_ms / 2));
                    enemy->Position(ChooseOffscreenPoint());
                    enemy->Points(points);
                    enemy->ShotSpeed(shot_speed);
                    enemy->MaxSpeed(ship_speed);
                    enemy->HitPoints(hit_points);
                    enemy->WayPoints(GenerateTopOnScreenWaypoints(num_waypoints*2));

                    World()->EnemyEntityList().push_back(entity);
                }
                break;
            }
        case ET_PLASMASITANDSHOOT:
            {
                for(unsigned int i = 0; i < num_enemies_in_batch_; i++)
                {
                    Gfx::IEntityPtr entity(new PlasmaEnemyEntity(this, World()));
                    PlasmaEnemyEntity* enemy = static_cast<PlasmaEnemyEntity*>(entity.get());

                    enemy->BindResources(World()->ResourceContext());
                    enemy->StartTimeout(0.5f);
                    enemy->MinShootTimeout(((min_shoot_ms / 2) < 100) ? 100 : (min_shoot_ms / 2));
                    enemy->MaxShootTimeout(((max_shoot_ms / 2) < 100) ? 100 : (max_shoot_ms / 2));
                    enemy->Position(ChooseOffscreenPoint());
                    enemy->Points(points);
                    enemy->ShotSpeed(shot_speed);
                    enemy->MaxSpeed(ship_speed*1.5f);
                    enemy->HitPoints(hit_points);
                    enemy->WayPoints(GenerateTopOnScreenWaypoints(num_waypoints));
                    enemy->SitMinTimeout(min_sit_timeout_ms);
                    enemy->SitMaxTimeout(max_sit_timeout_ms);

                    World()->EnemyEntityList().push_back(entity);
                }
                break;
            }
        case ET_ROCKETHOVER:
            {
                for(unsigned int i = 0; i < num_enemies_in_batch_; i++)
                {
                    Gfx::IEntityPtr entity(new RocketEnemyEntity(this, World()));
                    RocketEnemyEntity* enemy = static_cast<RocketEnemyEntity*>(entity.get());

                    enemy->BindResources(World()->ResourceContext());
                    enemy->StartTimeout(0.5f);
                    enemy->MinShootTimeout(((min_shoot_ms / 2) < 100) ? 100 : (min_shoot_ms / 2));
                    enemy->MaxShootTimeout(((max_shoot_ms / 2) < 100) ? 100 : (max_shoot_ms / 2));
                    enemy->Position(ChooseOffscreenPoint());
                    enemy->Points(points);
                    enemy->ShotSpeed(shot_speed);
                    enemy->MaxSpeed(ship_speed);
                    enemy->HitPoints(hit_points);
                    enemy->WayPoints(GenerateTopOnScreenWaypoints(num_waypoints*2));

                    World()->EnemyEntityList().push_back(entity);
                }
                break;
            }
        }

        // Remove this type from the list
        std::vector<EntityType>::iterator temp_itor = batch_types_.begin();
        std::advance(temp_itor, type_index);
        batch_types_.erase(temp_itor);
    }
}






///////////////////////////////////////////////////////////////////////////////
Math::Vector StateWave::ChooseOffscreenPoint() const
{
    switch(World()->GetWave()/5)
    {
    // Entities only fly out from the top (waves 0-4)
    case 0:
        return Math::Vector(100.0f+float(rand()%600), -100.0f, 0.0f);

    // Entities fly out from the top, left and right (waves 5-9)
    case 1:
        switch(rand()%3)
        {
        case 0:
            return Math::Vector(100.0f+float(rand()%600), -100.0f, 0.0f);
        case 1:
            return Math::Vector(800.0f, float(rand()%600), 0.0f);
        }
        return Math::Vector(0.0f, float(rand()%600), 0.0f);
    }

    // Entities fly out from all sides of the screen (waves 10-29)
    switch(rand()%4)
    {
    case 0:
        return Math::Vector(100.0f+float(rand()%600), -100.0f, 0.0f);
    case 1:
        return Math::Vector(800.0f, float(rand()%600), 0.0f);
    case 2:
        return Math::Vector(100.0f+float(rand()%600), 700.0f, 0.0f);
    }
    return Math::Vector(0.0f, float(rand()%600), 0.0f);
}

///////////////////////////////////////////////////////////////////////////////
Math::Vector StateWave::ChooseTopOffscreenPoint() const
{
    return Math::Vector(100.0f+float(rand()%600), -100.0f, 0.0f);
}

///////////////////////////////////////////////////////////////////////////////
Math::Vector StateWave::ChooseOnscreenPoint() const
{
    float x = float(rand()%300);
    float y = float(rand()%300);

    switch(rand()%4)
    {
    case 0:
        return Math::Vector(100.0f+x, y, 0.0f);
    case 1:
        return Math::Vector(x+400, y, 0.0f);
    case 2:
        return Math::Vector(100.0f+x, y+300, 0.0f);
    }
    return Math::Vector(x+400, y+300, 0.0f);
}

///////////////////////////////////////////////////////////////////////////////
Math::Vector StateWave::ChooseTopOnscreenPoint() const
{
    float x = float(rand()%300);
    float y = float(rand()%300);

    if(rand()%2 == 0)
    {
        return Math::Vector(100.0f+x, y, 0.0f);
    }
    return Math::Vector(x+400, y, 0.0f);
}

///////////////////////////////////////////////////////////////////////////////
Math::VectorList StateWave::GenerateOnScreenWaypoints(unsigned int num_waypoints) const
{
    Math::VectorList way_points;
    for(unsigned int i = 0; i < num_waypoints; i++)
    {
        way_points.push_back(ChooseOnscreenPoint());
    }
    way_points.push_back(ChooseOffscreenPoint());
    return way_points;
}

///////////////////////////////////////////////////////////////////////////////
Math::VectorList StateWave::GenerateTopOnScreenWaypoints(unsigned int num_waypoints) const
{
    Math::VectorList way_points;
    for(unsigned int i = 0; i < num_waypoints; i++)
    {
        way_points.push_back(ChooseTopOnscreenPoint());
    }
    way_points.push_back(ChooseTopOffscreenPoint());
    return way_points;
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::InsertEntities(float min_percent, float max_percent, unsigned int min_wave, unsigned int max_wave, EntityType type, unsigned int& remaining)
{
    // First, work out what percentage we're after
    float percent;
    if(World()->GetWave() < min_wave)
    {
        percent = min_percent;
    }
    else if(World()->GetWave() > max_wave)
    {
        percent = max_percent;
    }
    else
    {
        // Lerp it from the min to the max
        const float ratio = (max_percent - min_percent) / float(max_wave-min_wave);
        percent = min_percent + (ratio * float(World()->GetWave()-min_wave));
    }

    // Push the types into the list.
    unsigned int amount = unsigned int(percent * float(remaining));
    if(amount > 0)
    {
        std::vector<EntityType> temp(amount, type);
        std::copy(temp.begin(), temp.end(), std::back_inserter(batch_types_));
        remaining -= amount;
    }
}

///////////////////////////////////////////////////////////////////////////////
void StateWave::CreateBoss()
{
    AddTimeout(TIMER_HIDE_BOSS_INTRO, 3000);

    show_boss_intro_ = true;
    snd_alert_->Play2d();

    switch(World()->GetWave())
    {
    case 4:
        {
            Gfx::IEntityPtr entity(new MiniBoss0EnemyEntity(this, World()));
            MiniBoss0EnemyEntity* enemy = static_cast<MiniBoss0EnemyEntity*>(entity.get());

            enemy->BindResources(World()->ResourceContext());
            enemy->StartTimeout(2.0f);
            enemy->MinShootTimeout(1000);
            enemy->MaxShootTimeout(1500);
            enemy->Position(ChooseOffscreenPoint());
            enemy->Points(1000);
            enemy->ShotSpeed(325.0f);
            enemy->MaxSpeed(150.0f);
            enemy->HitPoints(500);
            enemy->WayPoints(GenerateTopOnScreenWaypoints(15));

            World()->EnemyEntityList().push_back(entity);
            img_boss_intro_ = img_miniboss_;
            break;
        }
    case 9:
        {
            Gfx::IEntityPtr entity(new Boss0EnemyEntity(this, World()));
            Boss0EnemyEntity* enemy = static_cast<Boss0EnemyEntity*>(entity.get());

            enemy->BindResources(World()->ResourceContext());
            enemy->StartTimeout(2.0f);
            enemy->MinShootTimeout(1000);
            enemy->MaxShootTimeout(1500);
            enemy->Position(ChooseOffscreenPoint());
            enemy->Points(1250);
            enemy->ShotSpeed(325.0f);
            enemy->MaxSpeed(150.0f);
            enemy->HitPoints(1000);
            enemy->WayPoints(GenerateTopOnScreenWaypoints(15));

            World()->EnemyEntityList().push_back(entity);
            img_boss_intro_ = img_boss_;
            break;
        }
    case 14:
        {
            Gfx::IEntityPtr entity(new MiniBoss1EnemyEntity(this, World()));
            MiniBoss1EnemyEntity* enemy = static_cast<MiniBoss1EnemyEntity*>(entity.get());

            enemy->BindResources(World()->ResourceContext());
            enemy->StartTimeout(2.0f);
            enemy->MinShootTimeout(2000);
            enemy->MaxShootTimeout(2500);
            enemy->Position(ChooseOffscreenPoint());
            enemy->Points(1500);
            enemy->ShotSpeed(400.0f);
            enemy->MaxSpeed(300.0f);
            enemy->HitPoints(850);
            enemy->WayPoints(GenerateTopOnScreenWaypoints(15));
            enemy->SitMinTimeout(6000);
            enemy->SitMaxTimeout(10000);
            enemy->NumShots(8);
            enemy->SpreadAngle(70);
            enemy->ShotTimeout(0.180f);

            World()->EnemyEntityList().push_back(entity);
            img_boss_intro_ = img_miniboss_;
            break;
        }
    case 19:
        {
            Gfx::IEntityPtr entity(new Boss1EnemyEntity(this, World()));
            Boss1EnemyEntity* enemy = static_cast<Boss1EnemyEntity*>(entity.get());

            enemy->BindResources(World()->ResourceContext());
            enemy->StartTimeout(2.0f);
            enemy->MinShootTimeout(1000);
            enemy->MaxShootTimeout(1500);
            enemy->Position(ChooseOffscreenPoint());
            enemy->Points(1750);
            enemy->ShotSpeed(325.0f);
            enemy->MaxSpeed(150.0f);
            enemy->HitPoints(1250);
            enemy->WayPoints(GenerateTopOnScreenWaypoints(15));

            World()->EnemyEntityList().push_back(entity);
            img_boss_intro_ = img_boss_;
            break;
        }
    case 24:
        {
            Gfx::IEntityPtr entity(new MiniBoss2EnemyEntity(this, World()));
            MiniBoss2EnemyEntity* enemy = static_cast<MiniBoss2EnemyEntity*>(entity.get());

            enemy->BindResources(World()->ResourceContext());
            enemy->StartTimeout(2.0f);
            enemy->MinShootTimeout(500);
            enemy->MaxShootTimeout(1000);
            enemy->Position(ChooseOffscreenPoint());
            enemy->Points(2000);
            enemy->MaxSpeed(250.0f);
            enemy->HitPoints(1000);
            enemy->WayPoints(GenerateTopOnScreenWaypoints(15));

            World()->EnemyEntityList().push_back(entity);
            img_boss_intro_ = img_miniboss_;
            break;
        }
    case 29:
        {
            Gfx::IEntityPtr entity(new Boss2EnemyEntity(this, World()));
            Boss2EnemyEntity* enemy = static_cast<Boss2EnemyEntity*>(entity.get());

            enemy->BindResources(World()->ResourceContext());
            enemy->StartTimeout(2.0f);
            enemy->MinShootTimeout(1000);
            enemy->MaxShootTimeout(1500);
            enemy->Position(ChooseOffscreenPoint());
            enemy->Points(2500);
            enemy->ShotSpeed(325.0f);
            enemy->MaxSpeed(150.0f);
            enemy->HitPoints(1500);
            enemy->WayPoints(GenerateTopOnScreenWaypoints(15));

            World()->EnemyEntityList().push_back(entity);
            img_boss_intro_ = img_boss_;
            break;
        }
    }
}
