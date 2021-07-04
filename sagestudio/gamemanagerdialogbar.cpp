#include "gamemanagerdialogbar.h"
#include "resource.h"
#include <stdexcept>
#include <windowsx.h>
#include <commctrl.h>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'GameManagerDialogBar *' of greater size

GameManagerDialogBar::GameManagerDialogBar(HINSTANCE instance, HWND parent, GameManagerDialogBarEvents* event_handler)
: instance_(instance)
, event_handler_(event_handler)
{
    handle_ = CreateDialogParam(instance, MAKEINTRESOURCE(IDD_GAMEMANAGER_DIALOGBAR), parent,
        GameManagerDialogBarProc, reinterpret_cast<LPARAM>(this));
    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to create an GameManagerDialogBar instance");
    }

    tree_ctrl_ = GetDlgItem(handle_, IDC_GAME_TREE);
    InsertParentTreeItems();
}

GameManagerDialogBar::~GameManagerDialogBar()
{
    DestroyWindow(handle_);
}

void GameManagerDialogBar::InsertParentTreeItems()
{
    char buffer[128];
    memset(buffer, 0, sizeof(char)*128);

    TVINSERTSTRUCT item_info;
    memset(&item_info, 0, sizeof(TVINSERTSTRUCT));
    item_info.item.mask     = TVIF_TEXT;
    item_info.item.pszText  = buffer;

    strcpy_s(buffer, 128, "Unsaved");
    game_tree_item_ = TreeView_InsertItem(tree_ctrl_, &item_info);

    item_info.hParent       = game_tree_item_;
    item_info.hInsertAfter  = TVI_SORT;

    strcpy_s(buffer, 128, "Entities");
    entities_tree_item_ = TreeView_InsertItem(tree_ctrl_, &item_info);

    strcpy_s(buffer, 128, "Music");
    music_tree_item_ = TreeView_InsertItem(tree_ctrl_, &item_info);

    strcpy_s(buffer, 128, "Default Bindings");
    default_bindings_tree_item_ = TreeView_InsertItem(tree_ctrl_, &item_info);

    strcpy_s(buffer, 128, "Triggers");
    triggers_tree_item_ = TreeView_InsertItem(tree_ctrl_, &item_info);

    strcpy_s(buffer, 128, "Cell Maps");
    map_tree_item_ = TreeView_InsertItem(tree_ctrl_, &item_info);

    strcpy_s(buffer, 128, "State Machines");
    statemachines_tree_item_ = TreeView_InsertItem(tree_ctrl_, &item_info);

    strcpy_s(buffer, 128, "Dialogs");
    dialogs_tree_item_ = TreeView_InsertItem(tree_ctrl_, &item_info);

    strcpy_s(buffer, 128, "Fonts");
    fonts_tree_item_ = TreeView_InsertItem(tree_ctrl_, &item_info);

    strcpy_s(buffer, 128, "Resource Contexts");
    resourcecontexts_tree_item_ = TreeView_InsertItem(tree_ctrl_, &item_info);

    strcpy_s(buffer, 128, "Images");
    images_tree_item_ = TreeView_InsertItem(tree_ctrl_, &item_info);

    strcpy_s(buffer, 128, "Image Animations");
    imageanimations_tree_item_ = TreeView_InsertItem(tree_ctrl_, &item_info);

    strcpy_s(buffer, 128, "Entity Lists");
    entitylists_tree_item_ = TreeView_InsertItem(tree_ctrl_, &item_info);

    strcpy_s(buffer, 128, "Engine Parameters");
    engineparameters_tree_item_ = TreeView_InsertItem(tree_ctrl_, &item_info);

    strcpy_s(buffer, 128, "Cameras");
    cameras_tree_item_ = TreeView_InsertItem(tree_ctrl_, &item_info);

    strcpy_s(buffer, 128, "Texture Sets");
    texturesets_tree_item_ = TreeView_InsertItem(tree_ctrl_, &item_info);

    TreeView_Expand(tree_ctrl_, game_tree_item_, TVE_EXPAND);
}

void GameManagerDialogBar::RepositionWithinParent()
{
    RECT client_rect;
    GetClientRect(handle_, &client_rect);

    RECT parent_client_rect;
    GetClientRect(GetParent(handle_), &parent_client_rect);

    SetWindowPos(handle_, NULL, 0, 0, 200, parent_client_rect.bottom, SWP_NOZORDER);
    SetWindowPos(tree_ctrl_, NULL, 8, 8, 184, parent_client_rect.bottom-16, SWP_NOZORDER);
}

void GameManagerDialogBar::Clear()
{
    TreeView_DeleteAllItems(tree_ctrl_);
    InsertParentTreeItems();
}

