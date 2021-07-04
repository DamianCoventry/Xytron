#include "sagestudiowindow.h"
#include "resource.h"

#include "openfiledialog.h"
#include "savefiledialog.h"
#include "gettextdialog.h"
#include "contentdirectorydialog.h"
#include "creatingthumbnailsdialog.h"
#include "statenodepropertiesdialog.h"
#include "gamepropertiesdialog.h"
#include "camerapropertiesdialog.h"
#include "choosecelltypedialog.h"
#include "choosetexturesetdialog.h"
#include "bindingopenglimagesdialog.h"
#include "texturesetpropertiesdialog.h"
#include "choosetexturedialog.h"
#include "loadmapprogressdialog.h"
#include "savemapprogressdialog.h"
#include "lightingprogressdialog.h"

#include <commdlg.h>
#include <boost/bind.hpp>
#include <stdexcept>
#include <sstream>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'SageStudioWindow *' of greater size

const char* SageStudioWindow::window_title_ = "Sage Studio v1.0";
const char* SageStudioWindow::class_name_   = "SageStudioWindow";

SageStudioWindow::SageStudioWindow(HINSTANCE instance)
: instance_(instance)
, handle_(NULL)
, first_time_load_(false)
{
    Register();
    Create();

    state_machine_window_.reset(new StateMachineWindow(instance, handle_, this));
    empty_studio_window_.reset(new EmptyStudioWindow(instance, handle_));
    game_manager_dialog_bar_.reset(new GameManagerDialogBar(instance, handle_, this));
    state_machine_tools_dialog_bar_.reset(new StateMachineToolsDialogBar(instance, handle_, this));
    resource_context_dialog_bar_.reset(new ResourceContextDialogBar(instance, handle_, this));
    image_window_.reset(new ImageWindow(instance, handle_, this));
    image_props_dialog_bar_.reset(new ImagePropertiesDialogBar(instance, handle_, this));
    image_tools_dialog_bar_.reset(new ImageToolsDialogBar(instance, handle_, this));
    cell_map_window_.reset(new CellMapWindow(instance, handle_, this, &audio_device_));
    cell_map_view_dialog_bar_.reset(new CellMapViewDialogBar(instance, handle_, this));
    cell_map_props_dialog_bar_.reset(new CellMapPropsDialogBar(instance, handle_, this));
    default_bindings_dialog_bar_.reset(new DefaultBindingsDialogBar(instance, handle_, this));
    texture_set_dialog_bar_.reset(new TextureSetDialogBar(instance, handle_, this));
    texture_set_manager_dialog_bar_.reset(new TextureSetManagerDialogBar(instance, handle_, this));
    sky_dialog_bar_.reset(new SkyDialogBar(instance, handle_, this));
    lights_dialog_bar_.reset(new LightsDialogBar(instance, handle_, this, &cell_map_));

    empty_studio_window_->Show();
    game_manager_dialog_bar_->Disable();

    state_machine_window_->Hide();
    state_machine_window_->Disable();
    state_machine_tools_dialog_bar_->Hide();
    state_machine_tools_dialog_bar_->Disable();
    resource_context_dialog_bar_->Hide();
    resource_context_dialog_bar_->Disable();
    image_window_->Hide();
    image_window_->Disable();
    image_props_dialog_bar_->Hide();
    image_props_dialog_bar_->Disable();
    image_tools_dialog_bar_->Hide();
    image_tools_dialog_bar_->Disable();
    cell_map_window_->Hide();
    cell_map_window_->Disable();
    cell_map_view_dialog_bar_->Hide();
    cell_map_view_dialog_bar_->Disable();
    cell_map_props_dialog_bar_->Hide();
    cell_map_props_dialog_bar_->Disable();
    default_bindings_dialog_bar_->Hide();
    default_bindings_dialog_bar_->Disable();
    texture_set_dialog_bar_->Hide();
    texture_set_dialog_bar_->Disable();
    texture_set_manager_dialog_bar_->Hide();
    texture_set_manager_dialog_bar_->Disable();
    sky_dialog_bar_->Hide();
    sky_dialog_bar_->Disable();
    lights_dialog_bar_->Hide();
    lights_dialog_bar_->Disable();

    EnableMenuItems();
}

SageStudioWindow::~SageStudioWindow()
{
    dib_resources_.UnloadAll();
    audio_device_.Shutdown();

    lights_dialog_bar_.reset();
    sky_dialog_bar_.reset();
    texture_set_manager_dialog_bar_.reset();
    texture_set_dialog_bar_.reset();
    default_bindings_dialog_bar_.reset();
    cell_map_view_dialog_bar_.reset();
    cell_map_props_dialog_bar_.reset();
    cell_map_window_.reset();
    image_tools_dialog_bar_.reset();
    image_props_dialog_bar_.reset();
    image_window_.reset();
    resource_context_dialog_bar_.reset();
    state_machine_tools_dialog_bar_.reset();
    state_machine_window_.reset();
    game_manager_dialog_bar_.reset();

    Delete();
    Unregister();
}

void SageStudioWindow::Show(int cmd_show)
{
    ShowWindow(handle_, cmd_show);
    UpdateWindow(handle_);
}

void SageStudioWindow::Hide()
{
    ShowWindow(handle_, SW_HIDE);
}





void SageStudioWindow::Register()
{
    WNDCLASS wc;
    wc.style            = CS_OWNDC | CS_DBLCLKS;
    wc.lpfnWndProc      = SageStudioWindowProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = instance_;
    wc.hIcon            = LoadIcon(instance_, MAKEINTRESOURCE(IDI_SAGESTUDIO));
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = MAKEINTRESOURCE(IDR_MAIN_WIN_MENU);
    wc.lpszClassName    = class_name_;

    if(!RegisterClass(&wc))
    {
        throw std::runtime_error("Unable to register the main window's window class");
    }
}

void SageStudioWindow::Unregister()
{
    UnregisterClass(class_name_, instance_);
}

void SageStudioWindow::Create()
{
    handle_ = CreateWindowEx(WS_EX_WINDOWEDGE, class_name_, window_title_, WS_OVERLAPPEDWINDOW | WS_MAXIMIZE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, GetDesktopWindow(),
        NULL, instance_, reinterpret_cast<void*>(this));

    if(handle_ == NULL)
    {
        throw std::runtime_error("Unable to register the main window");
    }
}

void SageStudioWindow::Delete()
{
    if(handle_)
    {
        DestroyWindow(handle_);
        handle_ = NULL;
    }
}

