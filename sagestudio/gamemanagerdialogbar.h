#ifndef INCLUDED_GAMEMANAGERDIALOGBAR
#define INCLUDED_GAMEMANAGERDIALOGBAR

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>
#include <commctrl.h>
#include <boost/shared_ptr.hpp>
#include <string>

struct GameManagerDialogBarEvents
{
    virtual void OnTreeItemGameSelected(const std::string& game_tree_item) = 0;
    virtual void OnTreeItemGameTypeSelected(const std::string& game_type_tree_item) = 0;
    virtual void OnTreeItemGameInstanceSelected(const std::string& game_type_tree_item, const std::string& game_instance_tree_item) = 0;
    virtual void OnTreeItemGameRightClicked(const std::string& game_tree_item) = 0;
    virtual void OnTreeItemGameTypeRightClicked(const std::string& game_type_tree_item) = 0;
    virtual void OnTreeItemGameInstanceRightClicked(const std::string& game_type_tree_item, const std::string& game_instance_tree_item) = 0;
};

class GameManagerDialogBar
{
public:
    GameManagerDialogBar(HINSTANCE instance, HWND parent, GameManagerDialogBarEvents* event_handler);
    ~GameManagerDialogBar();

    void RepositionWithinParent();
    void Clear();

    void Enable();
    void Disable();
    void Show();
    void Hide();

    bool GetCurrentSelection(std::string& game_tree_item, std::string& game_type_tree_item, std::string& game_instance_tree_item) const;

    void InsertEntity(const std::string& name, bool select = true);
    void InsertCamera(const std::string& name, bool select = true);
    void InsertLight(const std::string& name, bool select = true);
    void InsertMusic(const std::string& name, bool select = true);
    void InsertTrigger(const std::string& name, bool select = true);
    void InsertMap(const std::string& name, bool select = true);
    void InsertStateMachine(const std::string& name, bool select = true);
    void InsertDialog(const std::string& name, bool select = true);
    void InsertFont(const std::string& name, bool select = true);
    void InsertResourceContext(const std::string& name, bool select = true);
    void InsertImage(const std::string& name, bool select = true);
    void InsertImageAnimation(const std::string& name, bool select = true);
    void InsertEntityList(const std::string& name, bool select = true);
    void InsertEngineParameter(const std::string& name, bool select = true);
    void InsertTextureSet(const std::string& name, bool select = true);

    void RemoveEntity(const std::string& name);
    void RemoveCamera(const std::string& name);
    void RemoveLight(const std::string& name);
    void RemoveMusic(const std::string& name);
    void RemoveTrigger(const std::string& name);
    void RemoveMap(const std::string& name);
    void RemoveStateMachine(const std::string& name);
    void RemoveDialog(const std::string& name);
    void RemoveFont(const std::string& name);
    void RemoveResourceContext(const std::string& name);
    void RemoveImage(const std::string& name);
    void RemoveImageAnimation(const std::string& name);
    void RemoveEntityList(const std::string& name);
    void RemoveEngineParameter(const std::string& name);
    void RemoveTextureSet(const std::string& name);

    void UpdateGame(const std::string& new_name);
    void UpdateEntity(const std::string& old_name, const std::string& new_name);
    void UpdateCamera(const std::string& old_name, const std::string& new_name);
    void UpdateLight(const std::string& old_name, const std::string& new_name);
    void UpdateMusic(const std::string& old_name, const std::string& new_name);
    void UpdateTrigger(const std::string& old_name, const std::string& new_name);
    void UpdateMap(const std::string& old_name, const std::string& new_name);
    void UpdateStateMachine(const std::string& old_name, const std::string& new_name);
    void UpdateDialog(const std::string& old_name, const std::string& new_name);
    void UpdateFont(const std::string& old_name, const std::string& new_name);
    void UpdateResourceContext(const std::string& old_name, const std::string& new_name);
    void UpdateImage(const std::string& old_name, const std::string& new_name);
    void UpdateImageAnimation(const std::string& old_name, const std::string& new_name);
    void UpdateEntityList(const std::string& old_name, const std::string& new_name);
    void UpdateEngineParameter(const std::string& old_name, const std::string& new_name);
    void UpdateTextureSet(const std::string& old_name, const std::string& new_name);

private:
    friend INT_PTR CALLBACK GameManagerDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void InsertParentTreeItems();

    std::string GetItemText(HTREEITEM item_handle) const;
    void SetItemText(HTREEITEM item_handle, const std::string& item_text);
    void InsertChildItem(const std::string& child_text, HTREEITEM parent_item, bool select);
    void RemoveChildItem(const std::string& child_text, HTREEITEM parent_item);
    void UpdateChildItem(const std::string& old_name, const std::string& new_name, HTREEITEM parent_item);

private:
    HINSTANCE instance_;
    HWND handle_;
    HWND tree_ctrl_;
    GameManagerDialogBarEvents* event_handler_;

    HTREEITEM game_tree_item_;
    HTREEITEM entities_tree_item_;
    HTREEITEM cameras_tree_item_;
    HTREEITEM texturesets_tree_item_;
    HTREEITEM lights_tree_item_;
    HTREEITEM music_tree_item_;
    HTREEITEM triggers_tree_item_;
    HTREEITEM default_bindings_tree_item_;
    HTREEITEM map_tree_item_;
    HTREEITEM statemachines_tree_item_;
    HTREEITEM dialogs_tree_item_;
    HTREEITEM fonts_tree_item_;
    HTREEITEM resourcecontexts_tree_item_;
    HTREEITEM images_tree_item_;
    HTREEITEM imageanimations_tree_item_;
    HTREEITEM entitylists_tree_item_;
    HTREEITEM engineparameters_tree_item_;
};

typedef boost::shared_ptr<GameManagerDialogBar> GameManagerDialogBarPtr;

#endif  // INCLUDED_GAMEMANAGERDIALOGBAR