void GameManagerDialogBar::Enable()
{
    EnableWindow(tree_ctrl_, TRUE);
    EnableWindow(handle_, TRUE);
}

void GameManagerDialogBar::Disable()
{
    EnableWindow(tree_ctrl_, FALSE);
    EnableWindow(handle_, FALSE);
}

void GameManagerDialogBar::Show()
{
    ShowWindow(handle_, SW_SHOW);
    UpdateWindow(handle_);
}

void GameManagerDialogBar::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}

bool GameManagerDialogBar::GetCurrentSelection(std::string& game_tree_item, std::string& game_type_tree_item, std::string& game_instance_tree_item) const
{
    HTREEITEM selected_item = TreeView_GetSelection(tree_ctrl_);
    if(selected_item == NULL)
    {
        return false;
    }

    HTREEITEM parent = TreeView_GetParent(tree_ctrl_, selected_item);
    if(parent == NULL)
    {
        game_tree_item = GetItemText(selected_item);
        game_type_tree_item.clear();
        game_instance_tree_item.clear();
    }
    else if(parent == game_tree_item_)
    {
        game_tree_item = GetItemText(game_tree_item_);
        game_type_tree_item = GetItemText(selected_item);
        game_instance_tree_item.clear();
    }
    else
    {
        game_tree_item = GetItemText(game_tree_item_);
        game_type_tree_item = GetItemText(TreeView_GetParent(tree_ctrl_, selected_item));
        game_instance_tree_item = GetItemText(selected_item);
    }
    return true;
}

void GameManagerDialogBar::InsertEntity(const std::string& name, bool select)
{
    InsertChildItem(name, entities_tree_item_, select);
}

void GameManagerDialogBar::InsertCamera(const std::string& name, bool select)
{
    InsertChildItem(name, cameras_tree_item_, select);
}

void GameManagerDialogBar::InsertLight(const std::string& name, bool select)
{
    InsertChildItem(name, lights_tree_item_, select);
}

void GameManagerDialogBar::InsertMusic(const std::string& name, bool select)
{
    InsertChildItem(name, music_tree_item_, select);
}

void GameManagerDialogBar::InsertTrigger(const std::string& name, bool select)
{
    InsertChildItem(name, triggers_tree_item_, select);
}

void GameManagerDialogBar::InsertMap(const std::string& name, bool select)
{
    InsertChildItem(name, map_tree_item_, select);
}

void GameManagerDialogBar::InsertStateMachine(const std::string& name, bool select)
{
    InsertChildItem(name, statemachines_tree_item_, select);
}

void GameManagerDialogBar::InsertDialog(const std::string& name, bool select)
{
    InsertChildItem(name, dialogs_tree_item_, select);
}

void GameManagerDialogBar::InsertFont(const std::string& name, bool select)
{
    InsertChildItem(name, fonts_tree_item_, select);
}

void GameManagerDialogBar::InsertResourceContext(const std::string& name, bool select)
{
    InsertChildItem(name, resourcecontexts_tree_item_, select);
}

void GameManagerDialogBar::InsertImage(const std::string& name, bool select)
{
    InsertChildItem(name, images_tree_item_, select);
}

void GameManagerDialogBar::InsertImageAnimation(const std::string& name, bool select)
{
    InsertChildItem(name, imageanimations_tree_item_, select);
}

void GameManagerDialogBar::InsertEntityList(const std::string& name, bool select)
{
    InsertChildItem(name, entitylists_tree_item_, select);
}

void GameManagerDialogBar::InsertEngineParameter(const std::string& name, bool select)
{
    InsertChildItem(name, engineparameters_tree_item_, select);
}

void GameManagerDialogBar::InsertTextureSet(const std::string& name, bool select)
{
    InsertChildItem(name, texturesets_tree_item_, select);
}

void GameManagerDialogBar::RemoveEntity(const std::string& name)
{
    RemoveChildItem(name, entities_tree_item_);
}

void GameManagerDialogBar::RemoveCamera(const std::string& name)
{
    RemoveChildItem(name, cameras_tree_item_);
}

void GameManagerDialogBar::RemoveLight(const std::string& name)
{
    RemoveChildItem(name, lights_tree_item_);
}

void GameManagerDialogBar::RemoveMusic(const std::string& name)
{
    RemoveChildItem(name, music_tree_item_);
}

void GameManagerDialogBar::RemoveTrigger(const std::string& name)
{
    RemoveChildItem(name, triggers_tree_item_);
}

void GameManagerDialogBar::RemoveMap(const std::string& name)
{
    RemoveChildItem(name, map_tree_item_);
}