void SageStudioWindow::EnableMenuItems()
{
    HMENU menu = GetMenu(handle_);

    HMENU sub_menu = GetSubMenu(menu, 0);
    EnableMenuItem(sub_menu, ID_FILE_SAVE40003, MF_BYCOMMAND | ((doc_.Open() && doc_.Modified()) ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(sub_menu, ID_FILE_SAVEAS, MF_BYCOMMAND | (doc_.Open() ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(sub_menu, ID_FILE_CLOSE40005, MF_BYCOMMAND | (doc_.Open() ? MF_ENABLED : MF_GRAYED));

    sub_menu = GetSubMenu(menu, 1);
    EnableMenuItem(sub_menu, ID_EDIT_INSERT, MF_BYCOMMAND | (doc_.Open() ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(sub_menu, ID_EDIT_UNDO40007, MF_BYCOMMAND | (doc_.Open() ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(sub_menu, ID_EDIT_REDO40008, MF_BYCOMMAND | (doc_.Open() ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(sub_menu, ID_EDIT_CUT40009, MF_BYCOMMAND | (doc_.Open() ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(sub_menu, ID_EDIT_COPY40010, MF_BYCOMMAND | (doc_.Open() ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(sub_menu, ID_EDIT_PASTE40011, MF_BYCOMMAND | (doc_.Open() ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(sub_menu, ID_EDIT_DELETE, MF_BYCOMMAND | (doc_.Open() ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(sub_menu, ID_EDIT_PROPERTIES, MF_BYCOMMAND | (doc_.Open() ? MF_ENABLED : MF_GRAYED));
}

bool SageStudioWindow::ModifiedCheck()
{
    if(doc_.Modified())
    {
        switch(MessageBox(handle_, "Save changes to this game?", "Confirmation", MB_ICONQUESTION | MB_YESNOCANCEL))
        {
        case IDYES:
            OnFileSave();
            break;
        case IDNO:
            break;
        case IDCANCEL:
            return false;
        }
    }
    return true;
}

void SageStudioWindow::DoFileClose()
{
    doc_.Close();
    cell_map_.Clear();
    state_machine_window_->Clear();

    EnableMenuItems();

    empty_studio_window_->Show();
    game_manager_dialog_bar_->Clear();
    game_manager_dialog_bar_->Disable();
    state_machine_window_->Clear();
    state_machine_window_->Hide();
    state_machine_tools_dialog_bar_->Hide();
    resource_context_dialog_bar_->Clear();
    resource_context_dialog_bar_->Hide();
    image_window_->Clear();
    image_window_->Hide();
    image_props_dialog_bar_->Clear();
    image_props_dialog_bar_->Hide();
    image_tools_dialog_bar_->Clear();
    image_tools_dialog_bar_->Hide();
    cell_map_window_->Clear();
    cell_map_window_->Hide();
    cell_map_view_dialog_bar_->Clear();
    cell_map_view_dialog_bar_->Hide();
    cell_map_props_dialog_bar_->Clear();
    cell_map_props_dialog_bar_->Hide();
    default_bindings_dialog_bar_->Clear();
    default_bindings_dialog_bar_->Hide();
    texture_set_dialog_bar_->Clear();
    texture_set_dialog_bar_->Hide();
    texture_set_manager_dialog_bar_->Clear();
    texture_set_manager_dialog_bar_->Hide();
    sky_dialog_bar_->Clear();
    sky_dialog_bar_->Hide();
    lights_dialog_bar_->Clear();
    lights_dialog_bar_->Hide();

    cell_map_window_->UnloadResourceContext();

    current_view_.clear();
    game_instance_tree_item_.clear();

    SetWindowTitle();
}

void SageStudioWindow::DoFileSave(const std::string& filename)
{
    SaveCurrentMap();

    switch(doc_.Save(filename, content_dir_))
    {
    case GameDocument::SE_OK:
        break;
    case GameDocument::SE_OPEN_FAILURE:
        MessageBox(handle_, "Unable to save to that file", "Information", MB_OK | MB_ICONINFORMATION);
        return;
    }

    EnableMenuItems();
    SetWindowTitle();
}

void SageStudioWindow::DoFirstTimeLoad()
{
    CreatingThumbnailsDialog dialog;
    dialog.ContentDirectory(content_dir_);
    dialog.DibResources(&dib_resources_);
    dialog.ImageDocuments(&(doc_.ImageDocuments()));
    dialog.ImageAnimationDocuments(&(doc_.ImageAnimationDocuments()));
    dialog.Run(instance_, handle_);
}

void SageStudioWindow::SetWindowTitle()
{
    std::ostringstream oss;
    if(doc_.IsNew())
    {
        oss << window_title_ << " - [*" << doc_.Name() << " (Unsaved)]";
    }
    else if(doc_.Filename().empty())
    {
        oss << window_title_;
    }
    else
    {
        oss << window_title_ << " - " << (doc_.Modified() ? "[*" : "[") << doc_.Name() << " (" << doc_.Filename() << ")]";
    }
    SetWindowText(handle_, oss.str().c_str());
}

StateMachineDocument* SageStudioWindow::GetCurrentStateMachineDoc(std::string& game_instance_tree_item) const
{
    StateMachineDocumentMap& sm_docs = doc_.StateMachineDocuments();

    std::string game_tree_item, game_type_tree_item;
    if(!game_manager_dialog_bar_->GetCurrentSelection(game_tree_item, game_type_tree_item, game_instance_tree_item))
    {
        // Then nothing selected in the tree view
        return NULL;
    }

    if(game_type_tree_item != "State Machines")
    {
        // Then the wrong item in the tree view is selected for this operation
        return NULL;
    }

    StateMachineDocumentMap::iterator itor = sm_docs.find(game_instance_tree_item);
    if(itor == sm_docs.end())
    {
        // Then the there's no such item in our document with this name
        return NULL;
    }

    return &itor->second;
}

MyTest::ResourceContextDocument* SageStudioWindow::GetCurrentResourceContextDoc(std::string& game_instance_tree_item) const
{
    MyTest::ResourceContextDocumentMap& rc_docs = doc_.ResourceContextDocuments();

    std::string game_tree_item, game_type_tree_item;
    if(!game_manager_dialog_bar_->GetCurrentSelection(game_tree_item, game_type_tree_item, game_instance_tree_item))
    {
        // Then nothing selected in the tree view
        return NULL;
    }

    if(game_type_tree_item != "Resource Contexts")
    {
        // Then the wrong item in the tree view is selected for this operation
        return NULL;
    }

    MyTest::ResourceContextDocumentMap::iterator itor = rc_docs.find(game_instance_tree_item);
    if(itor == rc_docs.end())
    {
        // Then the there's no such item in our document with this name
        return NULL;
    }

    return &itor->second;
}

ImageDocument* SageStudioWindow::GetCurrentImageDoc(std::string& game_instance_tree_item) const
{
    ImageDocumentMap& img_docs = doc_.ImageDocuments();

    std::string game_tree_item, game_type_tree_item;
    if(!game_manager_dialog_bar_->GetCurrentSelection(game_tree_item, game_type_tree_item, game_instance_tree_item))
    {
        // Then nothing selected in the tree view
        return NULL;
    }

    if(game_type_tree_item != "Images")
    {
        // Then the wrong item in the tree view is selected for this operation
        return NULL;
    }

    ImageDocumentMap::iterator itor = img_docs.find(game_instance_tree_item);
    if(itor == img_docs.end())
    {
        // Then the there's no such item in our document with this name
        return NULL;
    }

    return &itor->second;
}

ImageAnimationDocument* SageStudioWindow::GetCurrentImageAnimationDoc(std::string& game_instance_tree_item) const
{
    ImageAnimationDocumentMap& img_anim_docs = doc_.ImageAnimationDocuments();

    std::string game_tree_item, game_type_tree_item;
    if(!game_manager_dialog_bar_->GetCurrentSelection(game_tree_item, game_type_tree_item, game_instance_tree_item))
    {
        // Then nothing selected in the tree view
        return NULL;
    }

    if(game_type_tree_item != "Image Animations")
    {
        // Then the wrong item in the tree view is selected for this operation
        return NULL;
    }

    ImageAnimationDocumentMap::iterator itor = img_anim_docs.find(game_instance_tree_item);
    if(itor == img_anim_docs.end())
    {
        // Then the there's no such item in our document with this name
        return NULL;
    }

    return &itor->second;
}

CameraDocument* SageStudioWindow::GetCurrentCameraDoc(std::string& game_instance_tree_item) const
{
    CameraDocumentMap& cam_docs = doc_.CameraDocuments();

    std::string game_tree_item, game_type_tree_item;
    if(!game_manager_dialog_bar_->GetCurrentSelection(game_tree_item, game_type_tree_item, game_instance_tree_item))
    {
        // Then nothing selected in the tree view
        return NULL;
    }

    if(game_type_tree_item != "Cameras")
    {
        // Then the wrong item in the tree view is selected for this operation
        return NULL;
    }

    CameraDocumentMap::iterator itor = cam_docs.find(game_instance_tree_item);
    if(itor == cam_docs.end())
    {
        // Then the there's no such item in our document with this name
        return NULL;
    }

    return &itor->second;
}

TextureSetDocument* SageStudioWindow::GetCurrentTextureSetDoc(std::string& game_instance_tree_item) const
{
    TextureSetDocumentMap& ts_docs = doc_.TextureSetDocuments();

    std::string game_tree_item, game_type_tree_item;
    if(!game_manager_dialog_bar_->GetCurrentSelection(game_tree_item, game_type_tree_item, game_instance_tree_item))
    {
        // Then nothing selected in the tree view
        return NULL;
    }

    if(game_type_tree_item != "Texture Sets")
    {
        // Then the wrong item in the tree view is selected for this operation
        return NULL;
    }

    TextureSetDocumentMap::iterator itor = ts_docs.find(game_instance_tree_item);
    if(itor == ts_docs.end())
    {
        // Then the there's no such item in our document with this name
        return NULL;
    }

    return &itor->second;
}

void SageStudioWindow::HideAllViews()
{
    empty_studio_window_->Hide();
    state_machine_window_->Hide();
    state_machine_tools_dialog_bar_->Hide();
    resource_context_dialog_bar_->Hide();
    image_window_->Hide();
    image_props_dialog_bar_->Hide();
    image_tools_dialog_bar_->Hide();
    cell_map_window_->Hide();
    cell_map_view_dialog_bar_->Hide();
    cell_map_props_dialog_bar_->Hide();
    default_bindings_dialog_bar_->Hide();
    texture_set_dialog_bar_->Hide();
    texture_set_manager_dialog_bar_->Hide();
    sky_dialog_bar_->Hide();
    lights_dialog_bar_->Hide();
}

void SageStudioWindow::SaveCurrentMap()
{
    if(cell_map_.Name().empty())
    {
        return;
    }

    std::ostringstream path;
    path << content_dir_ << "\\Maps\\" << cell_map_.Name() << ".cellmap";

    SaveMapProgressDialog dialog;
    dialog.SetCellMapDocument(&cell_map_);
    dialog.SetFilePath(path.str());

    if(!dialog.Run(instance_, handle_))
    {
        std::ostringstream message;
        message << "Couldn't save the map [" << path.str() << "]";
        MessageBox(handle_, message.str().c_str(), "Information", MB_OK | MB_ICONINFORMATION);
    }
}

void SageStudioWindow::LoadNewMap(const std::string& map_name)
{
    SaveCurrentMap();
    cell_map_.ClearAllCells();
    cell_map_.Name(std::string());

    std::ostringstream path;
    path << content_dir_ << "\\Maps\\" << map_name << ".cellmap";

    cell_map_.Name(map_name);

    LoadMapProgressDialog dialog;
    dialog.SetResourceContext(cell_map_window_->GetResCxt());
    dialog.SetTextureSetDocumentMap(&(doc_.TextureSetDocuments()));
    dialog.SetContentDir(content_dir_);
    dialog.SetCellMapDocument(&cell_map_);
    dialog.SetFilePath(path.str());

    if(!dialog.Run(instance_, handle_))
    {
        // Inform the user.
        std::ostringstream message;
        message << "Couldn't open the map [" << path.str() << "]";
        MessageBox(handle_, message.str().c_str(), "Information", MB_OK | MB_ICONINFORMATION);

        // Clear the map instance
        cell_map_.Name(std::string());

        // Remove it from the GUI
        CellMapDocumentList& cm2d_list = doc_.CellMapDocuments();
        CellMapDocumentList::iterator cm2d_itor = std::find(cm2d_list.begin(), cm2d_list.end(), map_name);
        if(cm2d_itor != cm2d_list.end())
        {
            cm2d_list.erase(cm2d_itor);
        }
        game_manager_dialog_bar_->RemoveMap(map_name);
        return;
    }
}

void SageStudioWindow::SynchroniseTextureSetResourceContexts()
{
    // Remove all textures from all resource contexts.
    MyTest::ResourceContextDocumentMap& rc_map = doc_.ResourceContextDocuments();
    MyTest::ResourceContextDocumentMap::iterator i;
    for(i = rc_map.begin(); i != rc_map.end(); ++i)
    {
        MyTest::ResourceContextDocument::iterator j = i->second.begin();
        while(j != i->second.end())
        {
            if(j->second.type_ == MyTest::RT_TEXTURE)
            {
                i->second.erase(j);
                j = i->second.begin();
            }
            else
            {
                ++j;
            }
        }
    }

    // Add all textures from all texture sets to their associated resource contexts.
    TextureSetDocumentMap& ts_map = doc_.TextureSetDocuments();
    TextureSetDocumentMap::iterator k;
    for(k = ts_map.begin(); k != ts_map.end(); ++k)
    {
        i = rc_map.find(k->second.ResCxt());
        if(i != rc_map.end())
        {
            TextureSetDocument& ts = k->second;
            MyTest::ResourceContextDocument& rc = i->second;

            MyTest::ResourceInfo ri;
            ri.type_            = MyTest::RT_TEXTURE;
            ri.list_box_item_   = -1;

            if(!ts.TexWall().empty())
            {
                rc.insert(std::make_pair(ts.TexWall(), ri));
            }
            if(!ts.TexCeiling().empty())
            {
                rc.insert(std::make_pair(ts.TexCeiling(), ri));
            }
            if(!ts.TexFloor().empty())
            {
                rc.insert(std::make_pair(ts.TexFloor(), ri));
            }
            if(!ts.TexLiquid().empty())
            {
                rc.insert(std::make_pair(ts.TexLiquid(), ri));
            }
            if(!ts.TexDoor().empty())
            {
                rc.insert(std::make_pair(ts.TexDoor(), ri));
            }
            if(!ts.TexTransWall().empty())
            {
                rc.insert(std::make_pair(ts.TexTransWall(), ri));
            }
            if(!ts.TexTransDoor().empty())
            {
                rc.insert(std::make_pair(ts.TexTransDoor(), ri));
            }
        }
    }
}

void SageStudioWindow::RecalcLighting(Light* l, bool repaint)
{
    typedef std::map<CellIndex, bool> AffectedCells;
    AffectedCells ac;
    int cell_area, x, z;
    CellIndex ci;

    // Build a container of cells that are affected by this light position change.
    ci.x_ = int(l->Position().x_ / cell_map_.CellSize());
    ci.z_ = int(l->Position().z_ / cell_map_.CellSize());

    cell_area = int(l->Radius() / cell_map_.CellSize()) + 1;
    for(z = ci.z_ - cell_area; z <= ci.z_ + cell_area; ++z)
    {
        for(x = ci.x_ - cell_area; x <= ci.x_ + cell_area; ++x)
        {
            // The call to insert won't insert duplicates
            ac.insert(std::make_pair(CellIndex(x, z), true));
        }
    }

    // Now that we have a list of the cells affected by this light position change,
    // we can calculate the new lighting values.
    AffectedCells::iterator ac_itor;
    for(ac_itor = ac.begin(); ac_itor != ac.end(); ++ac_itor)
    {
        cell_map_.CalculateLight(ac_itor->first, cell_map_.GetLightList());
    }

    if(repaint)
    {
        cell_map_window_->Repaint();
    }
}

void SageStudioWindow::OnBindTexturesTotal(int total)
{
}

void SageStudioWindow::OnBindTexturesStep(int value)
{
}

void SageStudioWindow::ShowEmptyStudioView()
{
    state_machine_window_->Hide();
    state_machine_tools_dialog_bar_->Hide();
    resource_context_dialog_bar_->Hide();
    image_window_->Hide();
    image_props_dialog_bar_->Hide();
    image_tools_dialog_bar_->Hide();
    cell_map_window_->Hide();
    cell_map_view_dialog_bar_->Hide();
    cell_map_props_dialog_bar_->Hide();
    default_bindings_dialog_bar_->Hide();
    texture_set_dialog_bar_->Hide();
    texture_set_manager_dialog_bar_->Hide();
    sky_dialog_bar_->Hide();
    lights_dialog_bar_->Hide();

    empty_studio_window_->Show();
    current_view_.clear();
    game_instance_tree_item_.clear();
}

void SageStudioWindow::ShowView(const std::string& view_name)
{
    current_view_ = view_name;
    if(view_name == "Entities")
    {
    }
    else if(view_name == "Cameras")
    {
        ShowEmptyStudioView();
    }
    else if(view_name == "Texture Sets")
    {
        if(!first_time_load_)
        {
            first_time_load_ = true;
            DoFirstTimeLoad();
        }

        texture_set_manager_dialog_bar_->ContentDirectory(content_dir_);
        texture_set_manager_dialog_bar_->DibResources(&dib_resources_);
        texture_set_manager_dialog_bar_->SetTextureSetDocument(GetCurrentTextureSetDoc(game_instance_tree_item_));
        texture_set_manager_dialog_bar_->Show();
    }
    else if(view_name == "Music")
    {
    }
    else if(view_name == "Default Bindings")
    {
        default_bindings_dialog_bar_->CameraDocuments(&(doc_.CameraDocuments()));
        default_bindings_dialog_bar_->BindingsDocument(&(doc_.BindingsDocument()));
        default_bindings_dialog_bar_->Show();
    }
    else if(view_name == "Triggers")
    {
    }
    else if(view_name == "Cell Maps")
    {
        if(cell_map_.Name() != game_instance_tree_item_ && !game_instance_tree_item_.empty())
        {
            LoadNewMap(game_instance_tree_item_);
        }

        texture_set_dialog_bar_->SetTextureSetDocumentMap(&(doc_.TextureSetDocuments()));

        cell_map_window_->Clear();
        cell_map_window_->SetCellMapDocument(&cell_map_);
        cell_map_window_->SkyTextureFilename(content_dir_, cell_map_.SkyTextureFilename());

        lights_dialog_bar_->Clear();
        LightList& ll = cell_map_.GetLightList();
        LightList::iterator ll_itor;
        for(ll_itor = ll.begin(); ll_itor != ll.end(); ++ll_itor)
        {
            lights_dialog_bar_->InsertLight(ll_itor->Name());
        }
        lights_dialog_bar_->SetWorldAmbient(cell_map_.AmbientLight());

        cell_map_window_->Show();
        cell_map_view_dialog_bar_->Show();
        switch(cell_map_window_->GetEditMode())
        {
        case EM_CELL:
            cell_map_props_dialog_bar_->Show();
            texture_set_dialog_bar_->Hide();
            sky_dialog_bar_->Hide();
            lights_dialog_bar_->Hide();
            break;
        case EM_TEXTURE_SET:
            cell_map_props_dialog_bar_->Hide();
            texture_set_dialog_bar_->Show();
            sky_dialog_bar_->Hide();
            lights_dialog_bar_->Hide();
            break;
        case EM_SOUND:
            break;
        case EM_LIGHT:
            cell_map_props_dialog_bar_->Hide();
            texture_set_dialog_bar_->Hide();
            sky_dialog_bar_->Hide();
            lights_dialog_bar_->Show();
            break;
        case EM_ENTITY:
            break;
        case EM_CAMERA:
            break;
        case EM_REGION:
            break;
        case EM_FOG:
            break;
        case EM_TRIGGER:
            break;
        case EM_SKY:
            cell_map_props_dialog_bar_->Hide();
            texture_set_dialog_bar_->Hide();
            sky_dialog_bar_->Show();
            lights_dialog_bar_->Hide();
            break;
        }
    }
    else if(view_name == "State Machines")
    {
        if(!first_time_load_)
        {
            first_time_load_ = true;
            DoFirstTimeLoad();
        }

        state_machine_window_->Clear();

        // Insert the data of the currently selected state machine into
        // the state machine window
        StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
        if(sm_doc)
        {
            SmEntryDocumentList& entry_docs = sm_doc->EntryDocuments();
            SmEntryDocumentList::iterator entry_itor;
            for(entry_itor = entry_docs.begin(); entry_itor != entry_docs.end(); ++entry_itor)
            {
                state_machine_window_->InsertEntryNode(entry_itor->Id(), entry_itor->Position(), false, false);
            }

            SmExitDocumentList& exit_docs = sm_doc->ExitDocuments();
            SmExitDocumentList::iterator exit_itor;
            for(exit_itor = exit_docs.begin(); exit_itor != exit_docs.end(); ++exit_itor)
            {
                state_machine_window_->InsertExitNode(exit_itor->Id(), exit_itor->Position(), false, false);
            }

            SmStateDocumentList& state_docs = sm_doc->StateDocuments();
            SmStateDocumentList::iterator state_itor;
            for(state_itor = state_docs.begin(); state_itor != state_docs.end(); ++state_itor)
            {
                state_machine_window_->InsertStateNode(state_itor->Id(), state_itor->Position(), state_itor->Name(), false, false);
            }

            SmEventDocumentList& event_docs = sm_doc->EventDocuments();
            SmEventDocumentList::iterator event_itor;
            for(event_itor = event_docs.begin(); event_itor != event_docs.end(); ++event_itor)
            {
                state_machine_window_->InsertEventNode(event_itor->Id(), event_itor->Position(), event_itor->Name(), event_itor->SourceNodeId(), event_itor->DestNodeId(), false, false);
            }
        }

        state_machine_window_->SetUniqueIdToHighestId();
        state_machine_window_->Show();
        state_machine_tools_dialog_bar_->Show();
    }
    else if(view_name == "Dialogs")
    {
    }
    else if(view_name == "Fonts")
    {
    }
    else if(view_name == "Resource Contexts")
    {
        if(!first_time_load_)
        {
            first_time_load_ = true;
            DoFirstTimeLoad();
        }

        resource_context_dialog_bar_->Clear();

        // Insert the data of the currently selected resource context into
        // the resource context dialog bar.
        resource_context_dialog_bar_->ContentDirectory(content_dir_);
        resource_context_dialog_bar_->DibResources(&dib_resources_);
        resource_context_dialog_bar_->ImageDocuments(&(doc_.ImageDocuments()));
        resource_context_dialog_bar_->ImageAnimationDocuments(&(doc_.ImageAnimationDocuments()));
        resource_context_dialog_bar_->ResourceContextDocument(GetCurrentResourceContextDoc(game_instance_tree_item_));

        resource_context_dialog_bar_->Show();
    }
    else if(view_name == "Images")
    {
        if(!first_time_load_)
        {
            first_time_load_ = true;
            DoFirstTimeLoad();
        }

        // Insert the currently selected image into the image window
        ImageDocument* img_doc = GetCurrentImageDoc(game_instance_tree_item_);
        if(img_doc)
        {
            image_window_->Clear();
            image_window_->UseFrameCount(false);
            image_window_->DibResources(&dib_resources_);

            image_props_dialog_bar_->DibResources(&dib_resources_);
            image_props_dialog_bar_->ContentDirectory(content_dir_);
            image_props_dialog_bar_->RefreshListBox();
            image_props_dialog_bar_->EnableFrameCount(false);

            POINT point;
            point.x = img_doc->FrameX();
            point.y = img_doc->FrameY();

            SIZE size;
            size.cx = img_doc->FrameWidth();
            size.cy = img_doc->FrameHeight();

            image_props_dialog_bar_->Texture(img_doc->TextureFilename());
            image_props_dialog_bar_->Position(point);
            image_props_dialog_bar_->Size(size);

            std::ostringstream full_path;
            full_path << content_dir_ << "\\Textures\\" << img_doc->TextureFilename();
            image_window_->Name(game_instance_tree_item_);
            image_window_->Texture(full_path.str());
            image_window_->Position(point);
            image_window_->Size(size);
        }

        image_window_->Show();
        image_props_dialog_bar_->Show();
        image_tools_dialog_bar_->Show();
    }
    else if(view_name == "Image Animations")
    {
        if(!first_time_load_)
        {
            first_time_load_ = true;
            DoFirstTimeLoad();
        }

        // Insert the currently selected image animation into the image window
        ImageAnimationDocument* img_anim_doc = GetCurrentImageAnimationDoc(game_instance_tree_item_);
        if(img_anim_doc)
        {
            image_window_->Clear();
            image_window_->UseFrameCount(true);
            image_window_->DibResources(&dib_resources_);

            image_props_dialog_bar_->DibResources(&dib_resources_);
            image_props_dialog_bar_->ContentDirectory(content_dir_);
            image_props_dialog_bar_->RefreshListBox();
            image_props_dialog_bar_->EnableFrameCount(true);

            POINT point;
            point.x = img_anim_doc->StartX();
            point.y = img_anim_doc->StartY();

            SIZE size;
            size.cx = img_anim_doc->FrameWidth();
            size.cy = img_anim_doc->FrameHeight();

            image_props_dialog_bar_->Texture(img_anim_doc->TextureFilename());
            image_props_dialog_bar_->Position(point);
            image_props_dialog_bar_->Size(size);
            image_props_dialog_bar_->FrameCount(img_anim_doc->FrameCount());

            std::ostringstream full_path;
            full_path << content_dir_ << "\\Textures\\" << img_anim_doc->TextureFilename();
            image_window_->Name(game_instance_tree_item_);
            image_window_->Texture(full_path.str());
            image_window_->Position(point);
            image_window_->Size(size);
            image_window_->FrameCount(img_anim_doc->FrameCount());
        }

        image_window_->Show();
        image_props_dialog_bar_->Show();
        image_tools_dialog_bar_->Show();
    }
    else if(view_name == "Entity Lists")
    {
    }
    else if(view_name == "Engine Parameters")
    {
    }
}

void SageStudioWindow::EnableView(const std::string& view_name, bool enable)
{
    if(view_name == "Entities")
    {
    }
    else if(view_name == "Texture Sets")
    {
        if(enable)
        {
            texture_set_manager_dialog_bar_->Enable();
        }
        else
        {
            texture_set_manager_dialog_bar_->Disable();
        }
    }
    else if(view_name == "Music")
    {
    }
    else if(view_name == "Default Bindings")
    {
        if(enable)
        {
            default_bindings_dialog_bar_->Enable();
        }
        else
        {
            default_bindings_dialog_bar_->Disable();
        }
    }
    else if(view_name == "Triggers")
    {
    }
    else if(view_name == "Cell Maps")
    {
        if(enable)
        {
            cell_map_window_->Enable();
            cell_map_view_dialog_bar_->Enable();
            cell_map_props_dialog_bar_->Enable();
            texture_set_dialog_bar_->Enable();
            sky_dialog_bar_->Enable();
            lights_dialog_bar_->Enable();
        }
        else
        {
            cell_map_window_->Disable();
            cell_map_view_dialog_bar_->Disable();
            cell_map_props_dialog_bar_->Disable();
            texture_set_dialog_bar_->Disable();
            sky_dialog_bar_->Disable();
            lights_dialog_bar_->Disable();
        }
    }
    else if(view_name == "State Machines")
    {
        if(enable)
        {
            state_machine_window_->Enable();
            state_machine_tools_dialog_bar_->Enable();
        }
        else
        {
            state_machine_window_->Disable();
            state_machine_tools_dialog_bar_->Disable();
        }
    }
    else if(view_name == "Dialogs")
    {
    }
    else if(view_name == "Fonts")
    {
    }
    else if(view_name == "Resource Contexts")
    {
        if(enable)
        {
            resource_context_dialog_bar_->Enable();
        }
        else
        {
            resource_context_dialog_bar_->Disable();
        }
    }
    else if(view_name == "Images")
    {
        if(enable)
        {
            image_window_->Enable();
            image_props_dialog_bar_->Enable();
            image_tools_dialog_bar_->Enable();
        }
        else
        {
            image_window_->Disable();
            image_props_dialog_bar_->Disable();
            image_tools_dialog_bar_->Disable();
        }
    }
    else if(view_name == "Image Animations")
    {
        if(enable)
        {
            image_window_->Enable();
            image_props_dialog_bar_->Enable();
            image_tools_dialog_bar_->Enable();
        }
        else
        {
            image_window_->Disable();
            image_props_dialog_bar_->Disable();
            image_tools_dialog_bar_->Disable();
        }
    }
    else if(view_name == "Entity Lists")
    {
    }
    else if(view_name == "Engine Parameters")
    {
    }
}

void SageStudioWindow::UpdateCurrentView()
{
    if(current_view_ == "Entities")
    {
    }
    else if(current_view_ == "Cameras")
    {
        ShowEmptyStudioView();
    }
    else if(current_view_ == "Texture Sets")
    {
        texture_set_manager_dialog_bar_->Clear();
        texture_set_manager_dialog_bar_->SetTextureSetDocument(GetCurrentTextureSetDoc(game_instance_tree_item_));
    }
    else if(current_view_ == "Music")
    {
    }
    else if(current_view_ == "Default Bindings")
    {
    }
    else if(current_view_ == "Triggers")
    {
    }
    else if(current_view_ == "Cell Maps")
    {
        if(cell_map_.Name() != game_instance_tree_item_ && !game_instance_tree_item_.empty())
        {
            LoadNewMap(game_instance_tree_item_);
        }

        cell_map_window_->Clear();
        cell_map_window_->SetCellMapDocument(&cell_map_);
        cell_map_window_->SkyTextureFilename(content_dir_, cell_map_.SkyTextureFilename());

        lights_dialog_bar_->Clear();
        LightList& ll = cell_map_.GetLightList();
        LightList::iterator ll_itor;
        for(ll_itor = ll.begin(); ll_itor != ll.end(); ++ll_itor)
        {
            lights_dialog_bar_->InsertLight(ll_itor->Name());
        }
        lights_dialog_bar_->SetWorldAmbient(cell_map_.AmbientLight());
    }
    else if(current_view_ == "State Machines")
    {
        state_machine_window_->Clear();

        // Insert the data of the currently selected state machine into
        // the state machine window
        StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
        if(sm_doc)
        {
            SmEntryDocumentList& entry_docs = sm_doc->EntryDocuments();
            SmEntryDocumentList::iterator entry_itor;
            for(entry_itor = entry_docs.begin(); entry_itor != entry_docs.end(); ++entry_itor)
            {
                state_machine_window_->InsertEntryNode(entry_itor->Id(), entry_itor->Position(), false, false);
            }

            SmExitDocumentList& exit_docs = sm_doc->ExitDocuments();
            SmExitDocumentList::iterator exit_itor;
            for(exit_itor = exit_docs.begin(); exit_itor != exit_docs.end(); ++exit_itor)
            {
                state_machine_window_->InsertExitNode(exit_itor->Id(), exit_itor->Position(), false, false);
            }

            SmStateDocumentList& state_docs = sm_doc->StateDocuments();
            SmStateDocumentList::iterator state_itor;
            for(state_itor = state_docs.begin(); state_itor != state_docs.end(); ++state_itor)
            {
                state_machine_window_->InsertStateNode(state_itor->Id(), state_itor->Position(), state_itor->Name(), false, false);
            }

            SmEventDocumentList& event_docs = sm_doc->EventDocuments();
            SmEventDocumentList::iterator event_itor;
            for(event_itor = event_docs.begin(); event_itor != event_docs.end(); ++event_itor)
            {
                state_machine_window_->InsertEventNode(event_itor->Id(), event_itor->Position(), event_itor->Name(), event_itor->SourceNodeId(), event_itor->DestNodeId(), false, false);
            }
        }
        state_machine_window_->SetUniqueIdToHighestId();
    }
    else if(current_view_ == "Dialogs")
    {
    }
    else if(current_view_ == "Fonts")
    {
    }
    else if(current_view_ == "Resource Contexts")
    {
        // Insert the data of the currently selected resource context into
        // the resource context dialog bar.
        resource_context_dialog_bar_->ResourceContextDocument(GetCurrentResourceContextDoc(game_instance_tree_item_));
    }
    else if(current_view_ == "Images")
    {
        // Insert the currently selected image into the image window
        ImageDocument* img_doc = GetCurrentImageDoc(game_instance_tree_item_);
        if(img_doc)
        {
            image_window_->Clear();

            POINT point;
            point.x = img_doc->FrameX();
            point.y = img_doc->FrameY();

            SIZE size;
            size.cx = img_doc->FrameWidth();
            size.cy = img_doc->FrameHeight();

            image_props_dialog_bar_->Texture(img_doc->TextureFilename());
            image_props_dialog_bar_->Position(point);
            image_props_dialog_bar_->Size(size);

            std::ostringstream full_path;
            full_path << content_dir_ << "\\Textures\\" << img_doc->TextureFilename();
            image_window_->Name(game_instance_tree_item_);
            image_window_->Texture(full_path.str());
            image_window_->Position(point);
            image_window_->Size(size);
        }
    }
    else if(current_view_ == "Image Animations")
    {
        // Insert the currently selected image animation into the image window
        ImageAnimationDocument* img_anim_doc = GetCurrentImageAnimationDoc(game_instance_tree_item_);
        if(img_anim_doc)
        {
            image_window_->Clear();

            POINT point;
            point.x = img_anim_doc->StartX();
            point.y = img_anim_doc->StartY();

            SIZE size;
            size.cx = img_anim_doc->FrameWidth();
            size.cy = img_anim_doc->FrameHeight();

            image_props_dialog_bar_->Texture(img_anim_doc->TextureFilename());
            image_props_dialog_bar_->Position(point);
            image_props_dialog_bar_->Size(size);
            image_props_dialog_bar_->FrameCount(img_anim_doc->FrameCount());

            std::ostringstream full_path;
            full_path << content_dir_ << "\\Textures\\" << img_anim_doc->TextureFilename();
            image_window_->Name(game_instance_tree_item_);
            image_window_->Texture(full_path.str());
            image_window_->Position(point);
            image_window_->Size(size);
            image_window_->FrameCount(img_anim_doc->FrameCount());
        }
    }
    else if(current_view_ == "Entity Lists")
    {
    }
    else if(current_view_ == "Engine Parameters")
    {
    }
}





LRESULT CALLBACK SageStudioWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(window, GWL_USERDATA);
    SageStudioWindow* this_ = reinterpret_cast<SageStudioWindow*>(data);

    switch(msg)
    {
    case WM_CREATE:
        {
            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(window, GWL_USERDATA, reinterpret_cast<LONG>(create_struct->lpCreateParams));
            this_ = reinterpret_cast<SageStudioWindow*>(create_struct->lpCreateParams);
            this_->handle_ = window;
            this_->OnWindowCreate();
            return 0;
        }
    case WM_CLOSE:
        if(!this_->OnFileExit())
        {
            return 0;       // We handled this message.
        }
        break;
    case WM_SIZE:
        this_->OnWindowSize();
        return 0;
    case WM_COMMAND:
        switch(LOWORD(wparam))
        {
        case ID_FILE_NEW40001:
            this_->OnFileNew();
            return 0;
        case ID_FILE_OPEN40002:
            this_->OnFileOpen();
            return 0;
        case ID_FILE_SAVE40003:
            this_->OnFileSave();
            return 0;
        case ID_FILE_SAVEAS:
            this_->OnFileSaveAs();
            return 0;
        case ID_FILE_CLOSE40005:
            this_->OnFileClose();
            return 0;
        case ID_FILE_COMPILEALLMAPS:
            this_->OnCompileAllMaps();
            return 0;
        case ID_FILE_EXIT:
            this_->OnFileExit();
            return 0;
        case ID_EDIT_INSERT:
            this_->OnEditInsert();
            return 0;
        case ID_EDIT_UNDO40007:
            this_->OnEditUndo();
            return 0;
        case ID_EDIT_REDO40008:
            this_->OnEditRedo();
            return 0;
        case ID_EDIT_CUT40009:
            this_->OnEditCut();
            return 0;
        case ID_EDIT_COPY40010:
            this_->OnEditCopy();
            return 0;
        case ID_EDIT_PASTE40011:
            this_->OnEditPaste();
            return 0;
        case ID_EDIT_DELETE:
            this_->OnEditDelete();
            return 0;
        case ID_EDIT_PROPERTIES:
            this_->OnEditProperties();
            return 0;
        case ID_VIEW_CONTENTDIRECTORYMANAGER:
            this_->OnViewContentDirectoryManager();
            return 0;
        }
        return 0;
    }

    return DefWindowProc(window, msg, wparam, lparam);
}





void SageStudioWindow::OnWindowCreate()
{
    char buffer[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, buffer);
    content_dir_ = buffer;

    audio_device_.Initialise(handle_, NULL, 22050, 16, 1);
}

bool SageStudioWindow::OnWindowClose()
{
    PostQuitMessage(0);
    return true;        // Let DefWindowProc() call DestroyWindow() for us.
}

void SageStudioWindow::OnWindowSize()
{
    if(game_manager_dialog_bar_ == NULL)
    {
        return;
    }

    game_manager_dialog_bar_->RepositionWithinParent();
    state_machine_window_->RepositionWithinParent();
    state_machine_tools_dialog_bar_->RepositionWithinParent();
    resource_context_dialog_bar_->RepositionWithinParent();
    image_window_->RepositionWithinParent();
    image_props_dialog_bar_->RepositionWithinParent();
    image_tools_dialog_bar_->RepositionWithinParent();
    empty_studio_window_->RepositionWithinParent();
    cell_map_window_->RepositionWithinParent();
    cell_map_view_dialog_bar_->RepositionWithinParent();
    cell_map_props_dialog_bar_->RepositionWithinParent();
    default_bindings_dialog_bar_->RepositionWithinParent();
    texture_set_dialog_bar_->RepositionWithinParent();
    texture_set_manager_dialog_bar_->RepositionWithinParent();
    sky_dialog_bar_->RepositionWithinParent();
    lights_dialog_bar_->RepositionWithinParent();
}

void SageStudioWindow::OnFileNew()
{
    if(!ModifiedCheck())
    {
        return;
    }

    GetTextDialog dialog;
    dialog.WindowTitle("New Game");
    dialog.Label("Please enter the title for this game");
    if(!dialog.Run(instance_, handle_))
    {
        return;
    }

    doc_.New(dialog.Text());
    state_machine_window_->Clear();

    EnableMenuItems();

    game_manager_dialog_bar_->UpdateGame(doc_.Name());
    empty_studio_window_->Show();
    game_manager_dialog_bar_->Enable();
    state_machine_window_->Hide();
    state_machine_tools_dialog_bar_->Hide();
    resource_context_dialog_bar_->Hide();
    image_window_->Hide();
    image_props_dialog_bar_->Hide();
    image_tools_dialog_bar_->Hide();
    cell_map_window_->Hide();
    cell_map_view_dialog_bar_->Hide();
    cell_map_props_dialog_bar_->Hide();
    default_bindings_dialog_bar_->Hide();
    texture_set_dialog_bar_->Hide();
    texture_set_manager_dialog_bar_->Hide();
    sky_dialog_bar_->Hide();
    lights_dialog_bar_->Hide();

    SetWindowTitle();
}

void SageStudioWindow::OnFileOpen()
{
    if(!ModifiedCheck())
    {
        return;
    }

    std::string filename(OpenFileDialog::Get(instance_, handle_, "SageStudio Game (*.ssg)\0*.ssg\0All Files (*.*)\0*.*\0"));
    if(filename.empty())
    {
        return;
    }

    DoFileClose();

    switch(doc_.Open(filename, content_dir_))
    {
    case GameDocument::OE_OK:
        break;
    case GameDocument::OE_OPEN_FAILURE:
        MessageBox(handle_, "That file doesn't exist", "Information", MB_OK | MB_ICONINFORMATION);
        return;
    case GameDocument::OE_INVALID_FILE:
        MessageBox(handle_, "That file is an invalid SageStudio Game file", "Information", MB_OK | MB_ICONINFORMATION);
        return;
    }

    // Insert file's contents into the tree view.
    game_manager_dialog_bar_->UpdateGame(doc_.Name());

    StateMachineDocumentMap& sm_docs = doc_.StateMachineDocuments();
    StateMachineDocumentMap::iterator sm_itor;
    for(sm_itor = sm_docs.begin(); sm_itor != sm_docs.end(); ++sm_itor)
    {
        game_manager_dialog_bar_->InsertStateMachine(sm_itor->first, false);
    }

    MyTest::ResourceContextDocumentMap& rc_docs = doc_.ResourceContextDocuments();
    MyTest::ResourceContextDocumentMap::iterator rc_itor;
    for(rc_itor = rc_docs.begin(); rc_itor != rc_docs.end(); ++rc_itor)
    {
        game_manager_dialog_bar_->InsertResourceContext(rc_itor->first, false);
    }

    ImageDocumentMap& img_docs = doc_.ImageDocuments();
    ImageDocumentMap::iterator img_itor;
    for(img_itor = img_docs.begin(); img_itor != img_docs.end(); ++img_itor)
    {
        game_manager_dialog_bar_->InsertImage(img_itor->first, false);
    }

    ImageAnimationDocumentMap& img_anim_docs = doc_.ImageAnimationDocuments();
    ImageAnimationDocumentMap::iterator img_anim_itor;
    for(img_anim_itor = img_anim_docs.begin(); img_anim_itor != img_anim_docs.end(); ++img_anim_itor)
    {
        game_manager_dialog_bar_->InsertImageAnimation(img_anim_itor->first, false);
    }

    CellMapDocumentList& cm2d_docs = doc_.CellMapDocuments();
    CellMapDocumentList::iterator cm2d_itor;
    for(cm2d_itor = cm2d_docs.begin(); cm2d_itor != cm2d_docs.end(); ++cm2d_itor)
    {
        game_manager_dialog_bar_->InsertMap(*cm2d_itor, false);
    }

    CameraDocumentMap& cam_docs = doc_.CameraDocuments();
    CameraDocumentMap::iterator cam_itor;
    for(cam_itor = cam_docs.begin(); cam_itor != cam_docs.end(); ++cam_itor)
    {
        game_manager_dialog_bar_->InsertCamera(cam_itor->first, false);
    }

    TextureSetDocumentMap& ts_docs = doc_.TextureSetDocuments();
    TextureSetDocumentMap::iterator ts_itor;
    for(ts_itor = ts_docs.begin(); ts_itor != ts_docs.end(); ++ts_itor)
    {
        game_manager_dialog_bar_->InsertTextureSet(ts_itor->first, false);
    }

    BindingOpenGLImagesDialog dialog;
    dialog.SetTextureSetDocumentMap(&(doc_.TextureSetDocuments()));
    dialog.SetResourceContext(cell_map_window_->GetResCxt());
    dialog.ContentDirectory(content_dir_);
    dialog.Run(instance_, handle_);

    EnableMenuItems();

    cell_map_window_->SkyTextureFilename(content_dir_, cell_map_.SkyTextureFilename());

    empty_studio_window_->Show();
    game_manager_dialog_bar_->Enable();
    state_machine_window_->Hide();
    state_machine_tools_dialog_bar_->Hide();
    resource_context_dialog_bar_->Hide();
    image_window_->Hide();

    SetWindowTitle();
}

void SageStudioWindow::OnFileSave()
{
    if(doc_.Filename().empty())
    {
        OnFileSaveAs();
        return;
    }

    DoFileSave(doc_.Filename());
}

void SageStudioWindow::OnFileSaveAs()
{
    std::string filename(SaveFileDialog::Get(instance_, handle_, "SageStudio Game (*.ssg)\0*.ssg\0All Files (*.*)\0*.*\0", "ssg", doc_.Name()));
    if(filename.empty())
    {
        return;
    }

    DoFileSave(filename);
}

void SageStudioWindow::OnFileClose()
{
    if(!ModifiedCheck())
    {
        return;
    }

    DoFileClose();
}

bool SageStudioWindow::OnFileExit()
{
    if(!ModifiedCheck())
    {
        return false;        // We don't want to exit
    }
    OnWindowClose();
    return true;
}

void SageStudioWindow::OnEditInsert()
{
}

void SageStudioWindow::OnEditUndo()
{
}

void SageStudioWindow::OnEditRedo()
{
}

void SageStudioWindow::OnEditCut()
{
}

void SageStudioWindow::OnEditCopy()
{
}

void SageStudioWindow::OnEditPaste()
{
}

void SageStudioWindow::OnEditDelete()
{
}

void SageStudioWindow::OnEditProperties()
{
}

void SageStudioWindow::OnViewContentDirectoryManager()
{
    ContentDirectoryDialog dialog;
    dialog.AudioDevice(&audio_device_);
    dialog.DibResources(&dib_resources_);
    dialog.ContentDirectory(content_dir_);
    dialog.Run(instance_, handle_);
}





void SageStudioWindow::OnStateMachineSelect()
{
    state_machine_window_->CurrentTool(StateMachineWindow::T_SELECT_NODE);
}

void SageStudioWindow::OnStateMachineInsertEntry()
{
    state_machine_window_->CurrentTool(StateMachineWindow::T_INSERT_ENTRY);
}

void SageStudioWindow::OnStateMachineInsertExit()
{
    state_machine_window_->CurrentTool(StateMachineWindow::T_INSERT_EXIT);
}

void SageStudioWindow::OnStateMachineInsertState()
{
    state_machine_window_->CurrentTool(StateMachineWindow::T_INSERT_STATE);
}

void SageStudioWindow::OnStateMachineInsertEvent()
{
    state_machine_window_->CurrentTool(StateMachineWindow::T_INSERT_EVENT);
}

void SageStudioWindow::OnStateMachineDelete()
{
    state_machine_window_->DeleteSelected();
}

void SageStudioWindow::OnStateMachineProperties()
{
    EntryNodeDrawingElement* entry_node;
    ExitNodeDrawingElement* exit_node;
    StateDrawingElement* state_node;
    EventDrawingElement* event_node;
    if(state_machine_window_->GetFirstSelectedDrawingElement(&entry_node, &exit_node, &state_node, &event_node))
    {
        if(entry_node)
        {
            MessageBox(handle_, "There are no properties for the Entry node", "Information", MB_OK | MB_ICONINFORMATION);
        }
        else if(exit_node)
        {
            MessageBox(handle_, "There are no properties for the Exit node", "Information", MB_OK | MB_ICONINFORMATION);
        }
        else if(state_node)
        {
            StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
            if(sm_doc != NULL)
            {
                StateNodePropertiesDialog dialog;
                dialog.Name(state_node->Text());
                dialog.SetImageAnimationDocumentMap(&(doc_.ImageAnimationDocuments()));
                dialog.SetImageDocumentMap(&(doc_.ImageDocuments()));
                dialog.DibResources(&dib_resources_);
                dialog.ContentDirectory(content_dir_);
                dialog.SetResourceContextDocumentMap(&(doc_.ResourceContextDocuments()));
                dialog.SetCellMapDocumentList(&(doc_.CellMapDocuments()));
                dialog.SetCameraDocuments(&(doc_.CameraDocuments()));

                SmStateDocumentList& state_docs = sm_doc->StateDocuments();
                SmStateDocumentList::iterator state_itor;
                SmStateDocument* state_doc = NULL;
                for(state_itor = state_docs.begin(); state_itor != state_docs.end(); ++state_itor)
                {
                    if(state_itor->Id() == state_node->Id())
                    {
                        state_doc = &(*state_itor);
                        break;
                    }
                }

                if(state_doc)
                {
                    dialog.SetStateDocument(state_doc);
                    if(dialog.Run(instance_, handle_))
                    {
                        SmStateDocumentList::iterator itor;
                        for(itor = sm_doc->StateDocuments().begin(); itor != sm_doc->StateDocuments().end(); ++itor)
                        {
                            if(itor->Id() == state_node->Id())
                            {
                                itor->Name(dialog.Name());
                                state_node->Text(dialog.Name());
                                state_machine_window_->Repaint();
                                OnStateMachineContentsModified();
                                break;
                            }
                        }
                    }
                }
            }
        }
        else if(event_node)
        {
            GetTextDialog dialog;
            dialog.WindowTitle("Event Properties");
            dialog.Label("Please enter a name for this Event");
            dialog.Text(event_node->Text());
            if(dialog.Run(instance_, handle_))
            {
                StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
                if(sm_doc != NULL)
                {
                    SmEventDocumentList::iterator itor;
                    for(itor = sm_doc->EventDocuments().begin(); itor != sm_doc->EventDocuments().end(); ++itor)
                    {
                        if(itor->Id() == event_node->Id())
                        {
                            itor->Name(dialog.Text());
                            event_node->Text(dialog.Text());
                            state_machine_window_->Repaint();
                            OnStateMachineContentsModified();
                            break;
                        }
                    }
                }
            }
        }
    }
}

void SageStudioWindow::OnStateMachineClearAll()
{
    StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
    if(sm_doc)
    {
        sm_doc->Clear();
    }
    state_machine_window_->Clear();
}





void SageStudioWindow::OnStateMachineContentsModified()
{
    doc_.Modified(true);
    SetWindowTitle();
    EnableMenuItems();
}

void SageStudioWindow::OnStateMachineDoubleClick()
{
    OnStateMachineProperties();
}

void SageStudioWindow::OnStateMachineEntryNodeInserted(int id, const POINT& position)
{
    StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
    if(sm_doc == NULL)
    {
        // Then no currently selected doc
        return;
    }
    SmEntryDocument entry_doc;
    entry_doc.Id(id);
    entry_doc.Position(position);
    sm_doc->EntryDocuments().push_back(entry_doc);
}

void SageStudioWindow::OnStateMachineEntryNodeRemoved(int id)
{
    StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
    if(sm_doc == NULL)
    {
        // Then no currently selected doc
        return;
    }
    SmEntryDocumentList::iterator itor;
    for(itor = sm_doc->EntryDocuments().begin(); itor != sm_doc->EntryDocuments().end(); ++itor)
    {
        if(itor->Id() == id)
        {
            sm_doc->EntryDocuments().erase(itor);
            break;
        }
    }
}

void SageStudioWindow::OnStateMachineEntryNodeUpdated(int id, const POINT& position)
{
    StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
    if(sm_doc == NULL)
    {
        // Then no currently selected doc
        return;
    }
    SmEntryDocumentList::iterator itor;
    for(itor = sm_doc->EntryDocuments().begin(); itor != sm_doc->EntryDocuments().end(); ++itor)
    {
        if(itor->Id() == id)
        {
            itor->Position(position);
            break;
        }
    }
}

void SageStudioWindow::OnStateMachineExitNodeInserted(int id, const POINT& position)
{
    StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
    if(sm_doc == NULL)
    {
        // Then no currently selected doc
        return;
    }
    SmExitDocument exit_doc;
    exit_doc.Id(id);
    exit_doc.Position(position);
    sm_doc->ExitDocuments().push_back(exit_doc);
}

void SageStudioWindow::OnStateMachineExitNodeRemoved(int id)
{
    StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
    if(sm_doc == NULL)
    {
        // Then no currently selected doc
        return;
    }
    SmExitDocumentList::iterator itor;
    for(itor = sm_doc->ExitDocuments().begin(); itor != sm_doc->ExitDocuments().end(); ++itor)
    {
        if(itor->Id() == id)
        {
            sm_doc->ExitDocuments().erase(itor);
            break;
        }
    }
}

void SageStudioWindow::OnStateMachineExitNodeUpdated(int id, const POINT& position)
{
    StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
    if(sm_doc == NULL)
    {
        // Then no currently selected doc
        return;
    }
    SmExitDocumentList::iterator itor;
    for(itor = sm_doc->ExitDocuments().begin(); itor != sm_doc->ExitDocuments().end(); ++itor)
    {
        if(itor->Id() == id)
        {
            itor->Position(position);
            break;
        }
    }
}

void SageStudioWindow::OnStateMachineStateNodeInserted(int id, const POINT& position, const std::string& name)
{
    StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
    if(sm_doc == NULL)
    {
        // Then no currently selected doc
        return;
    }
    SmStateDocument state_doc;
    state_doc.Id(id);
    state_doc.Position(position);
    state_doc.Name(name);
    sm_doc->StateDocuments().push_back(state_doc);
}

void SageStudioWindow::OnStateMachineStateNodeRemoved(int id)
{
    StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
    if(sm_doc == NULL)
    {
        // Then no currently selected doc
        return;
    }
    SmStateDocumentList::iterator itor;
    for(itor = sm_doc->StateDocuments().begin(); itor != sm_doc->StateDocuments().end(); ++itor)
    {
        if(itor->Id() == id)
        {
            sm_doc->StateDocuments().erase(itor);
            break;
        }
    }
}

void SageStudioWindow::OnStateMachineStateNodeUpdated(int id, const POINT& position, const std::string& name)
{
    StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
    if(sm_doc == NULL)
    {
        // Then no currently selected doc
        return;
    }
    SmStateDocumentList::iterator itor;
    for(itor = sm_doc->StateDocuments().begin(); itor != sm_doc->StateDocuments().end(); ++itor)
    {
        if(itor->Id() == id)
        {
            itor->Position(position);
            itor->Name(name);
            break;
        }
    }
}

void SageStudioWindow::OnStateMachineEventNodeInserted(int id, const POINT& position, const std::string& name, int source_id, int dest_id)
{
    StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
    if(sm_doc == NULL)
    {
        // Then no currently selected doc
        return;
    }
    SmEventDocument event_doc;
    event_doc.Id(id);
    event_doc.Position(position);
    event_doc.Name(name);
    event_doc.SourceNodeId(source_id);
    event_doc.DestNodeId(dest_id);
    sm_doc->EventDocuments().push_back(event_doc);
}

void SageStudioWindow::OnStateMachineEventNodeRemoved(int id)
{
    StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
    if(sm_doc == NULL)
    {
        // Then no currently selected doc
        return;
    }
    SmEventDocumentList::iterator itor;
    for(itor = sm_doc->EventDocuments().begin(); itor != sm_doc->EventDocuments().end(); ++itor)
    {
        if(itor->Id() == id)
        {
            sm_doc->EventDocuments().erase(itor);
            break;
        }
    }
}

void SageStudioWindow::OnStateMachineEventNodeUpdated(int id, const POINT& position, const std::string& name, int source_id, int dest_id)
{
    StateMachineDocument* sm_doc = GetCurrentStateMachineDoc(game_instance_tree_item_);
    if(sm_doc == NULL)
    {
        // Then no currently selected doc
        return;
    }
    SmEventDocumentList::iterator itor;
    for(itor = sm_doc->EventDocuments().begin(); itor != sm_doc->EventDocuments().end(); ++itor)
    {
        if(itor->Id() == id)
        {
            itor->Position(position);
            itor->Name(name);
            itor->SourceNodeId(source_id);
            itor->DestNodeId(dest_id);
            break;
        }
    }
}





void SageStudioWindow::OnTreeItemGameSelected(const std::string& game_tree_item)
{
    ShowEmptyStudioView();
}

void SageStudioWindow::OnTreeItemGameTypeSelected(const std::string& game_type_tree_item)
{
    if(game_type_tree_item == "Default Bindings")
    {
        game_instance_tree_item_.clear();

        HideAllViews();
        ShowView(game_type_tree_item);
        EnableView(current_view_, true);
    }
    else
    {
        ShowEmptyStudioView();
    }
}

void SageStudioWindow::OnTreeItemGameInstanceSelected(const std::string& game_type_tree_item, const std::string& game_instance_tree_item)
{
    if(game_instance_tree_item_.empty())
    {
        game_instance_tree_item_ = game_instance_tree_item;
    }

    if(current_view_ != game_type_tree_item)
    {
        game_instance_tree_item_ = game_instance_tree_item;

        HideAllViews();
        ShowView(game_type_tree_item);
    }
    else if(game_instance_tree_item_ != game_instance_tree_item)
    {
        game_instance_tree_item_ = game_instance_tree_item;
        UpdateCurrentView();
    }

    EnableView(current_view_, true);
}

void SageStudioWindow::OnTreeItemGameRightClicked(const std::string& game_tree_item)
{
    HMENU menu = LoadMenu(instance_, MAKEINTRESOURCE(IDR_GAME_MANAGER_POPUP));
    HMENU sub_menu = GetSubMenu(menu, 0);

    POINT pos;
    GetCursorPos(&pos);

    EnableMenuItem(sub_menu, ID_GAMEMANAGERPOPUP_INSERT, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(sub_menu, ID_GAMEMANAGERPOPUP_REMOVE, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(sub_menu, ID_GAMEMANAGERPOPUP_PROPERTIES, MF_BYCOMMAND | MF_ENABLED);

    int id = TrackPopupMenu(sub_menu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, pos.x, pos.y, 0, handle_, NULL);
    switch(id)
    {
    case ID_GAMEMANAGERPOPUP_PROPERTIES:
        OnPropertiesGame(game_tree_item);
        break;
    }
}

void SageStudioWindow::OnTreeItemGameTypeRightClicked(const std::string& game_type_tree_item)
{
    if(game_type_tree_item == "Default Bindings")
    {
        return; // No popup menu needed here.
    }

    HMENU menu = LoadMenu(instance_, MAKEINTRESOURCE(IDR_GAME_MANAGER_POPUP));
    HMENU sub_menu;
    
    if(game_type_tree_item == "Cell Maps")
    {
        sub_menu = GetSubMenu(menu, 1);
        EnableMenuItem(sub_menu, ID_CELLMAP_INSERT, MF_BYCOMMAND | MF_ENABLED);
        EnableMenuItem(sub_menu, ID_CELLMAP_REMOVE, MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem(sub_menu, ID_CELLMAP_PROPERTIES, MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem(sub_menu, ID_CELLMAP_COMPILEALL, MF_BYCOMMAND | MF_ENABLED);
        EnableMenuItem(sub_menu, ID_CELLMAP_COMPILE, MF_BYCOMMAND | MF_GRAYED);
    }
    else
    {
        sub_menu = GetSubMenu(menu, 0);
        EnableMenuItem(sub_menu, ID_GAMEMANAGERPOPUP_INSERT, MF_BYCOMMAND | MF_ENABLED);
        EnableMenuItem(sub_menu, ID_GAMEMANAGERPOPUP_REMOVE, MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem(sub_menu, ID_GAMEMANAGERPOPUP_PROPERTIES, MF_BYCOMMAND | MF_GRAYED);
    }

    POINT pos;
    GetCursorPos(&pos);

    int id = TrackPopupMenu(sub_menu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, pos.x, pos.y, 0, handle_, NULL);
    switch(id)
    {
    case ID_GAMEMANAGERPOPUP_INSERT:
        if(game_type_tree_item == "Entities") OnInsertEntity();
        else if(game_type_tree_item == "Cameras") OnInsertCamera();
        else if(game_type_tree_item == "Texture Sets") OnInsertTextureSet();
        else if(game_type_tree_item == "Music") OnInsertMusic();
        else if(game_type_tree_item == "Triggers") OnInsertTrigger();
        else if(game_type_tree_item == "Cell Maps") OnInsertMap();
        else if(game_type_tree_item == "State Machines") OnInsertStateMachine();
        else if(game_type_tree_item == "Dialogs") OnInsertDialog();
        else if(game_type_tree_item == "Fonts") OnInsertFont();
        else if(game_type_tree_item == "Resource Contexts") OnInsertResourceContext();
        else if(game_type_tree_item == "Images") OnInsertImage();
        else if(game_type_tree_item == "Image Animations") OnInsertImageAnimation();
        else if(game_type_tree_item == "Entity Lists") OnInsertEntityList();
        else if(game_type_tree_item == "Engine Parameters") OnInsertEngineParameter();
        break;
    case ID_CELLMAP_COMPILEALL:
        OnCompileAllMaps();
        break;
    }
}

void SageStudioWindow::OnTreeItemGameInstanceRightClicked(const std::string& game_type_tree_item, const std::string& game_instance_tree_item)
{
    if(game_type_tree_item == "Default Bindings")
    {
        return; // No popup menu needed here.
    }

    HMENU menu = LoadMenu(instance_, MAKEINTRESOURCE(IDR_GAME_MANAGER_POPUP));
    HMENU sub_menu;

    if(game_type_tree_item == "Cell Maps")
    {
        sub_menu = GetSubMenu(menu, 1);
        EnableMenuItem(sub_menu, ID_CELLMAP_INSERT, MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem(sub_menu, ID_CELLMAP_REMOVE, MF_BYCOMMAND | MF_ENABLED);
        EnableMenuItem(sub_menu, ID_CELLMAP_PROPERTIES, MF_BYCOMMAND | MF_ENABLED);
        EnableMenuItem(sub_menu, ID_CELLMAP_COMPILEALL, MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem(sub_menu, ID_CELLMAP_COMPILE, MF_BYCOMMAND | MF_ENABLED);
    }
    else
    {
        sub_menu = GetSubMenu(menu, 0);
        EnableMenuItem(sub_menu, ID_GAMEMANAGERPOPUP_INSERT, MF_BYCOMMAND | MF_GRAYED);
        EnableMenuItem(sub_menu, ID_GAMEMANAGERPOPUP_REMOVE, MF_BYCOMMAND | MF_ENABLED);
        EnableMenuItem(sub_menu, ID_GAMEMANAGERPOPUP_PROPERTIES, MF_BYCOMMAND | MF_ENABLED);
    }

    POINT pos;
    GetCursorPos(&pos);

    int id = TrackPopupMenu(sub_menu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, pos.x, pos.y, 0, handle_, NULL);
    switch(id)
    {
    case ID_GAMEMANAGERPOPUP_REMOVE:
    case ID_CELLMAP_REMOVE:
        if(game_type_tree_item == "Entities") OnRemoveEntity(game_instance_tree_item);
        else if(game_type_tree_item == "Cameras") OnRemoveCamera(game_instance_tree_item);
        else if(game_type_tree_item == "Texture Sets") OnRemoveTextureSet(game_instance_tree_item);
        else if(game_type_tree_item == "Music") OnRemoveMusic(game_instance_tree_item);
        else if(game_type_tree_item == "Triggers") OnRemoveTrigger(game_instance_tree_item);
        else if(game_type_tree_item == "Cell Maps") OnRemoveMap(game_instance_tree_item);
        else if(game_type_tree_item == "State Machines") OnRemoveStateMachine(game_instance_tree_item);
        else if(game_type_tree_item == "Dialogs") OnRemoveDialog(game_instance_tree_item);
        else if(game_type_tree_item == "Fonts") OnRemoveFont(game_instance_tree_item);
        else if(game_type_tree_item == "Resource Contexts") OnRemoveResourceContext(game_instance_tree_item);
        else if(game_type_tree_item == "Images") OnRemoveImage(game_instance_tree_item);
        else if(game_type_tree_item == "Image Animations") OnRemoveImageAnimation(game_instance_tree_item);
        else if(game_type_tree_item == "Entity Lists") OnRemoveEntityList(game_instance_tree_item);
        else if(game_type_tree_item == "Engine Parameters") OnRemoveEngineParameter(game_instance_tree_item);
        break;
    case ID_GAMEMANAGERPOPUP_PROPERTIES:
    case ID_CELLMAP_PROPERTIES:
        if(game_type_tree_item == "Entities") OnPropertiesEntity(game_instance_tree_item);
        else if(game_type_tree_item == "Cameras") OnPropertiesCamera(game_instance_tree_item);
        else if(game_type_tree_item == "Texture Sets") OnPropertiesTextureSet(game_instance_tree_item);
        else if(game_type_tree_item == "Music") OnPropertiesMusic(game_instance_tree_item);
        else if(game_type_tree_item == "Triggers") OnPropertiesTrigger(game_instance_tree_item);
        else if(game_type_tree_item == "Cell Maps") OnPropertiesMap(game_instance_tree_item);
        else if(game_type_tree_item == "State Machines") OnPropertiesStateMachine(game_instance_tree_item);
        else if(game_type_tree_item == "Dialogs") OnPropertiesDialog(game_instance_tree_item);
        else if(game_type_tree_item == "Fonts") OnPropertiesFont(game_instance_tree_item);
        else if(game_type_tree_item == "Resource Contexts") OnPropertiesResourceContext(game_instance_tree_item);
        else if(game_type_tree_item == "Images") OnPropertiesImage(game_instance_tree_item);
        else if(game_type_tree_item == "Image Animations") OnPropertiesImageAnimation(game_instance_tree_item);
        else if(game_type_tree_item == "Entity Lists") OnPropertiesEntityList(game_instance_tree_item);
        else if(game_type_tree_item == "Engine Parameters") OnPropertiesEngineParameter(game_instance_tree_item);
        break;
    case ID_CELLMAP_COMPILE:
        OnCompileMap();
        break;
    }
}

void SageStudioWindow::OnInsertEntity()
{
    GetTextDialog dialog;
    dialog.WindowTitle("Insert New Entity");
    dialog.Label("Enter the name of this new Entity");
    if(dialog.Run(instance_, handle_))
    {
        game_manager_dialog_bar_->InsertEntity(dialog.Text());
        OnStateMachineContentsModified();
        UpdateCurrentView();
    }
}

void SageStudioWindow::OnInsertCamera()
{
    CameraPropertiesDialog d;
    if(d.Run(instance_, handle_))
    {
        CameraDocumentMap& cam_map = doc_.CameraDocuments();
        CameraDocumentMap::iterator cam_itor = cam_map.find(d.Name());
        if(cam_itor != cam_map.end())
        {
            MessageBox(handle_, "A Camera with that name already exists", "Information", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            CameraDocument c;
            c.Name(d.Name());
            c.Position(d.Position());
            c.Rotation(d.Rotation());
            cam_map.insert(std::make_pair(d.Name(), c));
            game_manager_dialog_bar_->InsertCamera(d.Name());
            resource_context_dialog_bar_->Clear();
            OnStateMachineContentsModified();
            UpdateCurrentView();
        }
    }
}

void SageStudioWindow::OnInsertTextureSet()
{
    TextureSetPropertiesDialog dialog;
    dialog.SetResourceContextDocumentMap(&(doc_.ResourceContextDocuments()));
    if(dialog.Run(instance_, handle_))
    {
        TextureSetDocumentMap& ts_map = doc_.TextureSetDocuments();
        TextureSetDocumentMap::iterator ts_itor = ts_map.find(dialog.Name());
        if(ts_itor != ts_map.end())
        {
            MessageBox(handle_, "A Texture Set with that name already exists", "Information", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            TextureSetDocument ts;
            ts.ResCxt(dialog.ResCxt());
            ts_map.insert(std::make_pair(dialog.Name(), ts));
            game_manager_dialog_bar_->InsertTextureSet(dialog.Name());
            resource_context_dialog_bar_->Clear();
            OnStateMachineContentsModified();
            UpdateCurrentView();
        }
    }
}

void SageStudioWindow::OnInsertMusic()
{
    GetTextDialog dialog;
    dialog.WindowTitle("Insert New Music");
    dialog.Label("Enter the name of this new Music");
    if(dialog.Run(instance_, handle_))
    {
        game_manager_dialog_bar_->InsertMusic(dialog.Text());
        OnStateMachineContentsModified();
        UpdateCurrentView();
    }
}

void SageStudioWindow::OnInsertTrigger()
{
    GetTextDialog dialog;
    dialog.WindowTitle("Insert New Trigger");
    dialog.Label("Enter the name of this new Trigger");
    if(dialog.Run(instance_, handle_))
    {
        game_manager_dialog_bar_->InsertTrigger(dialog.Text());
        OnStateMachineContentsModified();
        UpdateCurrentView();
    }
}

void SageStudioWindow::OnInsertMap()
{
    GetTextDialog dialog;
    dialog.WindowTitle("Insert New Map");
    dialog.Label("Enter the name of this new Map");
    if(dialog.Run(instance_, handle_))
    {
        CellMapDocumentList& cm2d_list = doc_.CellMapDocuments();
        CellMapDocumentList::iterator cm2d_itor = std::find(cm2d_list.begin(), cm2d_list.end(), dialog.Text());
        if(cm2d_itor != cm2d_list.end())
        {
            MessageBox(handle_, "A CellMap with that name already exists", "Information", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            cell_map_.ClearAllCells();
            cell_map_.Name(dialog.Text());
            SaveCurrentMap();

            cell_map_.BuildTextureSetMap(cell_map_window_->GetResCxt(), &(doc_.TextureSetDocuments()), content_dir_);

            cm2d_list.push_back(dialog.Text());
            game_manager_dialog_bar_->InsertMap(dialog.Text());
            cell_map_window_->Clear();
            OnStateMachineContentsModified();
            UpdateCurrentView();
        }
    }
}

void SageStudioWindow::OnInsertStateMachine()
{
    GetTextDialog dialog;
    dialog.WindowTitle("Insert New StateMachine");
    dialog.Label("Enter the name of this new StateMachine");
    if(dialog.Run(instance_, handle_))
    {
        StateMachineDocumentMap& sm_map = doc_.StateMachineDocuments();
        StateMachineDocumentMap::iterator sm_itor = sm_map.find(dialog.Text());
        if(sm_itor != sm_map.end())
        {
            MessageBox(handle_, "A StateMachine with that name already exists", "Information", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            sm_map.insert(std::make_pair(dialog.Text(), StateMachineDocument()));
            game_manager_dialog_bar_->InsertStateMachine(dialog.Text());
            state_machine_window_->Clear();
            OnStateMachineContentsModified();
            UpdateCurrentView();
        }
    }
}

void SageStudioWindow::OnInsertDialog()
{
    GetTextDialog dialog;
    dialog.WindowTitle("Insert New Dialog");
    dialog.Label("Enter the name of this new Dialog");
    if(dialog.Run(instance_, handle_))
    {
        game_manager_dialog_bar_->InsertDialog(dialog.Text());
        OnStateMachineContentsModified();
        UpdateCurrentView();
    }
}

void SageStudioWindow::OnInsertFont()
{
    GetTextDialog dialog;
    dialog.WindowTitle("Insert New Font");
    dialog.Label("Enter the name of this new Font");
    if(dialog.Run(instance_, handle_))
    {
        game_manager_dialog_bar_->InsertFont(dialog.Text());
        OnStateMachineContentsModified();
        UpdateCurrentView();
    }
}

void SageStudioWindow::OnInsertResourceContext()
{
    GetTextDialog dialog;
    dialog.WindowTitle("Insert New ResourceContext");
    dialog.Label("Enter the name of this new ResourceContext");
    if(dialog.Run(instance_, handle_))
    {
        MyTest::ResourceContextDocumentMap& rc_map = doc_.ResourceContextDocuments();
        MyTest::ResourceContextDocumentMap::iterator rc_itor = rc_map.find(dialog.Text());
        if(rc_itor != rc_map.end())
        {
            MessageBox(handle_, "A ResourceContext with that name already exists", "Information", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            rc_map.insert(std::make_pair(dialog.Text(), MyTest::ResourceContextDocument()));
            game_manager_dialog_bar_->InsertResourceContext(dialog.Text());
            resource_context_dialog_bar_->Clear();
            OnStateMachineContentsModified();
            UpdateCurrentView();
        }
    }
}

void SageStudioWindow::OnInsertImage()
{
    GetTextDialog dialog;
    dialog.WindowTitle("Insert New Image");
    dialog.Label("Enter the name of this new Image");
    if(dialog.Run(instance_, handle_))
    {
        ImageDocumentMap& img_map = doc_.ImageDocuments();
        ImageDocumentMap::iterator img_itor = img_map.find(dialog.Text());
        if(img_itor != img_map.end())
        {
            MessageBox(handle_, "A Image with that name already exists", "Information", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            img_map.insert(std::make_pair(dialog.Text(), ImageDocument()));
            game_manager_dialog_bar_->InsertImage(dialog.Text());
            resource_context_dialog_bar_->Clear();
            OnStateMachineContentsModified();
            UpdateCurrentView();
        }
    }
}

void SageStudioWindow::OnInsertImageAnimation()
{
    GetTextDialog dialog;
    dialog.WindowTitle("Insert New ImageAnimation");
    dialog.Label("Enter the name of this new ImageAnimation");
    if(dialog.Run(instance_, handle_))
    {
        ImageAnimationDocumentMap& img_map = doc_.ImageAnimationDocuments();
        ImageAnimationDocumentMap::iterator img_itor = img_map.find(dialog.Text());
        if(img_itor != img_map.end())
        {
            MessageBox(handle_, "A ImageAnimation with that name already exists", "Information", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            img_map.insert(std::make_pair(dialog.Text(), ImageAnimationDocument()));
            game_manager_dialog_bar_->InsertImageAnimation(dialog.Text());
            resource_context_dialog_bar_->Clear();
            OnStateMachineContentsModified();
            UpdateCurrentView();
        }
    }
}

void SageStudioWindow::OnInsertEntityList()
{
    GetTextDialog dialog;
    dialog.WindowTitle("Insert New EntityList");
    dialog.Label("Enter the name of this new EntityList");
    if(dialog.Run(instance_, handle_))
    {
        game_manager_dialog_bar_->InsertEntityList(dialog.Text());
        OnStateMachineContentsModified();
        UpdateCurrentView();
    }
}

void SageStudioWindow::OnInsertEngineParameter()
{
    GetTextDialog dialog;
    dialog.WindowTitle("Insert New EngineParameter");
    dialog.Label("Enter the name of this new EngineParameter");
    if(dialog.Run(instance_, handle_))
    {
        game_manager_dialog_bar_->InsertEngineParameter(dialog.Text());
        OnStateMachineContentsModified();
        UpdateCurrentView();
    }
}

void SageStudioWindow::OnRemoveEntity(const std::string& game_instance_tree_item)
{
    if(MessageBox(handle_, "Are you sure you want to remove this Entity?", "Confirmation", MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        game_manager_dialog_bar_->RemoveEntity(game_instance_tree_item);
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnRemoveCamera(const std::string& game_instance_tree_item)
{
    if(MessageBox(handle_, "Are you sure you want to remove this Camera?", "Confirmation", MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        CameraDocumentMap& cam_map = doc_.CameraDocuments();
        CameraDocumentMap::iterator cam_itor = cam_map.find(game_instance_tree_item);
        if(cam_itor != cam_map.end())
        {
            cam_map.erase(cam_itor);
        }
        game_manager_dialog_bar_->RemoveCamera(game_instance_tree_item);
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnRemoveTextureSet(const std::string& game_instance_tree_item)
{
    if(MessageBox(handle_, "Are you sure you want to remove this Texture Set?", "Confirmation", MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        TextureSetDocumentMap& ts_map = doc_.TextureSetDocuments();
        TextureSetDocumentMap::iterator ts_itor = ts_map.find(game_instance_tree_item);
        if(ts_itor != ts_map.end())
        {
            ts_map.erase(ts_itor);
        }
        game_manager_dialog_bar_->RemoveTextureSet(game_instance_tree_item);
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnRemoveMusic(const std::string& game_instance_tree_item)
{
    if(MessageBox(handle_, "Are you sure you want to remove this Music?", "Confirmation", MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        game_manager_dialog_bar_->RemoveMusic(game_instance_tree_item);
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnRemoveTrigger(const std::string& game_instance_tree_item)
{
    if(MessageBox(handle_, "Are you sure you want to remove this Trigger?", "Confirmation", MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        game_manager_dialog_bar_->RemoveTrigger(game_instance_tree_item);
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnRemoveMap(const std::string& game_instance_tree_item)
{
    if(MessageBox(handle_, "Are you sure you want to remove this Map?", "Confirmation", MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        CellMapDocumentList& cm2d_list = doc_.CellMapDocuments();
        CellMapDocumentList::iterator cm2d_itor = std::find(cm2d_list.begin(), cm2d_list.end(), game_instance_tree_item);
        if(cm2d_itor != cm2d_list.end())
        {
            cm2d_list.erase(cm2d_itor);
        }
        game_manager_dialog_bar_->RemoveMap(game_instance_tree_item);
        cell_map_.ClearAllCells();
        cell_map_.Name(std::string());
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnRemoveStateMachine(const std::string& game_instance_tree_item)
{
    if(MessageBox(handle_, "Are you sure you want to remove this StateMachine?", "Confirmation", MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        StateMachineDocumentMap& sm_map = doc_.StateMachineDocuments();
        StateMachineDocumentMap::iterator sm_itor = sm_map.find(game_instance_tree_item);
        if(sm_itor != sm_map.end())
        {
            sm_map.erase(sm_itor);
        }
        game_manager_dialog_bar_->RemoveStateMachine(game_instance_tree_item);
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnRemoveDialog(const std::string& game_instance_tree_item)
{
    if(MessageBox(handle_, "Are you sure you want to remove this Dialog?", "Confirmation", MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        game_manager_dialog_bar_->RemoveDialog(game_instance_tree_item);
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnRemoveFont(const std::string& game_instance_tree_item)
{
    if(MessageBox(handle_, "Are you sure you want to remove this Font?", "Confirmation", MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        game_manager_dialog_bar_->RemoveFont(game_instance_tree_item);
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnRemoveResourceContext(const std::string& game_instance_tree_item)
{
    if(MessageBox(handle_, "Are you sure you want to remove this ResourceContext?", "Confirmation", MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        MyTest::ResourceContextDocumentMap& rc_map = doc_.ResourceContextDocuments();
        MyTest::ResourceContextDocumentMap::iterator rc_itor = rc_map.find(game_instance_tree_item);
        if(rc_itor != rc_map.end())
        {
            rc_map.erase(rc_itor);
        }
        game_manager_dialog_bar_->RemoveResourceContext(game_instance_tree_item);
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnRemoveImage(const std::string& game_instance_tree_item)
{
    if(MessageBox(handle_, "Are you sure you want to remove this Image?", "Confirmation", MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        ImageDocumentMap& img_map = doc_.ImageDocuments();
        ImageDocumentMap::iterator img_itor = img_map.find(game_instance_tree_item);
        if(img_itor != img_map.end())
        {
            img_map.erase(img_itor);
        }
        game_manager_dialog_bar_->RemoveImage(game_instance_tree_item);
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnRemoveImageAnimation(const std::string& game_instance_tree_item)
{
    if(MessageBox(handle_, "Are you sure you want to remove this ImageAnimation?", "Confirmation", MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        ImageAnimationDocumentMap& img_anim_map = doc_.ImageAnimationDocuments();
        ImageAnimationDocumentMap::iterator img_anim_itor = img_anim_map.find(game_instance_tree_item);
        if(img_anim_itor != img_anim_map.end())
        {
            img_anim_map.erase(img_anim_itor);
        }
        game_manager_dialog_bar_->RemoveImageAnimation(game_instance_tree_item);
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnRemoveEntityList(const std::string& game_instance_tree_item)
{
    if(MessageBox(handle_, "Are you sure you want to remove this EntityList?", "Confirmation", MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        game_manager_dialog_bar_->RemoveEntityList(game_instance_tree_item);
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnRemoveEngineParameter(const std::string& game_instance_tree_item)
{
    if(MessageBox(handle_, "Are you sure you want to remove this EngineParameter?", "Confirmation", MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
        game_manager_dialog_bar_->RemoveEngineParameter(game_instance_tree_item);
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnPropertiesGame(const std::string& game_tree_item)
{
    GamePropertiesDialog dialog;
    dialog.SetStateMachineDocumentMap(&(doc_.StateMachineDocuments()));
    dialog.Name(game_tree_item);
    dialog.StateMachine(doc_.StateMachine());
    dialog.ClearBackBuffer(doc_.ClearBackBuffer());
    if(dialog.Run(instance_, handle_))
    {
        doc_.Name(dialog.Name());
        doc_.StateMachine(dialog.StateMachine());
        doc_.ClearBackBuffer(dialog.ClearBackBuffer());

        game_manager_dialog_bar_->UpdateGame(dialog.Name());
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnPropertiesEntity(const std::string& game_instance_tree_item)
{
    GetTextDialog dialog;
    dialog.WindowTitle("Entity Properties");
    dialog.Label("Name:");
    dialog.Text(game_instance_tree_item);
    if(dialog.Run(instance_, handle_))
    {
        game_manager_dialog_bar_->UpdateEntity(game_instance_tree_item, dialog.Text());
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnPropertiesCamera(const std::string& item)
{
    CameraDocumentMap& docs = doc_.CameraDocuments();
    CameraDocumentMap::iterator i = docs.find(item);
    CameraDocument& c = i->second;

    CameraPropertiesDialog d;
    d.Name(c.Name());
    d.Position(c.Position());
    d.Rotation(c.Rotation());
    if(d.Run(instance_, handle_))
    {
        docs.erase(i);

        CameraDocument n;
        n.Name(d.Name());
        n.Position(d.Position());
        n.Rotation(d.Rotation());
        docs.insert(std::make_pair(d.Name(), n));

        game_manager_dialog_bar_->UpdateCamera(item, d.Name());
        OnStateMachineContentsModified();

        doc_.UpdateResourceContexts(item, d.Name());
    }
}

void SageStudioWindow::OnPropertiesTextureSet(const std::string& game_instance_tree_item)
{
    TextureSetDocumentMap& ts_map = doc_.TextureSetDocuments();
    TextureSetDocumentMap::iterator ts_itor = ts_map.find(game_instance_tree_item);

    TextureSetPropertiesDialog dialog;
    dialog.SetResourceContextDocumentMap(&(doc_.ResourceContextDocuments()));
    dialog.Name(game_instance_tree_item);
    dialog.ResCxt(ts_itor->second.ResCxt());
    if(dialog.Run(instance_, handle_))
    {
        TextureSetDocumentMap::iterator temp = ts_map.find(dialog.Name());
        if(temp != ts_map.end() && game_instance_tree_item != dialog.Name())
        {
            MessageBox(handle_, "A TextureSet with that name already exists", "Information", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            TextureSetDocument ts = ts_itor->second;
            ts_map.erase(ts_itor);

            ts.ResCxt(dialog.ResCxt());
            ts_map.insert(std::make_pair(dialog.Name(), ts));

            game_manager_dialog_bar_->UpdateMap(game_instance_tree_item, dialog.Name());
            OnStateMachineContentsModified();
        }
    }
}

void SageStudioWindow::OnPropertiesMusic(const std::string& game_instance_tree_item)
{
    GetTextDialog dialog;
    dialog.WindowTitle("Music Properties");
    dialog.Label("Name:");
    dialog.Text(game_instance_tree_item);
    if(dialog.Run(instance_, handle_))
    {
        game_manager_dialog_bar_->UpdateMusic(game_instance_tree_item, dialog.Text());
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnPropertiesTrigger(const std::string& game_instance_tree_item)
{
    GetTextDialog dialog;
    dialog.WindowTitle("Trigger Properties");
    dialog.Label("Name:");
    dialog.Text(game_instance_tree_item);
    if(dialog.Run(instance_, handle_))
    {
        game_manager_dialog_bar_->UpdateTrigger(game_instance_tree_item, dialog.Text());
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnPropertiesMap(const std::string& game_instance_tree_item)
{
    GetTextDialog dialog;
    dialog.WindowTitle("Map Properties");
    dialog.Label("Name:");
    dialog.Text(game_instance_tree_item);
    if(dialog.Run(instance_, handle_))
    {
        CellMapDocumentList& cm2d_list = doc_.CellMapDocuments();
        CellMapDocumentList::iterator cm2d_itor = std::find(cm2d_list.begin(), cm2d_list.end(), dialog.Text());
        if(cm2d_itor != cm2d_list.end())
        {
            MessageBox(handle_, "A CellMap with that name already exists", "Information", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            cm2d_itor = std::find(cm2d_list.begin(), cm2d_list.end(), game_instance_tree_item);
            if(cm2d_itor != cm2d_list.end())
            {
                cm2d_list.erase(cm2d_itor);
                cm2d_list.push_back(dialog.Text());
                game_manager_dialog_bar_->UpdateMap(game_instance_tree_item, dialog.Text());
                OnStateMachineContentsModified();
            }
        }
    }
}

void SageStudioWindow::OnPropertiesStateMachine(const std::string& game_instance_tree_item)
{
    GetTextDialog dialog;
    dialog.WindowTitle("StateMachine Properties");
    dialog.Label("Name:");
    dialog.Text(game_instance_tree_item);
    if(dialog.Run(instance_, handle_))
    {
        StateMachineDocumentMap& sm_map = doc_.StateMachineDocuments();
        StateMachineDocumentMap::iterator sm_itor = sm_map.find(dialog.Text());
        if(sm_itor != sm_map.end())
        {
            MessageBox(handle_, "A StateMachine with that name already exists", "Information", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            sm_itor = sm_map.find(game_instance_tree_item);
            if(sm_itor != sm_map.end())
            {
                StateMachineDocument copy = sm_itor->second;
                sm_map.erase(sm_itor);
                sm_map.insert(std::make_pair(dialog.Text(), copy));
                game_manager_dialog_bar_->UpdateStateMachine(game_instance_tree_item, dialog.Text());
                OnStateMachineContentsModified();
            }
        }
    }
}

void SageStudioWindow::OnPropertiesDialog(const std::string& game_instance_tree_item)
{
    GetTextDialog dialog;
    dialog.WindowTitle("Dialog Properties");
    dialog.Label("Name:");
    dialog.Text(game_instance_tree_item);
    if(dialog.Run(instance_, handle_))
    {
        game_manager_dialog_bar_->UpdateDialog(game_instance_tree_item, dialog.Text());
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnPropertiesFont(const std::string& game_instance_tree_item)
{
    GetTextDialog dialog;
    dialog.WindowTitle("Font Properties");
    dialog.Label("Name:");
    dialog.Text(game_instance_tree_item);
    if(dialog.Run(instance_, handle_))
    {
        game_manager_dialog_bar_->UpdateFont(game_instance_tree_item, dialog.Text());
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnPropertiesResourceContext(const std::string& game_instance_tree_item)
{
    GetTextDialog dialog;
    dialog.WindowTitle("ResourceContext Properties");
    dialog.Label("Name:");
    dialog.Text(game_instance_tree_item);
    if(dialog.Run(instance_, handle_))
    {
        MyTest::ResourceContextDocumentMap& rc_map = doc_.ResourceContextDocuments();
        MyTest::ResourceContextDocumentMap::iterator rc_itor = rc_map.find(dialog.Text());
        if(rc_itor != rc_map.end())
        {
            MessageBox(handle_, "A ResourceContext with that name already exists", "Information", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            rc_itor = rc_map.find(game_instance_tree_item);
            if(rc_itor != rc_map.end())
            {
                MyTest::ResourceContextDocument copy = rc_itor->second;
                rc_map.erase(rc_itor);
                rc_map.insert(std::make_pair(dialog.Text(), copy));
                game_manager_dialog_bar_->UpdateResourceContext(game_instance_tree_item, dialog.Text());
                resource_context_dialog_bar_->ResourceContextDocument(GetCurrentResourceContextDoc(game_instance_tree_item_));
                OnStateMachineContentsModified();
            }
        }
    }
}

void SageStudioWindow::OnPropertiesImage(const std::string& game_instance_tree_item)
{
    GetTextDialog dialog;
    dialog.WindowTitle("Image Properties");
    dialog.Label("Name:");
    dialog.Text(game_instance_tree_item);
    if(dialog.Run(instance_, handle_))
    {
        ImageDocumentMap& img_map = doc_.ImageDocuments();
        ImageDocumentMap::iterator img_itor = img_map.find(dialog.Text());
        if(img_itor != img_map.end())
        {
            MessageBox(handle_, "A Image with that name already exists", "Information", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            img_itor = img_map.find(game_instance_tree_item);
            if(img_itor != img_map.end())
            {
                ImageDocument copy = img_itor->second;
                img_map.erase(img_itor);
                img_map.insert(std::make_pair(dialog.Text(), copy));

                game_manager_dialog_bar_->UpdateImage(game_instance_tree_item, dialog.Text());
                OnStateMachineContentsModified();

                doc_.UpdateResourceContexts(game_instance_tree_item, dialog.Text());
            }
        }
    }
}

void SageStudioWindow::OnPropertiesImageAnimation(const std::string& game_instance_tree_item)
{
    GetTextDialog dialog;
    dialog.WindowTitle("ImageAnimation Properties");
    dialog.Label("Name:");
    dialog.Text(game_instance_tree_item);
    if(dialog.Run(instance_, handle_))
    {
        ImageAnimationDocumentMap& img_anim_map = doc_.ImageAnimationDocuments();
        ImageAnimationDocumentMap::iterator img_anim_itor = img_anim_map.find(dialog.Text());
        if(img_anim_itor != img_anim_map.end())
        {
            MessageBox(handle_, "A ImageAnimation with that name already exists", "Information", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            img_anim_itor = img_anim_map.find(game_instance_tree_item);
            if(img_anim_itor != img_anim_map.end())
            {
                ImageAnimationDocument copy = img_anim_itor->second;
                img_anim_map.erase(img_anim_itor);
                img_anim_map.insert(std::make_pair(dialog.Text(), copy));

                game_manager_dialog_bar_->UpdateImageAnimation(game_instance_tree_item, dialog.Text());
                OnStateMachineContentsModified();

                doc_.UpdateResourceContexts(game_instance_tree_item, dialog.Text());
            }
        }
    }
}

void SageStudioWindow::OnPropertiesEntityList(const std::string& game_instance_tree_item)
{
    GetTextDialog dialog;
    dialog.WindowTitle("EntityList Properties");
    dialog.Label("Name:");
    dialog.Text(game_instance_tree_item);
    if(dialog.Run(instance_, handle_))
    {
        game_manager_dialog_bar_->UpdateEntityList(game_instance_tree_item, dialog.Text());
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnPropertiesEngineParameter(const std::string& game_instance_tree_item)
{
    GetTextDialog dialog;
    dialog.WindowTitle("EngineParameter Properties");
    dialog.Label("Name:");
    dialog.Text(game_instance_tree_item);
    if(dialog.Run(instance_, handle_))
    {
        game_manager_dialog_bar_->UpdateEngineParameter(game_instance_tree_item, dialog.Text());
        OnStateMachineContentsModified();
    }
}




void SageStudioWindow::OnResourceContextModified()
{
    doc_.Modified(true);
    SetWindowTitle();
    EnableMenuItems();
}




void SageStudioWindow::OnImageModified()
{
    doc_.Modified(true);
    SetWindowTitle();
    EnableMenuItems();
}

void SageStudioWindow::OnImageFrameMoved(const POINT& position)
{
    ImageDocument* img_doc = GetCurrentImageDoc(game_instance_tree_item_);
    if(img_doc != NULL)
    {
        img_doc->FrameX(position.x);
        img_doc->FrameY(position.y);
    }
    else
    {
        ImageAnimationDocument* img_anim_doc = GetCurrentImageAnimationDoc(game_instance_tree_item_);
        if(img_anim_doc != NULL)
        {
            img_anim_doc->StartX(position.x);
            img_anim_doc->StartY(position.y);
        }
    }
    image_props_dialog_bar_->Position(position);
}

void SageStudioWindow::OnImageFrameResized(const SIZE& size)
{
    ImageDocument* img_doc = GetCurrentImageDoc(game_instance_tree_item_);
    if(img_doc != NULL)
    {
        img_doc->FrameWidth(size.cx);
        img_doc->FrameHeight(size.cy);
    }
    else
    {
        ImageAnimationDocument* img_anim_doc = GetCurrentImageAnimationDoc(game_instance_tree_item_);
        if(img_anim_doc != NULL)
        {
            img_anim_doc->FrameWidth(size.cx);
            img_anim_doc->FrameHeight(size.cy);
        }
    }
    image_props_dialog_bar_->Size(size);
}





void SageStudioWindow::OnImagePropertiesDialogBarModified()
{
    doc_.Modified(true);
    SetWindowTitle();
    EnableMenuItems();
}

void SageStudioWindow::OnImagePropertiesDialogBarTextureChanged(const std::string& filename)
{
    ImageDocument* img_doc = GetCurrentImageDoc(game_instance_tree_item_);
    if(img_doc != NULL)
    {
        img_doc->TextureFilename(filename);
    }
    else
    {
        ImageAnimationDocument* img_anim_doc = GetCurrentImageAnimationDoc(game_instance_tree_item_);
        if(img_anim_doc != NULL)
        {
            img_anim_doc->TextureFilename(filename);
        }
    }

    std::ostringstream full_path;
    full_path << content_dir_ << "\\Textures\\" << filename;
    image_window_->Name(game_instance_tree_item_);
    image_window_->Texture(full_path.str());
}

void SageStudioWindow::OnImagePropertiesDialogBarPositionChanged(const POINT& position)
{
    ImageDocument* img_doc = GetCurrentImageDoc(game_instance_tree_item_);
    if(img_doc != NULL)
    {
        img_doc->FrameX(position.x);
        img_doc->FrameY(position.y);
    }
    else
    {
        ImageAnimationDocument* img_anim_doc = GetCurrentImageAnimationDoc(game_instance_tree_item_);
        if(img_anim_doc != NULL)
        {
            img_anim_doc->StartX(position.x);
            img_anim_doc->StartY(position.y);
        }
    }
    image_window_->Position(position);
}

void SageStudioWindow::OnImagePropertiesDialogBarSizeChanged(const SIZE& size)
{
    ImageDocument* img_doc = GetCurrentImageDoc(game_instance_tree_item_);
    if(img_doc != NULL)
    {
        img_doc->FrameWidth(size.cx);
        img_doc->FrameHeight(size.cy);
    }
    else
    {
        ImageAnimationDocument* img_anim_doc = GetCurrentImageAnimationDoc(game_instance_tree_item_);
        if(img_anim_doc != NULL)
        {
            img_anim_doc->FrameWidth(size.cx);
            img_anim_doc->FrameHeight(size.cy);
        }
    }
    image_window_->Size(size);
}

void SageStudioWindow::OnImageDialogFrameCountChanged(int frame_count)
{
    ImageAnimationDocument* img_anim_doc = GetCurrentImageAnimationDoc(game_instance_tree_item_);
    if(img_anim_doc == NULL)
    {
        return;
    }
    img_anim_doc->FrameCount(frame_count);
    image_window_->FrameCount(frame_count);
}





void SageStudioWindow::OnImageToolsDialogBarZoomChanged(int zoom)
{
    image_window_->Zoom(zoom);
}

void SageStudioWindow::OnImageToolsDialogBarToolChanged(ImageToolsDialogBarEvents::Tool tool)
{
    switch(tool)
    {
    case ImageToolsDialogBarEvents::T_MOVEIMAGE:
        image_window_->SetMouseAction(ImageWindow::MA_MOVEIMAGE);
        break;
    case ImageToolsDialogBarEvents::T_RESIZEIMAGE:
        image_window_->SetMouseAction(ImageWindow::MA_RESIZEIMAGE);
        break;
    case ImageToolsDialogBarEvents::T_MOVEVIEW:
        image_window_->SetMouseAction(ImageWindow::MA_MOVEVIEW);
        break;
    }
}

void SageStudioWindow::OnImageToolsDialogBarEnableGridSnapChanged(bool enabled)
{
    image_window_->EnableGridSnap(enabled);
}

void SageStudioWindow::OnImageToolsDialogBarGridSnapChanged(int grid_snap)
{
    image_window_->GridSnap(grid_snap);
}




void SageStudioWindow::OnCellMapWindowZoomLevelChanged(int zoom)
{
    cell_map_view_dialog_bar_->SetZoomLevel(zoom);
}

void SageStudioWindow::OnCellMapWindowInsertCell(const CellIndex& index, CellType type, float cell_size, const std::string& ts)
{
    cell_map_.InsertCell(index, type, cell_size, ts);
    OnStateMachineContentsModified();
}

void SageStudioWindow::OnCellMapWindowRemoveCell(const CellIndex& index)
{
    cell_map_.RemoveCell(index);
    OnStateMachineContentsModified();
}

void SageStudioWindow::OnCellMapWindowSetTextureSet(const CellIndex& index, const std::string& ts)
{
    cell_map_.SetTextureSet(index, ts);
    OnStateMachineContentsModified();
}

void SageStudioWindow::OnCellMapWindowSetSkyFlag(const CellIndex& index, bool sky)
{
    cell_map_.SetSkyFlag(index, sky);
    OnStateMachineContentsModified();
}

void SageStudioWindow::OnCellMapLightsSelected(const std::vector<std::string>& light_names)
{
    lights_dialog_bar_->SelectLights(light_names);
}

void SageStudioWindow::OnCellMapLightSelectNone()
{
    lights_dialog_bar_->SelectLights(std::vector<std::string>());
}

void SageStudioWindow::OnCellMapLightPositionChanged(const std::vector<Light*>& lights)
{
    //std::vector<Light*>::const_iterator itor;
    //for(itor = lights.begin(); itor != lights.end(); ++itor)
    //{
    //    RecalcLighting(*itor, false);
    //}

    cell_map_window_->Repaint();
    OnStateMachineContentsModified();
}

void SageStudioWindow::OnCellMapLightRadiusChanged(const std::vector<Light*>& lights)
{
    lights_dialog_bar_->OnLightRadiusChanged();

    // I call this to recalc the lighting, and to flag the document as modified
    OnCellMapLightPositionChanged(lights);
}




void SageStudioWindow::OnCellMapViewDialogShowGridChanged(bool show)
{
    cell_map_window_->ShowGrid(show);
}

void SageStudioWindow::OnCellMapViewDialogBarViewStyleChanged(ViewStyle vs)
{
    cell_map_window_->SetViewStyle(vs);
}

void SageStudioWindow::OnCellMapViewDialogBarEditModeChanged(EditMode em)
{
    cell_map_window_->SetEditMode(em);
    switch(em)
    {
    case EM_CELL:
        cell_map_props_dialog_bar_->Show();
        texture_set_dialog_bar_->Hide();
        sky_dialog_bar_->Hide();
        lights_dialog_bar_->Hide();
        break;
    case EM_TEXTURE_SET:
        cell_map_props_dialog_bar_->Hide();
        texture_set_dialog_bar_->Show();
        sky_dialog_bar_->Hide();
        lights_dialog_bar_->Hide();
        break;
    case EM_SOUND:
        break;
    case EM_LIGHT:
        cell_map_props_dialog_bar_->Hide();
        texture_set_dialog_bar_->Hide();
        sky_dialog_bar_->Hide();
        lights_dialog_bar_->Show();
        break;
    case EM_ENTITY:
        break;
    case EM_CAMERA:
        break;
    case EM_REGION:
        break;
    case EM_FOG:
        break;
    case EM_TRIGGER:
        break;
    case EM_SKY:
        cell_map_props_dialog_bar_->Hide();
        texture_set_dialog_bar_->Hide();
        sky_dialog_bar_->Show();
        lights_dialog_bar_->Hide();
        break;
    }
}

void SageStudioWindow::OnCellMapViewDialogBarZoomLevelChanged(int zoom)
{
    cell_map_window_->SetZoomLevel(zoom);
}




void SageStudioWindow::OnCellMapCellBrushChanged(CellBrush cell_brush)
{
    cell_map_window_->SetCellBrush(cell_brush);
}

void SageStudioWindow::OnCellMapSelectNone()
{
    if(cell_map_window_->AreCellsSelected())
    {
        cell_map_window_->ClearSelectedCells();
    }
}

void SageStudioWindow::OnCellMapSetCellType()
{
    if(cell_map_window_->AreCellsSelected())
    {
        ChooseCellTypeDialog dialog;
        if(dialog.Run(instance_, handle_))
        {
            cell_map_window_->SetSelectedCellsCellType(dialog.GetCellType());
            OnStateMachineContentsModified();
        }
    }
}

void SageStudioWindow::OnCellMapSetTextureSet()
{
    if(cell_map_window_->AreCellsSelected())
    {
        ChooseTextureSetDialog dialog;
        dialog.SetTextureSetDocumentMap(&(doc_.TextureSetDocuments()));
        if(dialog.Run(instance_, handle_))
        {
            cell_map_window_->SetSelectedCellsTextureSet(dialog.TextureSet());
            OnStateMachineContentsModified();
        }
    }
}




void SageStudioWindow::OnDefaultBindingsChanged(const std::string& binding, const std::string& value)
{
    doc_.BindingsDocument().Set(binding, value);
    OnStateMachineContentsModified();
}

void SageStudioWindow::OnDefaultBindingsRemoved(const std::string& binding)
{
    doc_.BindingsDocument().Clear(binding);
    OnStateMachineContentsModified();
}

void SageStudioWindow::OnAllDefaultBindingsCleared()
{
    doc_.BindingsDocument().ClearAll();
    OnStateMachineContentsModified();
}




void SageStudioWindow::OnTextureSetBrushChanged(TextureSetBrush ts_brush)
{
    cell_map_window_->SetTextureSetBrush(ts_brush);
}

void SageStudioWindow::OnTextureSetSelectNone()
{
    if(cell_map_window_->AreCellsSelected())
    {
        cell_map_window_->ClearSelectedCells();
    }
}

void SageStudioWindow::OnTextureSetSetCellType()
{
    if(cell_map_window_->AreCellsSelected())
    {
        ChooseCellTypeDialog dialog;
        if(dialog.Run(instance_, handle_))
        {
            cell_map_window_->SetSelectedCellsCellType(dialog.GetCellType());
            OnStateMachineContentsModified();
        }
    }
}

void SageStudioWindow::OnTextureSetSetTextureSet()
{
    if(cell_map_window_->AreCellsSelected())
    {
        ChooseTextureSetDialog dialog;
        dialog.SetTextureSetDocumentMap(&(doc_.TextureSetDocuments()));
        if(dialog.Run(instance_, handle_))
        {
            cell_map_window_->SetSelectedCellsTextureSet(dialog.TextureSet());
            OnStateMachineContentsModified();
        }
    }
}

void SageStudioWindow::OnTextureSetTextureSetChanged(const std::string& texure_set)
{
    cell_map_window_->SetTextureSet(texure_set);
}

void SageStudioWindow::OnTextureSetDefaultTextureSetChanged(const std::string& default_texure_set)
{
    cell_map_window_->SetDefaultTextureSet(default_texure_set);
}




void SageStudioWindow::OnTextureSetSet(TextureSetChannel c, const std::string& filename)
{
    TextureSetDocument* ts_doc = GetCurrentTextureSetDoc(game_instance_tree_item_);
    if(ts_doc)
    {
        switch(c)
        {
        case TSC_WALL:          ts_doc->TexWall(filename);      break;
        case TSC_CEILING:       ts_doc->TexCeiling(filename);   break;
        case TSC_FLOOR:         ts_doc->TexFloor(filename);     break;
        case TSC_LIQUID:        ts_doc->TexLiquid(filename);    break;
        case TSC_DOOR:          ts_doc->TexDoor(filename);      break;
        case TSC_TRANS_WALL:    ts_doc->TexTransWall(filename); break;
        case TSC_TRANS_DOOR:    ts_doc->TexTransDoor(filename); break;
        }

        // Make sure the cell map window is able to draw this texture by loading
        // this texture into its resource context.
        Util::ResourceContext* res_cxt = cell_map_window_->GetResCxt();
        res_cxt->LoadTexture(content_dir_ + "\\Textures\\" + filename);

        cell_map_.BuildTextureSetMap(cell_map_window_->GetResCxt(), &(doc_.TextureSetDocuments()), content_dir_);

        cell_map_.BindCellsToTextures(
            boost::bind(&SageStudioWindow::OnBindTexturesTotal, this, _1),
            boost::bind(&SageStudioWindow::OnBindTexturesStep, this, _1));

        // Add this texture to the resource context indicated by this texture set's
        // resource context.
        MyTest::ResourceContextDocumentMap& rc_map = doc_.ResourceContextDocuments();
        MyTest::ResourceContextDocumentMap::iterator i = rc_map.find(ts_doc->ResCxt());
        if(i != rc_map.end())
        {
            // Add this texture to the res cxt if it's not already there.
            MyTest::ResourceContextDocument::iterator j = i->second.find(filename);
            if(j == i->second.end())
            {
                MyTest::ResourceInfo ri;
                ri.type_            = MyTest::RT_TEXTURE;
                ri.list_box_item_   = -1;
                i->second.insert(std::make_pair(filename, ri));
            }
        }

        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnTextureSetRemove(TextureSetChannel c)
{
    TextureSetDocument* ts_doc = GetCurrentTextureSetDoc(game_instance_tree_item_);
    if(ts_doc)
    {
        switch(c)
        {
        case TSC_WALL: ts_doc->TexWall(std::string()); break;
        case TSC_CEILING: ts_doc->TexCeiling(std::string()); break;
        case TSC_FLOOR: ts_doc->TexFloor(std::string()); break;
        case TSC_LIQUID: ts_doc->TexLiquid(std::string()); break;
        case TSC_DOOR: ts_doc->TexDoor(std::string()); break;
        case TSC_TRANS_WALL: ts_doc->TexTransWall(std::string()); break;
        case TSC_TRANS_DOOR: ts_doc->TexTransDoor(std::string()); break;
        }

        SynchroniseTextureSetResourceContexts();

        cell_map_.BuildTextureSetMap(cell_map_window_->GetResCxt(), &(doc_.TextureSetDocuments()), content_dir_);

        cell_map_.BindCellsToTextures(
            boost::bind(&SageStudioWindow::OnBindTexturesTotal, this, _1),
            boost::bind(&SageStudioWindow::OnBindTexturesStep, this, _1));

        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnTextureSetClearAll()
{
    TextureSetDocument* ts_doc = GetCurrentTextureSetDoc(game_instance_tree_item_);
    if(ts_doc)
    {
        ts_doc->TexWall(std::string());
        ts_doc->TexCeiling(std::string());
        ts_doc->TexFloor(std::string());
        ts_doc->TexLiquid(std::string());
        ts_doc->TexDoor(std::string());
        ts_doc->TexTransWall(std::string());
        ts_doc->TexTransDoor(std::string());

        SynchroniseTextureSetResourceContexts();

        cell_map_.BuildTextureSetMap(cell_map_window_->GetResCxt(), &(doc_.TextureSetDocuments()), content_dir_);

        cell_map_.BindCellsToTextures(
            boost::bind(&SageStudioWindow::OnBindTexturesTotal, this, _1),
            boost::bind(&SageStudioWindow::OnBindTexturesStep, this, _1));

        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnTextureSetSetLiquidColor(const Gfx::Color& color)
{
    TextureSetDocument* ts_doc = GetCurrentTextureSetDoc(game_instance_tree_item_);
    if(ts_doc)
    {
        ts_doc->LiquidColor(color);
        OnStateMachineContentsModified();
    }
}




void SageStudioWindow::OnSkyDialogBarCurrentBrushChanged(SkyBrush sky_brush)
{
    cell_map_window_->SetSkyBrush(sky_brush);
}

void SageStudioWindow::OnSkyDialogBarSetToSkyClicked()
{
    cell_map_window_->SetSelectedCellsToSky();
    OnStateMachineContentsModified();
}

void SageStudioWindow::OnSkyDialogBarClearSkyClicked()
{
    cell_map_window_->ClearSelectedCellsSky();
    OnStateMachineContentsModified();
}

void SageStudioWindow::OnSkyDialogBarChooseSkyTexClicked()
{
    ChooseTextureDialog dialog;
    dialog.DibResources(&dib_resources_);
    dialog.ContentDirectory(content_dir_);
    dialog.Filename(cell_map_.SkyTextureFilename());
    if(dialog.Run(instance_, handle_))
    {
        cell_map_.SkyTextureFilename(dialog.Filename());
        cell_map_window_->SkyTextureFilename(content_dir_, dialog.Filename());
        OnStateMachineContentsModified();
    }
}




void SageStudioWindow::OnLightsDialogBarLightsSelected(const std::vector<std::string>& light_names)
{
    cell_map_window_->SelectLights(light_names);
}

void SageStudioWindow::OnLightsDialogBarCurrentLightToolChanged(LightTool tool)
{
    cell_map_window_->SetCurrentLightTool(tool);
}

void SageStudioWindow::OnLightsDialogBarInsertLight()
{
    GetTextDialog dialog;
    dialog.WindowTitle("New Light");
    dialog.Label("Please enter a name for this light, or you may accept the default");

    std::ostringstream oss;
    oss << "Light" << (cell_map_.GetHighestLightNumber() + 1);
    dialog.Text(oss.str());

    if(!dialog.Run(instance_, handle_))
    {
        return;
    }

    lights_dialog_bar_->InsertLight(dialog.Text());
    cell_map_window_->InsertLight(dialog.Text());
    OnStateMachineContentsModified();
}

void SageStudioWindow::OnLightsDialogBarViewLight(const std::string& light_name)
{
    cell_map_window_->ViewLight(light_name);
}

void SageStudioWindow::OnLightsDialogBarSetSelectedLightsColor(const std::vector<std::string>& light_names)
{
    CHOOSECOLOR Info;
    memset(&Info, 0, sizeof(CHOOSECOLOR));

    COLORREF CustomColors[16];
    memset(CustomColors, 0, sizeof(COLORREF)*16);

    Info.lStructSize    = sizeof(CHOOSECOLOR);
    Info.hwndOwner      = handle_;
    Info.lpCustColors   = (LPDWORD)CustomColors;
    Info.Flags          = CC_ANYCOLOR | CC_FULLOPEN;

    if(ChooseColor(&Info))
    {
        cell_map_window_->SetLightsColor(light_names, Info.rgbResult);
        OnStateMachineContentsModified();
    }
}

void SageStudioWindow::OnLightsDialogBarRemoveLights(const std::vector<std::string>& light_names)
{
    cell_map_window_->RemoveLights(light_names);
    OnStateMachineContentsModified();
}

void SageStudioWindow::OnLightsDialogBarSetLightRadius(const std::vector<std::string>& light_names, int radius)
{
    cell_map_window_->SetLightRadius(light_names, radius);
    OnStateMachineContentsModified();
}

void SageStudioWindow::OnLightsDialogBarUseWorldAmbient()
{
    cell_map_window_->CurrentLightCellSetUseWorldAmbient();
    OnStateMachineContentsModified();
}

void SageStudioWindow::OnLightsDialogBarSpecifyAmbient()
{
    cell_map_window_->CurrentLightCellSetSpecifyAmbient();
    OnStateMachineContentsModified();
}

void SageStudioWindow::OnLightsDialogBarSetAmbientSpecificLight(int ambient)
{
    cell_map_window_->CurrentLightCellSetAmbientSpecificLight(ambient);
    OnStateMachineContentsModified();
}

void SageStudioWindow::OnLightsDialogBarSetAmbientWorldLight(int ambient)
{
    cell_map_window_->SetAmbientWorldLight(ambient);
    OnStateMachineContentsModified();
}

void SageStudioWindow::OnLightsDialogBarCalculateLighting()
{
    LightingProgressDialog Dialog;
    Dialog.SetCellMapDocument(&cell_map_);
    Dialog.Run(instance_, handle_);
    OnStateMachineContentsModified();
}

void OnCompileAllMaps()
{
    OnFileSave();
}

void OnCompileMap()
{
    if(cell_map_.Name().empty())
    {
        return;
    }

    OnFileSave();
}
