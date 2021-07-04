#ifndef INCLUDED_HIGHSCORESDIALOG
#define INCLUDED_HIGHSCORESDIALOG

#include "../win/window.h"
#include "../win/buttonwindow.h"
#include "../win/listviewwindow.h"
#include "../win/editboxwindow.h"
#include "../util/resourcecontext.h"
#include "../aud/aud.h"

struct IDialogControl;
class Highscores;

class HighscoresDialog
    : public Win::Window
{
public:
    HighscoresDialog(Win::WindowManager* manager, IDialogControl* dialog_control, Util::ResourceContext& resources, Highscores& highscores);

    void EnterNewHighscore(bool enter_new_highscore, unsigned int level = 0, unsigned long score = 0);

private:
    void OnWindowActivated();
    void OnButtonClickReset();
    void OnButtonClickClose();
    void OnButtonClickOK();
    void OnButtonClickCancel();

    void RefreshListView();

private:
    Win::ButtonWindowPtr reset_button_;
    Win::ButtonWindowPtr close_button_;
    Win::ListViewWindowPtr highscores_listview_;
    Win::WindowPtr instructions_image_;
    Win::EditBoxWindowPtr name_editbox_;
    Win::ButtonWindowPtr ok_button_;
    Win::ButtonWindowPtr cancel_button_;
    IDialogControl* dialog_control_;
    Highscores& highscores_;
    unsigned int level_;
    unsigned long score_;
    Aud::SoundPtr snd_newhighscore_;
};

typedef boost::shared_ptr<HighscoresDialog> HighscoresDialogPtr;

#endif  // INCLUDED_HIGHSCORESDIALOG
