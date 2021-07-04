#ifndef INCLUDED_IDIALOGCONTROL
#define INCLUDED_IDIALOGCONTROL

struct IDialogControl
{
    virtual void ShowMainDialog() = 0;
    virtual void ShowOptionsDialog() = 0;
    virtual void ShowScoresDialog(bool new_highscore = false, unsigned int level = 0, unsigned long score = 0) = 0;
    virtual void ShowHelpDialog() = 0;
    virtual void ShowInGameDialog() = 0;

    virtual void HideAllWindows() = 0;

    virtual void StartNewGame() = 0;
    virtual void ResumeCurrentGame() = 0;
    virtual void EndCurrentGame() = 0;

    virtual void ExitApplication() = 0;
};

#endif  // INCLUDED_IDIALOGCONTROL
