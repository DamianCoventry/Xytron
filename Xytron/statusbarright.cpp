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
#include "statusbarright.h"
#include "iworld.h"
#include "playerentity.h"
#include "pickupentity.h"

///////////////////////////////////////////////////////////////////////////////
StatusBarRight::StatusBarRight(IWorld* world)
: frame_time_(0.0f)
, current_frame_(0)
, world_(world)
{
}

///////////////////////////////////////////////////////////////////////////////
void StatusBarRight::BindResources(Util::ResourceContext* resources)
{
    img_background_ = resources->FindImage("Images/StatusBarRight.tga");
    img_background_->DrawCentered(false);

    imgani_cannongun_   = resources->FindImageAnim("Images/AnimIconCannonGun.png");
    imgani_lasergun_    = resources->FindImageAnim("Images/AnimIconLaserGun.png");
    imgani_railgun_     = resources->FindImageAnim("Images/AnimIconRailGun.png");
    imgani_plasmagun_   = resources->FindImageAnim("Images/AnimIconPlasmaGun.png");
    imgani_missilesgun_ = resources->FindImageAnim("Images/AnimIconMissilesGun.png");

    imgani_funkybomb_   = resources->FindImageAnim("Images/AnimIconFunkyBomb.png");
    imgani_nukebomb_    = resources->FindImageAnim("Images/AnimIconNukeBomb.png");
    imgani_scatterbomb_ = resources->FindImageAnim("Images/AnimIconScatterBomb.png");
    imgani_empbomb_     = resources->FindImageAnim("Images/AnimIconEmpBomb.png");

    imgani_staticbombicons_ = resources->FindImageAnim("Images/StaticBombIcons.png");

    img_largeglasshighlight_ = resources->FindImage("Images/GlassHighlight.tga");
    img_smallglasshighlight_ = resources->FindImage("Images/SmallGlassHighlight.tga");

    img_funkybomboverlay_   = resources->FindImage("Images/FunkyBombOverlay.tga");
    img_empbomboverlay_     = resources->FindImage("Images/EmpBombOverlay.tga");
    img_nukebomboverlay_    = resources->FindImage("Images/NukeBombOverlay.tga");
    img_scatterbomboverlay_ = resources->FindImage("Images/ScatterBombOverlay.tga");

    img_cannongunoverlay_[0]    = resources->FindImage("Images/CannonGunOverlay0.tga");
    img_cannongunoverlay_[1]    = resources->FindImage("Images/CannonGunOverlay1.tga");
    img_cannongunoverlay_[2]    = resources->FindImage("Images/CannonGunOverlay2.tga");
    img_cannongunoverlay_[3]    = resources->FindImage("Images/CannonGunOverlay3.tga");

    img_lasergunoverlay_[0]     = resources->FindImage("Images/LaserGunOverlay0.tga");
    img_lasergunoverlay_[1]     = resources->FindImage("Images/LaserGunOverlay1.tga");
    img_lasergunoverlay_[2]     = resources->FindImage("Images/LaserGunOverlay2.tga");
    img_lasergunoverlay_[3]     = resources->FindImage("Images/LaserGunOverlay3.tga");

    img_missilesgunoverlay_[0]  = resources->FindImage("Images/MissilesGunOverlay0.tga");
    img_missilesgunoverlay_[1]  = resources->FindImage("Images/MissilesGunOverlay1.tga");
    img_missilesgunoverlay_[2]  = resources->FindImage("Images/MissilesGunOverlay2.tga");
    img_missilesgunoverlay_[3]  = resources->FindImage("Images/MissilesGunOverlay3.tga");

    img_railgunoverlay_[0]      = resources->FindImage("Images/RailGunOverlay0.tga");
    img_railgunoverlay_[1]      = resources->FindImage("Images/RailGunOverlay1.tga");
    img_railgunoverlay_[2]      = resources->FindImage("Images/RailGunOverlay2.tga");
    img_railgunoverlay_[3]      = resources->FindImage("Images/RailGunOverlay3.tga");

    img_plasmagunoverlay_[0]    = resources->FindImage("Images/PlasmaGunOverlay0.tga");
    img_plasmagunoverlay_[1]    = resources->FindImage("Images/PlasmaGunOverlay1.tga");
    img_plasmagunoverlay_[2]    = resources->FindImage("Images/PlasmaGunOverlay2.tga");
    img_plasmagunoverlay_[3]    = resources->FindImage("Images/PlasmaGunOverlay3.tga");

    imgani_cannongun_->DrawCentered(false);
    imgani_lasergun_->DrawCentered(false);
    imgani_railgun_->DrawCentered(false);
    imgani_plasmagun_->DrawCentered(false);
    imgani_missilesgun_->DrawCentered(false);

    imgani_funkybomb_->DrawCentered(false);
    imgani_empbomb_->DrawCentered(false);
    imgani_scatterbomb_->DrawCentered(false);
    imgani_nukebomb_->DrawCentered(false);

    imgani_staticbombicons_->DrawCentered(false);
    img_largeglasshighlight_->DrawCentered(false);
    img_smallglasshighlight_->DrawCentered(false);

    img_funkybomboverlay_->DrawCentered(false);
    img_empbomboverlay_->DrawCentered(false);
    img_nukebomboverlay_->DrawCentered(false);
    img_scatterbomboverlay_->DrawCentered(false);

    img_cannongunoverlay_[0]->DrawCentered(false);
    img_cannongunoverlay_[1]->DrawCentered(false);
    img_cannongunoverlay_[2]->DrawCentered(false);
    img_cannongunoverlay_[3]->DrawCentered(false);

    img_lasergunoverlay_[0]->DrawCentered(false);
    img_lasergunoverlay_[1]->DrawCentered(false);
    img_lasergunoverlay_[2]->DrawCentered(false);
    img_lasergunoverlay_[3]->DrawCentered(false);

    img_missilesgunoverlay_[0]->DrawCentered(false);
    img_missilesgunoverlay_[1]->DrawCentered(false);
    img_missilesgunoverlay_[2]->DrawCentered(false);
    img_missilesgunoverlay_[3]->DrawCentered(false);

    img_railgunoverlay_[0]->DrawCentered(false);
    img_railgunoverlay_[1]->DrawCentered(false);
    img_railgunoverlay_[2]->DrawCentered(false);
    img_railgunoverlay_[3]->DrawCentered(false);

    img_plasmagunoverlay_[0]->DrawCentered(false);
    img_plasmagunoverlay_[1]->DrawCentered(false);
    img_plasmagunoverlay_[2]->DrawCentered(false);
    img_plasmagunoverlay_[3]->DrawCentered(false);
}

