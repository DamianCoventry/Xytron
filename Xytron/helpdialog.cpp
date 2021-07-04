#include "stdafx.h"
#include "helpdialog.h"
#include "idialogcontrol.h"
#include <boost/bind/bind.hpp>

using namespace boost::placeholders;

const unsigned int HelpDialog::PAGE_OVERVIEW        = 0;
const unsigned int HelpDialog::PAGE_PLAYER_SHIP     = 1;
const unsigned int HelpDialog::PAGE_WEAPON_SYSTEM   = 2;
const unsigned int HelpDialog::PAGE_CANNON_GUN      = 3;
const unsigned int HelpDialog::PAGE_LASER_GUN       = 4;
const unsigned int HelpDialog::PAGE_MISSILES        = 5;
const unsigned int HelpDialog::PAGE_PLASMA_GUN      = 6;
const unsigned int HelpDialog::PAGE_RAIL_GUN        = 7;
const unsigned int HelpDialog::PAGE_BOMB_SYSTEM     = 8;
const unsigned int HelpDialog::PAGE_FUNKY_BOMB      = 9;
const unsigned int HelpDialog::PAGE_EMP_BOMB        = 10;
const unsigned int HelpDialog::PAGE_NUKE_BOMB       = 11;
const unsigned int HelpDialog::PAGE_SCATTER_BOMB    = 12;
const unsigned int HelpDialog::PAGE_ITEM_SYSTEM     = 13;


