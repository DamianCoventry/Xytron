#include "stdafx.h"
#include "ingamedialog.h"
#include "idialogcontrol.h"
#include <boost/bind/bind.hpp>

using namespace boost::placeholders;

InGameDialog::InGameDialog(Win::WindowManager* manager, IDialogControl* dialog_control, Util::ResourceContext& resources)
: Window(manager)
, dialog_control_(dialog_control)
{
    Moveable(true);
    Focusable(false);
    Modal(true);

    Gfx::ImageAnimPtr imgani = resources.FindImageAnim("Images/InGameDialog.tga");
    imgani->DrawCentered(false);
    ClientImage(imgani);

    imgani = resources.FindImageAnim("Images/InGameDialogShadow.tga");
    imgani->DrawCentered(false);
    ClientImageShadow(imgani);

    Position(Math::Vector(250.0f, 100.0f, 0.0f));



    imgani = resources.FindImageAnim("Images/ResumeButton.tga");
    imgani->DrawCentered(false);

    resume_button_.reset(new Win::ButtonWindow(manager, this));
    resume_button_->ClientImage(imgani);
    resume_button_->Position(Math::Vector(22.0f, 300.0f, 0.0f));
    resume_button_->OnButtonClick(boost::bind(&InGameDialog::OnButtonClickResume, this));
    Children().push_back(resume_button_);


    imgani = resources.FindImageAnim("Images/OptionsButton.tga");
    imgani->DrawCentered(false);

    options_button_.reset(new Win::ButtonWindow(manager, this));
    options_button_->ClientImage(imgani);
    options_button_->Position(Math::Vector(86.0f, 300.0f, 0.0f));
    options_button_->OnButtonClick(boost::bind(&InGameDialog::OnButtonClickOptions, this));
    Children().push_back(options_button_);


    imgani = resources.FindImageAnim("Images/StopButton.tga");
    imgani->DrawCentered(false);

    stop_button.reset(new Win::ButtonWindow(manager, this));
    stop_button->ClientImage(imgani);
    stop_button->Position(Math::Vector(150.0f, 300.0f, 0.0f));
    stop_button->OnButtonClick(boost::bind(&InGameDialog::OnButtonClickEnd, this));
    Children().push_back(stop_button);


    imgani = resources.FindImageAnim("Images/ExitButton.tga");
    imgani->DrawCentered(false);

    exit_button_.reset(new Win::ButtonWindow(manager, this));
    exit_button_->ClientImage(imgani);
    exit_button_->Position(Math::Vector(214.0f, 300.0f, 0.0f));
    exit_button_->OnButtonClick(boost::bind(&InGameDialog::OnButtonClickExit, this));
    Children().push_back(exit_button_);
}

void InGameDialog::OnButtonClickResume()
{
    dialog_control_->ResumeCurrentGame();
}

void InGameDialog::OnButtonClickOptions()
{
    dialog_control_->ShowOptionsDialog();
}

void InGameDialog::OnButtonClickEnd()
{
    dialog_control_->EndCurrentGame();
}

void InGameDialog::OnButtonClickExit()
{
    dialog_control_->ExitApplication();
}
