#ifndef INCLUDED_INGAMEDIALOG
#define INCLUDED_INGAMEDIALOG

#include "../win/window.h"
#include "../win/buttonwindow.h"
#include "../util/resourcecontext.h"

struct IDialogControl;

class InGameDialog
    : public Win::Window
{
public:
    InGameDialog(Win::WindowManager* manager, IDialogControl* dialog_control, Util::ResourceContext& resources);

private:
    void OnButtonClickResume();
    void OnButtonClickOptions();
    void OnButtonClickEnd();
    void OnButtonClickExit();

private:
    Win::ButtonWindowPtr resume_button_;
    Win::ButtonWindowPtr options_button_;
    Win::ButtonWindowPtr stop_button;
    Win::ButtonWindowPtr exit_button_;
    IDialogControl* dialog_control_;
};

typedef boost::shared_ptr<InGameDialog> InGameDialogPtr;

#endif  // INCLUDED_INGAMEDIALOG