///////////////////////////////////////////////////////////////////////////////
void StatusBarRight::Think(float time_delta)
{
    if(world_ && world_->Player() && world_->Player()->PrimaryWeapon())
    {
        unsigned int upgrade = world_->Player()->PrimaryWeapon()->GetUpgradeLevel();
        switch(world_->Player()->PrimaryWeapon()->Type())
        {
        case PickupEntity::PICKUP_GUN_CANNON:
            imgani_gun_         = imgani_cannongun_;
            img_gunoverlay_     = img_cannongunoverlay_[upgrade];
            break;
        case PickupEntity::PICKUP_GUN_LASER:
            imgani_gun_         = imgani_lasergun_;
            img_gunoverlay_     = img_lasergunoverlay_[upgrade];
            break;
        case PickupEntity::PICKUP_GUN_MISSILES:
            imgani_gun_         = imgani_missilesgun_;
            img_gunoverlay_     = img_missilesgunoverlay_[upgrade];
            break;
        case PickupEntity::PICKUP_GUN_PLASMA:
            imgani_gun_         = imgani_plasmagun_;
            img_gunoverlay_     = img_plasmagunoverlay_[upgrade];
            break;
        case PickupEntity::PICKUP_GUN_RAIL:
            imgani_gun_         = imgani_railgun_;
            img_gunoverlay_     = img_railgunoverlay_[upgrade];
            break;
        }

        if(!world_->Player()->BombList().empty())
        {
            switch(world_->Player()->BombList().front()->Type())
            {
            case PickupEntity::PICKUP_BOMB_NUKE:
                imgani_bomb_        = imgani_nukebomb_;
                img_bomboverlay_    = img_nukebomboverlay_;
                break;
            case PickupEntity::PICKUP_BOMB_SCATTER:
                imgani_bomb_        = imgani_scatterbomb_;
                img_bomboverlay_    = img_scatterbomboverlay_;
                break;
            case PickupEntity::PICKUP_BOMB_FUNKY:
                imgani_bomb_        = imgani_funkybomb_;
                img_bomboverlay_    = img_funkybomboverlay_;
                break;
            case PickupEntity::PICKUP_BOMB_EMP:
                imgani_bomb_        = imgani_empbomb_;
                img_bomboverlay_    = img_empbomboverlay_;
                break;
            }
        }
    }

    if(imgani_gun_)
    {
        frame_time_ += time_delta;
        if(frame_time_ >= 0.1f)
        {
            frame_time_ = 0.0f;
            if(++current_frame_ >= imgani_gun_->ImageCount())
            {
                current_frame_ = 0;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void StatusBarRight::Draw2d(const Gfx::Graphics& g)
{
    img_background_->Draw2d(g, Math::Vector(680.0f, 0.0f, 0.0f));

    if(imgani_gun_)
    {
        imgani_gun_->CurrentFrame(current_frame_);
        imgani_gun_->Draw2d(g, Math::Vector(713.0f, 21.0f, 0.0f));
        img_gunoverlay_->Draw2d(g, Math::Vector(693.0f, 4.0f, 0.0f));
    }

    float y;
    const WeaponList& bomb_list = world_->Player()->BombList();
    if(!bomb_list.empty())
    {
        if(imgani_bomb_)
        {
            imgani_bomb_->CurrentFrame(current_frame_);
            imgani_bomb_->Draw2d(g, Math::Vector(713.0f, 137.0f, 0.0f));
            img_bomboverlay_->Draw2d(g, Math::Vector(693.0f, 120.0f, 0.0f));
        }

        WeaponList::const_iterator bomb = bomb_list.begin();
        ++bomb;

        y = 260.0f;
        while(bomb != bomb_list.end())
        {
            switch((*bomb)->Type())
            {
            case PickupEntity::PICKUP_BOMB_SCATTER: imgani_staticbombicons_->CurrentFrame(0); break;
            case PickupEntity::PICKUP_BOMB_EMP:     imgani_staticbombicons_->CurrentFrame(1); break;
            case PickupEntity::PICKUP_BOMB_FUNKY:   imgani_staticbombicons_->CurrentFrame(2); break;
            case PickupEntity::PICKUP_BOMB_NUKE:    imgani_staticbombicons_->CurrentFrame(3); break;
            }
            imgani_staticbombicons_->Draw2d(g, Math::Vector(750.0f, y, 0.0f));

            ++bomb;
            y += 63.0f;
        }
    }

    // Draw each of the glass highlights.
    img_largeglasshighlight_->Draw2d(g, Math::Vector(693.0f, 8.0f, 0.0f));
    img_largeglasshighlight_->Draw2d(g, Math::Vector(693.0f, 122.0f, 0.0f));

    y = 248.0f;
    for(int i = 0; i < 5; i++)
    {
        img_smallglasshighlight_->Draw2d(g, Math::Vector(740.0f, y, 0.0f));
        y += 63.0f;
    }

}