HelpDialog::HelpDialog(Win::WindowManager* manager, IDialogControl* dialog_control, Util::ResourceContext& resources)
: Window(manager)
, dialog_control_(dialog_control)
, current_page_(PAGE_OVERVIEW)
{
    Moveable(true);
    Focusable(false);
    Modal(true);

    Gfx::ImageAnimPtr imgani = resources.FindImageAnim("Images/HelpDialog.tga");
    imgani->DrawCentered(false);
    ClientImage(imgani);

    imgani = resources.FindImageAnim("Images/HelpDialogShadow.tga");
    imgani->DrawCentered(false);
    ClientImageShadow(imgani);

    Position(Math::Vector(150.0f, 100.0f, 0.0f));



    imgani = resources.FindImageAnim("Images/PrevButton.tga");
    imgani->DrawCentered(false);

    prev_button_.reset(new Win::ButtonWindow(manager, this));
    prev_button_->ClientImage(imgani);
    prev_button_->Position(Math::Vector(154.0f, 300.0f, 0.0f));
    prev_button_->OnButtonClick(boost::bind(&HelpDialog::OnButtonClickPrev, this));
    Children().push_back(prev_button_);


    imgani = resources.FindImageAnim("Images/NextButton.tga");
    imgani->DrawCentered(false);

    next_button_.reset(new Win::ButtonWindow(manager, this));
    next_button_->ClientImage(imgani);
    next_button_->Position(Math::Vector(218.0f, 300.0f, 0.0f));
    next_button_->OnButtonClick(boost::bind(&HelpDialog::OnButtonClickNext, this));
    Children().push_back(next_button_);


    imgani = resources.FindImageAnim("Images/CloseButton.tga");
    imgani->DrawCentered(false);

    close_button_.reset(new Win::ButtonWindow(manager, this));
    close_button_->ClientImage(imgani);
    close_button_->Position(Math::Vector(282.0f, 300.0f, 0.0f));
    close_button_->OnButtonClick(boost::bind(&HelpDialog::OnButtonClickClose, this));
    Children().push_back(close_button_);


    imgani = resources.FindImageAnim("Images/OverviewText.tga");
    imgani->DrawCentered(false);

    overview_text_.reset(new Win::Window(manager, this));
    overview_text_->ClientImage(imgani);
    overview_text_->Position(Math::Vector(55.0f, 80.0f, 0.0f));
    Children().push_back(overview_text_);


    imgani = resources.FindImageAnim("Images/PlayerShipText.tga");
    imgani->DrawCentered(false);

    player_ship_text_.reset(new Win::Window(manager, this));
    player_ship_text_->ClientImage(imgani);
    player_ship_text_->Position(Math::Vector(55.0f, 80.0f, 0.0f));
    Children().push_back(player_ship_text_);


    imgani = resources.FindImageAnim("Images/WeaponSystemText.tga");
    imgani->DrawCentered(false);

    weapon_system_text_.reset(new Win::Window(manager, this));
    weapon_system_text_->ClientImage(imgani);
    weapon_system_text_->Position(Math::Vector(55.0f, 80.0f, 0.0f));
    Children().push_back(weapon_system_text_);


    imgani = resources.FindImageAnim("Images/CannonGunText.tga");
    imgani->DrawCentered(false);

    cannon_gun_text_.reset(new Win::Window(manager, this));
    cannon_gun_text_->ClientImage(imgani);
    cannon_gun_text_->Position(Math::Vector(55.0f, 80.0f, 0.0f));
    Children().push_back(cannon_gun_text_);


    imgani = resources.FindImageAnim("Images/LaserGunText.tga");
    imgani->DrawCentered(false);

    laser_gun_text_.reset(new Win::Window(manager, this));
    laser_gun_text_->ClientImage(imgani);
    laser_gun_text_->Position(Math::Vector(55.0f, 80.0f, 0.0f));
    Children().push_back(laser_gun_text_);


    imgani = resources.FindImageAnim("Images/MissilesText.tga");
    imgani->DrawCentered(false);

    missiles_text_.reset(new Win::Window(manager, this));
    missiles_text_->ClientImage(imgani);
    missiles_text_->Position(Math::Vector(55.0f, 80.0f, 0.0f));
    Children().push_back(missiles_text_);


    imgani = resources.FindImageAnim("Images/PlasmaGunText.tga");
    imgani->DrawCentered(false);

    plasma_gun_text_.reset(new Win::Window(manager, this));
    plasma_gun_text_->ClientImage(imgani);
    plasma_gun_text_->Position(Math::Vector(55.0f, 80.0f, 0.0f));
    Children().push_back(plasma_gun_text_);


    imgani = resources.FindImageAnim("Images/RailGunText.tga");
    imgani->DrawCentered(false);

    rail_gun_text_.reset(new Win::Window(manager, this));
    rail_gun_text_->ClientImage(imgani);
    rail_gun_text_->Position(Math::Vector(55.0f, 80.0f, 0.0f));
    Children().push_back(rail_gun_text_);


    imgani = resources.FindImageAnim("Images/BombSystemText.tga");
    imgani->DrawCentered(false);

    bomb_system_text_.reset(new Win::Window(manager, this));
    bomb_system_text_->ClientImage(imgani);
    bomb_system_text_->Position(Math::Vector(55.0f, 80.0f, 0.0f));
    Children().push_back(bomb_system_text_);


    imgani = resources.FindImageAnim("Images/FunkyBombText.tga");
    imgani->DrawCentered(false);

    funky_bomb_text_.reset(new Win::Window(manager, this));
    funky_bomb_text_->ClientImage(imgani);
    funky_bomb_text_->Position(Math::Vector(55.0f, 80.0f, 0.0f));
    Children().push_back(funky_bomb_text_);


    imgani = resources.FindImageAnim("Images/EmpBombText.tga");
    imgani->DrawCentered(false);

    emp_bomb_text_.reset(new Win::Window(manager, this));
    emp_bomb_text_->ClientImage(imgani);
    emp_bomb_text_->Position(Math::Vector(55.0f, 80.0f, 0.0f));
    Children().push_back(emp_bomb_text_);


    imgani = resources.FindImageAnim("Images/NukeBombText.tga");
    imgani->DrawCentered(false);

    nuke_bomb_text_.reset(new Win::Window(manager, this));
    nuke_bomb_text_->ClientImage(imgani);
    nuke_bomb_text_->Position(Math::Vector(55.0f, 80.0f, 0.0f));
    Children().push_back(nuke_bomb_text_);


    imgani = resources.FindImageAnim("Images/ScatterBombText.tga");
    imgani->DrawCentered(false);

    scatter_bomb_text_.reset(new Win::Window(manager, this));
    scatter_bomb_text_->ClientImage(imgani);
    scatter_bomb_text_->Position(Math::Vector(55.0f, 80.0f, 0.0f));
    Children().push_back(scatter_bomb_text_);


    imgani = resources.FindImageAnim("Images/ItemSystemText.tga");
    imgani->DrawCentered(false);

    item_system_text_.reset(new Win::Window(manager, this));
    item_system_text_->ClientImage(imgani);
    item_system_text_->Position(Math::Vector(55.0f, 80.0f, 0.0f));
    Children().push_back(item_system_text_);

    HideAllWindows();
    overview_text_->Visible(true);
}

