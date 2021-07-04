#include "stdafx.h"
#include "highscoresdialog.h"
#include "idialogcontrol.h"
#include "highscores.h"
#include <boost/bind/bind.hpp>
#include <sstream>

using namespace boost::placeholders;

HighscoresDialog::HighscoresDialog(Win::WindowManager* manager, IDialogControl* dialog_control, Util::ResourceContext& resources, Highscores& highscores)
: Window(manager)
, dialog_control_(dialog_control)
, highscores_(highscores)
, level_(0)
, score_(0)
{
    Moveable(true);
    Focusable(false);
    Modal(true);

    snd_newhighscore_ = resources.FindSound("Sounds/GameWon.wav");

    Gfx::ImageAnimPtr imgani = resources.FindImageAnim("Images/HighscoresDialog.tga");
    imgani->DrawCentered(false);
    ClientImage(imgani);

    imgani = resources.FindImageAnim("Images/HighscoresDialogShadow.tga");
    imgani->DrawCentered(false);
    ClientImageShadow(imgani);

    Position(Math::Vector(150.0f, 50.0f, 0.0f));



    imgani = resources.FindImageAnim("Images/ResetButton.tga");
    imgani->DrawCentered(false);

    reset_button_.reset(new Win::ButtonWindow(manager, this));
    reset_button_->ClientImage(imgani);
    reset_button_->Position(Math::Vector(186.0f, 400.0f, 0.0f));
    reset_button_->OnButtonClick(boost::bind(&HighscoresDialog::OnButtonClickReset, this));
    Children().push_back(reset_button_);


    imgani = resources.FindImageAnim("Images/CloseButton.tga");
    imgani->DrawCentered(false);

    close_button_.reset(new Win::ButtonWindow(manager, this));
    close_button_->ClientImage(imgani);
    close_button_->Position(Math::Vector(250.0f, 400.0f, 0.0f));
    close_button_->OnButtonClick(boost::bind(&HighscoresDialog::OnButtonClickClose, this));
    Children().push_back(close_button_);


    imgani = resources.FindImageAnim("Images/HighscoresListView.tga");
    imgani->DrawCentered(false);

    highscores_listview_.reset(new Win::ListViewWindow(manager, this));
    highscores_listview_->ClientImage(imgani);
    highscores_listview_->Position(Math::Vector(55.0f, 80.0f, 0.0f));
    highscores_listview_->SetHeaderFont(resources.FindFont("Fonts/Trebuchet MS12Regular.font"));
    highscores_listview_->SetRowFont(resources.FindFont("Fonts/Trebuchet MS12Regular.font"));
    highscores_listview_->SetRowDefaultColor(Gfx::Color(1.0f, 1.0f, 1.0f));
    highscores_listview_->SetTopMargin(8.0f);
    highscores_listview_->SetLeftMargin(10.0f);
    highscores_listview_->AddColumn("Name", 125.0f);
    highscores_listview_->AddColumn("Date/Time", 150.0f);
    highscores_listview_->AddColumn("Level", 50.0f);
    highscores_listview_->AddColumn("Score", 75.0f);
    Children().push_back(highscores_listview_);


    imgani = resources.FindImageAnim("Images/EnterNameInstructions.tga");
    imgani->DrawCentered(false);

    instructions_image_.reset(new Win::Window(manager, this));
    instructions_image_->ClientImage(imgani);
    instructions_image_->Position(Math::Vector(55.0f, 385.0f, 0.0f));
    Children().push_back(instructions_image_);


    imgani = resources.FindImageAnim("Images/NameEditBox.tga");
    imgani->DrawCentered(false);

    name_editbox_.reset(new Win::EditBoxWindow(manager, this));
    name_editbox_->ClientImage(imgani);
    name_editbox_->Position(Math::Vector(55.0f, 420.0f, 0.0f));
    name_editbox_->Font(resources.FindFont("Fonts/Trebuchet MS12Regular.font"));
    name_editbox_->OnEditBoxReturnPressed(boost::bind(&HighscoresDialog::OnButtonClickOK, this));
    Children().push_back(name_editbox_);


    imgani = resources.FindImageAnim("Images/OkButton.tga");
    imgani->DrawCentered(false);

    ok_button_.reset(new Win::ButtonWindow(manager, this));
    ok_button_->ClientImage(imgani);
    ok_button_->Position(Math::Vector(326.0f, 400.0f, 0.0f));
    ok_button_->OnButtonClick(boost::bind(&HighscoresDialog::OnButtonClickOK, this));
    Children().push_back(ok_button_);


    imgani = resources.FindImageAnim("Images/CancelButton.tga");
    imgani->DrawCentered(false);

    cancel_button_.reset(new Win::ButtonWindow(manager, this));
    cancel_button_->ClientImage(imgani);
    cancel_button_->Position(Math::Vector(390.0f, 400.0f, 0.0f));
    cancel_button_->OnButtonClick(boost::bind(&HighscoresDialog::OnButtonClickCancel, this));
    Children().push_back(cancel_button_);

    EnterNewHighscore(false);
}

void HighscoresDialog::OnWindowActivated()
{
    RefreshListView();
    name_editbox_->Text().clear();

    if(instructions_image_->Visible())
    {
        snd_newhighscore_->Play2d();
    }
}

void HighscoresDialog::OnButtonClickReset()
{
    highscores_.Reset();
    RefreshListView();
}

void HighscoresDialog::OnButtonClickClose()
{
    Close();
}

void HighscoresDialog::OnButtonClickOK()
{
    if(!name_editbox_->Text().empty())
    {
        int index = highscores_.Insert(name_editbox_->Text(), level_+1, score_);
        if(index >= 0)
        {
            RefreshListView();
            highscores_listview_->SetRowTextColor(index, Gfx::Color(0.0f, 0.0f, 1.0f));
        }

        EnterNewHighscore(false);
    }
}

void HighscoresDialog::OnButtonClickCancel()
{
    EnterNewHighscore(false);
}

void HighscoresDialog::EnterNewHighscore(bool enter_new_highscore, unsigned int level, unsigned long score)
{
    level_ = level;
    score_ = score;

    if(enter_new_highscore)
    {
        reset_button_->Visible(false);
        close_button_->Visible(false);
        instructions_image_->Visible(true);
        name_editbox_->Visible(true);
        ok_button_->Visible(true);
        cancel_button_->Visible(true);
        name_editbox_->Focus();
    }
    else
    {
        reset_button_->Visible(true);
        close_button_->Visible(true);
        instructions_image_->Visible(false);
        name_editbox_->Visible(false);
        ok_button_->Visible(false);
        cancel_button_->Visible(false);
    }
}

void HighscoresDialog::RefreshListView()
{
    highscores_listview_->ClearItems();

    Highscores::Iterator itor;
    for(itor = highscores_.Begin(); itor != highscores_.End(); ++itor)
    {
        int index = highscores_listview_->AddItem(itor->Name());
        highscores_listview_->SetSubItemText(index, 1, itor->DateTime());

        std::ostringstream oss;
        oss << itor->Level();
        highscores_listview_->SetSubItemText(index, 2, oss.str());

        oss.str("");
        oss << itor->Score();
        highscores_listview_->SetSubItemText(index, 3, oss.str());
    }
}