void GameManagerDialogBar::RemoveStateMachine(const std::string& name)
{
    RemoveChildItem(name, statemachines_tree_item_);
}

void GameManagerDialogBar::RemoveDialog(const std::string& name)
{
    RemoveChildItem(name, dialogs_tree_item_);
}

void GameManagerDialogBar::RemoveFont(const std::string& name)
{
    RemoveChildItem(name, fonts_tree_item_);
}

void GameManagerDialogBar::RemoveResourceContext(const std::string& name)
{
    RemoveChildItem(name, resourcecontexts_tree_item_);
}

void GameManagerDialogBar::RemoveImage(const std::string& name)
{
    RemoveChildItem(name, images_tree_item_);
}

void GameManagerDialogBar::RemoveImageAnimation(const std::string& name)
{
    RemoveChildItem(name, imageanimations_tree_item_);
}

void GameManagerDialogBar::RemoveEntityList(const std::string& name)
{
    RemoveChildItem(name, entitylists_tree_item_);
}

void GameManagerDialogBar::RemoveEngineParameter(const std::string& name)
{
    RemoveChildItem(name, engineparameters_tree_item_);
}

void GameManagerDialogBar::RemoveTextureSet(const std::string& name)
{
    RemoveChildItem(name, texturesets_tree_item_);
}

void GameManagerDialogBar::UpdateGame(const std::string& new_name)
{
    SetItemText(game_tree_item_, new_name);
}

void GameManagerDialogBar::UpdateEntity(const std::string& old_name, const std::string& new_name)
{
    UpdateChildItem(old_name, new_name, entities_tree_item_);
}

void GameManagerDialogBar::UpdateCamera(const std::string& old_name, const std::string& new_name)
{
    UpdateChildItem(old_name, new_name, cameras_tree_item_);
}

void GameManagerDialogBar::UpdateLight(const std::string& old_name, const std::string& new_name)
{
    UpdateChildItem(old_name, new_name, lights_tree_item_);
}

void GameManagerDialogBar::UpdateMusic(const std::string& old_name, const std::string& new_name)
{
    UpdateChildItem(old_name, new_name, music_tree_item_);
}

void GameManagerDialogBar::UpdateTrigger(const std::string& old_name, const std::string& new_name)
{
    UpdateChildItem(old_name, new_name, triggers_tree_item_);
}

void GameManagerDialogBar::UpdateMap(const std::string& old_name, const std::string& new_name)
{
    UpdateChildItem(old_name, new_name, map_tree_item_);
}

void GameManagerDialogBar::UpdateStateMachine(const std::string& old_name, const std::string& new_name)
{
    UpdateChildItem(old_name, new_name, statemachines_tree_item_);
}

void GameManagerDialogBar::UpdateDialog(const std::string& old_name, const std::string& new_name)
{
    UpdateChildItem(old_name, new_name, dialogs_tree_item_);
}

void GameManagerDialogBar::UpdateFont(const std::string& old_name, const std::string& new_name)
{
    UpdateChildItem(old_name, new_name, fonts_tree_item_);
}

void GameManagerDialogBar::UpdateResourceContext(const std::string& old_name, const std::string& new_name)
{
    UpdateChildItem(old_name, new_name, resourcecontexts_tree_item_);
}

void GameManagerDialogBar::UpdateImage(const std::string& old_name, const std::string& new_name)
{
    UpdateChildItem(old_name, new_name, images_tree_item_);
}

void GameManagerDialogBar::UpdateImageAnimation(const std::string& old_name, const std::string& new_name)
{
    UpdateChildItem(old_name, new_name, imageanimations_tree_item_);
}

void GameManagerDialogBar::UpdateEntityList(const std::string& old_name, const std::string& new_name)
{
    UpdateChildItem(old_name, new_name, entitylists_tree_item_);
}

void GameManagerDialogBar::UpdateEngineParameter(const std::string& old_name, const std::string& new_name)
{
    UpdateChildItem(old_name, new_name, engineparameters_tree_item_);
}

void GameManagerDialogBar::UpdateTextureSet(const std::string& old_name, const std::string& new_name)
{
    UpdateChildItem(old_name, new_name, texturesets_tree_item_);
}

std::string GameManagerDialogBar::GetItemText(HTREEITEM item_handle) const
{
    TVITEM item;
    memset(&item, 0, sizeof(TVITEM));

    char buffer[128];
    memset(buffer, 0, sizeof(char)*128);

    item.mask           = TVIF_HANDLE | TVIF_TEXT;
    item.hItem          = item_handle;
    item.pszText        = buffer;
    item.cchTextMax     = 128;
    TreeView_GetItem(tree_ctrl_, &item);

    return std::string(buffer);
}

