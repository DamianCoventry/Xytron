#include "stdafx.h"
#include "optionsdialog.h"
#include "idialogcontrol.h"
#include "settings.h"
#include <boost/bind/bind.hpp>

using namespace boost::placeholders;

OptionsDialog::OptionsDialog(Win::WindowManager* manager, IDialogControl* dialog_control, Util::ResourceContext& resources, Settings& settings)
: Window(manager)
, dialog_control_(dialog_control)
, settings_(settings)
, fullscreen_changed_(false)
{
    Moveable(true);
    Focusable(false);
    Modal(true);

    snd_test_ = resources.FindSound("Sounds/Test.wav");

    Gfx::ImageAnimPtr imgani = resources.FindImageAnim("Images/OptionsDialog.tga");
    imgani->DrawCentered(false);
    ClientImage(imgani);

    imgani = resources.FindImageAnim("Images/OptionsDialogShadow.tga");
    imgani->DrawCentered(false);
    ClientImageShadow(imgani);

    Position(Math::Vector(200.0f, 100.0f, 0.0f));


    imgani = resources.FindImageAnim("Images/CheckboxFullscreenGraphics.tga");
    imgani->DrawCentered(false);

    fullscreen_gfx_checkbox_.reset(new Win::CheckBoxWindow(manager, this));
    fullscreen_gfx_checkbox_->ClientImage(imgani);
    fullscreen_gfx_checkbox_->Position(Math::Vector(40.0f, 90.0f, 0.0f));
    fullscreen_gfx_checkbox_->Checked(settings_.FullscreenGraphics());
    fullscreen_gfx_checkbox_->OnCheckBoxClick(boost::bind(&OptionsDialog::OnCheckBoxClickFullscreenGraphics, this));
    Children().push_back(fullscreen_gfx_checkbox_);


    imgani = resources.FindImageAnim("Images/ApplyButton.tga");
    imgani->DrawCentered(false);

    apply_button_.reset(new Win::ButtonWindow(manager, this));
    apply_button_->ClientImage(imgani);
    apply_button_->Position(Math::Vector(300.0f, 73.0f, 0.0f));
    apply_button_->OnButtonClick(boost::bind(&OptionsDialog::OnButtonClickApply, this));
    Children().push_back(apply_button_);


    imgani = resources.FindImageAnim("Images/CheckboxEnableSound.tga");
    imgani->DrawCentered(false);

    enable_snd_checkbox_.reset(new Win::CheckBoxWindow(manager, this));
    enable_snd_checkbox_->ClientImage(imgani);
    enable_snd_checkbox_->Position(Math::Vector(40.0f, 156.0f, 0.0f));
    enable_snd_checkbox_->Checked(settings_.SoundEnabled());
    enable_snd_checkbox_->OnCheckBoxClick(boost::bind(&OptionsDialog::OnCheckBoxClickEnableSound, this));
    Children().push_back(enable_snd_checkbox_);


    imgani = resources.FindImageAnim("Images/TestButton.tga");
    imgani->DrawCentered(false);

    test_button_.reset(new Win::ButtonWindow(manager, this));
    test_button_->ClientImage(imgani);
    test_button_->Position(Math::Vector(300.0f, 139.0f, 0.0f));
    test_button_->OnButtonClick(boost::bind(&OptionsDialog::OnButtonClickTest, this));
    Children().push_back(test_button_);


    imgani = resources.FindImageAnim("Images/SliderBack.tga");
    imgani->DrawCentered(false);

    sound_slider_.reset(new Win::SliderWindow(manager, this));
    sound_slider_->ClientImage(imgani);
    sound_slider_->Position(Math::Vector(40.0f, 186.0f, 0.0f));
    sound_slider_->OnSliderValueChanged(boost::bind(&OptionsDialog::OnSoundSliderValueChanged, this));

    imgani = resources.FindImageAnim("Images/SliderBox.tga");
    imgani->DrawCentered(false);

    sound_slider_->BoxImage(imgani);
    sound_slider_->LeftMargin(8.0f);
    sound_slider_->RightMargin(28.0f);
    sound_slider_->VertOffset(3.0f);
    sound_slider_->MinValue(0);
    sound_slider_->MaxValue(100);
    sound_slider_->Value(settings_.SoundVolume());
    Children().push_back(sound_slider_);


    imgani = resources.FindImageAnim("Images/CheckboxEnableMusic.tga");
    imgani->DrawCentered(false);

    enable_mus_checkbox_.reset(new Win::CheckBoxWindow(manager, this));
    enable_mus_checkbox_->ClientImage(imgani);
    enable_mus_checkbox_->Position(Math::Vector(40.0f, 222.0f, 0.0f));
    enable_mus_checkbox_->Checked(settings_.MusicEnabled());
    enable_mus_checkbox_->OnCheckBoxClick(boost::bind(&OptionsDialog::OnCheckBoxClickEnableMusic, this));
    Children().push_back(enable_mus_checkbox_);


    imgani = resources.FindImageAnim("Images/SliderBack.tga");
    imgani->DrawCentered(false);

    music_slider_.reset(new Win::SliderWindow(manager, this));
    music_slider_->ClientImage(imgani);
    music_slider_->Position(Math::Vector(40.0f, 252.0f, 0.0f));
    music_slider_->OnSliderValueChanged(boost::bind(&OptionsDialog::OnMusicSliderValueChanged, this));

    imgani = resources.FindImageAnim("Images/SliderBox.tga");
    imgani->DrawCentered(false);

    music_slider_->BoxImage(imgani);
    music_slider_->LeftMargin(8.0f);
    music_slider_->RightMargin(28.0f);
    music_slider_->VertOffset(3.0f);
    music_slider_->MinValue(0);
    music_slider_->MaxValue(100);
    music_slider_->Value(settings_.MusicVolume());
    Children().push_back(music_slider_);


    imgani = resources.FindImageAnim("Images/CloseButton.tga");
    imgani->DrawCentered(false);

    close_button_.reset(new Win::ButtonWindow(manager, this));
    close_button_->ClientImage(imgani);
    close_button_->Position(Math::Vector(300.0f, 300.0f, 0.0f));
    close_button_->OnButtonClick(boost::bind(&OptionsDialog::OnButtonClickClose, this));
    Children().push_back(close_button_);

    EnableControls();
}