void HelpDialog::OnButtonClickPrev()
{
    HideAllWindows();

    if(current_page_ == PAGE_OVERVIEW)
    {
        current_page_ = PAGE_ITEM_SYSTEM;
    }
    else
    {
        current_page_--;
    }

    switch(current_page_)
    {
    case HelpDialog::PAGE_OVERVIEW: overview_text_->Visible(true); break;
    case HelpDialog::PAGE_PLAYER_SHIP: player_ship_text_->Visible(true); break;
    case HelpDialog::PAGE_WEAPON_SYSTEM: weapon_system_text_->Visible(true); break;
    case HelpDialog::PAGE_CANNON_GUN: cannon_gun_text_->Visible(true); break;
    case HelpDialog::PAGE_LASER_GUN: laser_gun_text_->Visible(true); break;
    case HelpDialog::PAGE_MISSILES: missiles_text_->Visible(true); break;
    case HelpDialog::PAGE_PLASMA_GUN: plasma_gun_text_->Visible(true); break;
    case HelpDialog::PAGE_RAIL_GUN: rail_gun_text_->Visible(true); break;
    case HelpDialog::PAGE_BOMB_SYSTEM: bomb_system_text_->Visible(true); break;
    case HelpDialog::PAGE_FUNKY_BOMB: funky_bomb_text_->Visible(true); break;
    case HelpDialog::PAGE_EMP_BOMB: emp_bomb_text_->Visible(true); break;
    case HelpDialog::PAGE_NUKE_BOMB: nuke_bomb_text_->Visible(true); break;
    case HelpDialog::PAGE_SCATTER_BOMB: scatter_bomb_text_->Visible(true); break;
    case HelpDialog::PAGE_ITEM_SYSTEM: item_system_text_->Visible(true); break;
    }
}

void HelpDialog::OnButtonClickNext()
{
    HideAllWindows();

    if(current_page_ == PAGE_ITEM_SYSTEM)
    {
        current_page_ = PAGE_OVERVIEW;
    }
    else
    {
        current_page_++;
    }

    switch(current_page_)
    {
    case HelpDialog::PAGE_OVERVIEW: overview_text_->Visible(true); break;
    case HelpDialog::PAGE_PLAYER_SHIP: player_ship_text_->Visible(true); break;
    case HelpDialog::PAGE_WEAPON_SYSTEM: weapon_system_text_->Visible(true); break;
    case HelpDialog::PAGE_CANNON_GUN: cannon_gun_text_->Visible(true); break;
    case HelpDialog::PAGE_LASER_GUN: laser_gun_text_->Visible(true); break;
    case HelpDialog::PAGE_MISSILES: missiles_text_->Visible(true); break;
    case HelpDialog::PAGE_PLASMA_GUN: plasma_gun_text_->Visible(true); break;
    case HelpDialog::PAGE_RAIL_GUN: rail_gun_text_->Visible(true); break;
    case HelpDialog::PAGE_BOMB_SYSTEM: bomb_system_text_->Visible(true); break;
    case HelpDialog::PAGE_FUNKY_BOMB: funky_bomb_text_->Visible(true); break;
    case HelpDialog::PAGE_EMP_BOMB: emp_bomb_text_->Visible(true); break;
    case HelpDialog::PAGE_NUKE_BOMB: nuke_bomb_text_->Visible(true); break;
    case HelpDialog::PAGE_SCATTER_BOMB: scatter_bomb_text_->Visible(true); break;
    case HelpDialog::PAGE_ITEM_SYSTEM: item_system_text_->Visible(true); break;
    }
}

void HelpDialog::OnButtonClickClose()
{
    Close();
}

void HelpDialog::HideAllWindows()
{
    overview_text_->Visible(false);
    player_ship_text_->Visible(false);
    weapon_system_text_->Visible(false);
    cannon_gun_text_->Visible(false);
    laser_gun_text_->Visible(false);
    missiles_text_->Visible(false);
    plasma_gun_text_->Visible(false);
    rail_gun_text_->Visible(false);
    bomb_system_text_->Visible(false);
    funky_bomb_text_->Visible(false);
    emp_bomb_text_->Visible(false);
    nuke_bomb_text_->Visible(false);
    scatter_bomb_text_->Visible(false);
    item_system_text_->Visible(false);
}
