#ifndef INCLUDED_MAINDIALOG
#define INCLUDED_MAINDIALOG

#include "../win/window.h"
#include "../win/buttonwindow.h"
#include "../util/resourcecontext.h"

struct IDialogControl;

class MainDialog
    : public Win::Window
{
public:
    MainDialog(Win::WindowManager* manager, IDialogControl* dialog_control, Util::ResourceContext& resources);

private:
    void OnButtonClickGame();
    void OnButtonClickOptions();
    void OnButtonClickScores();
    void OnButtonClickHelp();
    void OnButtonClickExit();

private:
    Win::ButtonWindowPtr game_button_;
    Win::ButtonWindowPtr options_button_;
    Win::ButtonWindowPtr scores_button_;
    Win::ButtonWindowPtr help_button_;
    Win::ButtonWindowPtr exit_button_;
    IDialogControl* dialog_control_;
};

typedef boost::shared_ptr<MainDialog> MainDialogPtr;

#endif  // INCLUDED_MAINDIALOG
