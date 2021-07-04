#ifndef INCLUDED_HELPDIALOG
#define INCLUDED_HELPDIALOG

#include "../win/window.h"
#include "../win/buttonwindow.h"
#include "../util/resourcecontext.h"

struct IDialogControl;

class HelpDialog
    : public Win::Window
{
public:
    static const unsigned int PAGE_OVERVIEW;
    static const unsigned int PAGE_PLAYER_SHIP;
    static const unsigned int PAGE_WEAPON_SYSTEM;
    static const unsigned int PAGE_CANNON_GUN;
    static const unsigned int PAGE_LASER_GUN;
    static const unsigned int PAGE_MISSILES;
    static const unsigned int PAGE_PLASMA_GUN;
    static const unsigned int PAGE_RAIL_GUN;
    static const unsigned int PAGE_BOMB_SYSTEM;
    static const unsigned int PAGE_FUNKY_BOMB;
    static const unsigned int PAGE_EMP_BOMB;
    static const unsigned int PAGE_NUKE_BOMB;
    static const unsigned int PAGE_SCATTER_BOMB;
    static const unsigned int PAGE_ITEM_SYSTEM;

public:
    HelpDialog(Win::WindowManager* manager, IDialogControl* dialog_control, Util::ResourceContext& resources);

    void StartPage(int index) { current_page_ = index; }

private:
    void OnButtonClickPrev();
    void OnButtonClickNext();
    void OnButtonClickClose();

    void HideAllWindows();

private:
    Win::ButtonWindowPtr prev_button_;
    Win::ButtonWindowPtr next_button_;
    Win::ButtonWindowPtr close_button_;
    IDialogControl* dialog_control_;
    unsigned int current_page_;

    Win::WindowPtr overview_text_;
    Win::WindowPtr player_ship_text_;
    Win::WindowPtr weapon_system_text_;
    Win::WindowPtr cannon_gun_text_;
    Win::WindowPtr laser_gun_text_;
    Win::WindowPtr missiles_text_;
    Win::WindowPtr plasma_gun_text_;
    Win::WindowPtr rail_gun_text_;
    Win::WindowPtr bomb_system_text_;
    Win::WindowPtr funky_bomb_text_;
    Win::WindowPtr emp_bomb_text_;
    Win::WindowPtr nuke_bomb_text_;
    Win::WindowPtr scatter_bomb_text_;
    Win::WindowPtr item_system_text_;
};

typedef boost::shared_ptr<HelpDialog> HelpDialogPtr;

#endif  // INCLUDED_HELPDIALOG
