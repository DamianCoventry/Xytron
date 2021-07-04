#ifndef INCLUDED_OPTIONSDIALOG
#define INCLUDED_OPTIONSDIALOG

#include "../win/window.h"
#include "../win/buttonwindow.h"
#include "../win/checkboxwindow.h"
#include "../win/sliderwindow.h"
#include "../util/resourcecontext.h"
#include "../aud/aud.h"

struct IDialogControl;
class Settings;

class OptionsDialog
    : public Win::Window
{
public:
    OptionsDialog(Win::WindowManager* manager, IDialogControl* dialog_control, Util::ResourceContext& resources, Settings& settings);

private:
    void OnButtonClickApply();
    void OnButtonClickTest();
    void OnButtonClickClose();
    void OnCheckBoxClickFullscreenGraphics();
    void OnCheckBoxClickEnableSound();
    void OnCheckBoxClickEnableMusic();
    void OnSoundSliderValueChanged();
    void OnMusicSliderValueChanged();

private:
    void EnableControls();

private:
    Win::ButtonWindowPtr apply_button_;
    Win::ButtonWindowPtr test_button_;
    Win::ButtonWindowPtr close_button_;
    Win::CheckBoxWindowPtr fullscreen_gfx_checkbox_;
    Win::CheckBoxWindowPtr enable_snd_checkbox_;
    Win::CheckBoxWindowPtr enable_mus_checkbox_;
    Win::SliderWindowPtr sound_slider_;
    Win::SliderWindowPtr music_slider_;
    Aud::SoundPtr snd_test_;
    IDialogControl* dialog_control_;
    Settings& settings_;
    bool fullscreen_changed_;
};

typedef boost::shared_ptr<OptionsDialog> OptionsDialogPtr;

#endif  // INCLUDED_OPTIONSDIALOG