void OptionsDialog::OnButtonClickApply()
{
    settings_.FullscreenGraphics(fullscreen_gfx_checkbox_->Checked());
}

void OptionsDialog::OnButtonClickTest()
{
    snd_test_->Play2d();
}

void OptionsDialog::OnButtonClickClose()
{
    settings_.FullscreenGraphics(fullscreen_gfx_checkbox_->Checked());
    Close();
}

void OptionsDialog::OnCheckBoxClickFullscreenGraphics()
{
    fullscreen_changed_ = !fullscreen_changed_;
    EnableControls();
}

void OptionsDialog::OnCheckBoxClickEnableSound()
{
    settings_.SoundEnabled(enable_snd_checkbox_->Checked());
    EnableControls();
}

void OptionsDialog::OnCheckBoxClickEnableMusic()
{
    settings_.MusicEnabled(enable_mus_checkbox_->Checked());
    EnableControls();
}

void OptionsDialog::OnSoundSliderValueChanged()
{
    settings_.SoundVolume(sound_slider_->Value());
}

void OptionsDialog::OnMusicSliderValueChanged()
{
    settings_.MusicVolume(music_slider_->Value());
}

void OptionsDialog::EnableControls()
{
    apply_button_->Enabled(fullscreen_changed_);

    test_button_->Enabled(settings_.SoundEnabled());
    sound_slider_->Enabled(settings_.SoundEnabled());

    music_slider_->Enabled(settings_.MusicEnabled());
}