void GameManagerDialogBar::SetItemText(HTREEITEM item_handle, const std::string& item_text)
{
    TVITEM info;
    memset(&info, 0, sizeof(TVITEM));

    char buffer[128];
    strcpy_s(buffer, 128, item_text.c_str());

    info.mask           = TVIF_HANDLE | TVIF_TEXT;
    info.hItem          = item_handle;
    info.pszText        = buffer;
    info.cchTextMax     = 128;

    TreeView_SetItem(tree_ctrl_, &info);
}

void GameManagerDialogBar::InsertChildItem(const std::string& child_text, HTREEITEM parent_item, bool select)
{
    char buffer[128];
    memset(buffer, 0, sizeof(char)*128);

    TVINSERTSTRUCT item_info;
    memset(&item_info, 0, sizeof(TVINSERTSTRUCT));
    item_info.item.mask     = TVIF_TEXT;
    item_info.item.pszText  = buffer;
    item_info.hParent       = parent_item;

    strcpy_s(buffer, 128, child_text.c_str());
    HTREEITEM child = TreeView_InsertItem(tree_ctrl_, &item_info);
    if(select)
    {
        TreeView_SelectItem(tree_ctrl_, child);
    }
}

void GameManagerDialogBar::RemoveChildItem(const std::string& child_text, HTREEITEM parent_item)
{
    HTREEITEM child = TreeView_GetChild(tree_ctrl_, parent_item);
    while(child)
    {
        if(GetItemText(child) == child_text)
        {
            TreeView_DeleteItem(tree_ctrl_, child);
            break;
        }
        child = TreeView_GetNextSibling(tree_ctrl_, child);
    }
}

void GameManagerDialogBar::UpdateChildItem(const std::string& old_name, const std::string& new_name, HTREEITEM parent_item)
{
    HTREEITEM child = TreeView_GetChild(tree_ctrl_, parent_item);
    while(child)
    {
        if(GetItemText(child) == old_name)
        {
            SetItemText(child, new_name);
            break;
        }
        child = TreeView_GetNextSibling(tree_ctrl_, child);
    }
}





INT_PTR CALLBACK GameManagerDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    GameManagerDialogBar* this_ = reinterpret_cast<GameManagerDialogBar*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));
            this_ = reinterpret_cast<GameManagerDialogBar*>(lparam);
            return TRUE;        // Let the system call SetFocus()
        }
    case WM_NOTIFY:
        {
            NMHDR* notify_info = reinterpret_cast<NMHDR*>(lparam);
            switch(notify_info->idFrom)
            {
            case IDC_GAME_TREE:
                switch(notify_info->code)
                {
                case TVN_SELCHANGED:
                    {
                        NMTREEVIEW* tree_info = reinterpret_cast<NMTREEVIEW*>(lparam);

                        HTREEITEM parent_item = TreeView_GetParent(this_->tree_ctrl_, tree_info->itemNew.hItem);
                        if(parent_item == NULL)
                        {
                            this_->event_handler_->OnTreeItemGameSelected(this_->GetItemText(tree_info->itemNew.hItem));
                        }
                        else if(parent_item == this_->game_tree_item_)
                        {
                            this_->event_handler_->OnTreeItemGameTypeSelected(this_->GetItemText(tree_info->itemNew.hItem));
                        }
                        else
                        {
                            this_->event_handler_->OnTreeItemGameInstanceSelected(this_->GetItemText(parent_item), this_->GetItemText(tree_info->itemNew.hItem));
                        }
                        break;
                    }
                case NM_RCLICK:
                    {
                        TVHITTESTINFO info;
                        memset(&info, 0, sizeof(TVHITTESTINFO));

                        GetCursorPos(&info.pt);
                        ScreenToClient(this_->tree_ctrl_, &info.pt);

                        TreeView_HitTest(this_->tree_ctrl_, &info);
                        if(info.hItem)
                        {
                            TreeView_SelectItem(this_->tree_ctrl_, info.hItem);

                            HTREEITEM parent_item = TreeView_GetParent(this_->tree_ctrl_, info.hItem);
                            if(parent_item == NULL)
                            {
                                this_->event_handler_->OnTreeItemGameRightClicked(this_->GetItemText(info.hItem));
                            }
                            else if(parent_item == this_->game_tree_item_)
                            {
                                this_->event_handler_->OnTreeItemGameTypeRightClicked(this_->GetItemText(info.hItem));
                            }
                            else
                            {
                                this_->event_handler_->OnTreeItemGameInstanceRightClicked(this_->GetItemText(parent_item), this_->GetItemText(info.hItem));
                            }
                        }
                        break;
                    }
                }
                break;
            }
            break;
        }
    }

    return FALSE;
}
