#include "stdafx.h"
#include "maindialog.h"
#include "idialogcontrol.h"
#include <boost/bind/bind.hpp>

using namespace boost::placeholders;

MainDialog::MainDialog(Win::WindowManager* manager, IDialogControl* dialog_control, Util::ResourceContext& resources)
: Window(manager)
, dialog_control_(dialog_control)
{
    Moveable(true);
    Focusable(false);
    Modal(true);

    Gfx::ImageAnimPtr imgani = resources.FindImageAnim("Images/MainDialog.tga");
    imgani->DrawCentered(false);
    ClientImage(imgani);

    imgani = resources.FindImageAnim("Images/MainDialogShadow.tga");
    imgani->DrawCentered(false);
    ClientImageShadow(imgani);

    Position(Math::Vector(200.0f, 100.0f, 0.0f));



    imgani = resources.FindImageAnim("Images/GameButton.tga");
    imgani->DrawCentered(false);

    game_button_.reset(new Win::ButtonWindow(manager, this));
    game_button_->ClientImage(imgani);
    game_button_->Position(Math::Vector(44.0f, 300.0f, 0.0f));
    game_button_->OnButtonClick(boost::bind(&MainDialog::OnButtonClickGame, this));
    Children().push_back(game_button_);


    imgani = resources.FindImageAnim("Images/OptionsButton.tga");
    imgani->DrawCentered(false);

    options_button_.reset(new Win::ButtonWindow(manager, this));
    options_button_->ClientImage(imgani);
    options_button_->Position(Math::Vector(108.0f, 300.0f, 0.0f));
    options_button_->OnButtonClick(boost::bind(&MainDialog::OnButtonClickOptions, this));
    Children().push_back(options_button_);


    imgani = resources.FindImageAnim("Images/HighscoresButton.tga");
    imgani->DrawCentered(false);

    scores_button_.reset(new Win::ButtonWindow(manager, this));
    scores_button_->ClientImage(imgani);
    scores_button_->Position(Math::Vector(172.0f, 300.0f, 0.0f));
    scores_button_->OnButtonClick(boost::bind(&MainDialog::OnButtonClickScores, this));
    Children().push_back(scores_button_);


    imgani = resources.FindImageAnim("Images/HelpButton.tga");
    imgani->DrawCentered(false);

    help_button_.reset(new Win::ButtonWindow(manager, this));
    help_button_->ClientImage(imgani);
    help_button_->Position(Math::Vector(236.0f, 300.0f, 0.0f));
    help_button_->OnButtonClick(boost::bind(&MainDialog::OnButtonClickHelp, this));
    Children().push_back(help_button_);


    imgani = resources.FindImageAnim("Images/ExitButton.tga");
    imgani->DrawCentered(false);

    exit_button_.reset(new Win::ButtonWindow(manager, this));
    exit_button_->ClientImage(imgani);
    exit_button_->Position(Math::Vector(300.0f, 300.0f, 0.0f));
    exit_button_->OnButtonClick(boost::bind(&MainDialog::OnButtonClickExit, this));
    Children().push_back(exit_button_);
}

void MainDialog::OnButtonClickGame()
{
    dialog_control_->StartNewGame();
}

void MainDialog::OnButtonClickOptions()
{
    dialog_control_->ShowOptionsDialog();
}

void MainDialog::OnButtonClickScores()
{
    dialog_control_->ShowScoresDialog();
}

void MainDialog::OnButtonClickHelp()
{
    dialog_control_->ShowHelpDialog();
}

void MainDialog::OnButtonClickExit()
{
    dialog_control_->ExitApplication();
}
