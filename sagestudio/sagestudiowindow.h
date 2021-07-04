#ifndef INCLUDED_SAGESTUDIOWINDOW
#define INCLUDED_SAGESTUDIOWINDOW

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>

#include "gamemanagerdialogbar.h"
#include "statemachinewindow.h"
#include "statemachinetoolsdialogbar.h"
#include "emptystudiowindow.h"
#include "resourcecontextdialogbar.h"
#include "imagewindow.h"
#include "imagepropertiesdialogbar.h"
#include "imagetoolsdialogbar.h"
#include "cellmapwindow.h"
#include "cellmapviewdialogbar.h"
#include "cellmappropsdialogbar.h"
#include "defaultbindingsdialogbar.h"
#include "texturesetdialogbar.h"
#include "texturesetmanagerdialogbar.h"
#include "skydialogbar.h"
#include "lightsdialogbar.h"

#include "../sagedocuments/gamedocument.h"
#include "dibresources.h"

#include "../aud/aud.h"

class SageStudioWindow
    : public StateMachineToolsDialogBarEvents
    , public StateMachineWindowEvents
    , public GameManagerDialogBarEvents
    , public ResourceContextDialogBarEvents
    , public ImageWindowEvents
    , public ImagePropertiesDialogBarEvents
    , public ImageToolsDialogBarEvents
    , public CellMapWindowEvents
    , public CellMapViewDialogBarEvents
    , public CellMapPropsDialogBarEvents
    , public DefaultBindingsDialogBarEvents
    , public TextureSetDialogBarEvents
    , public TextureSetManagerDialogBarEvents
    , public SkyDialogBarEvents
    , public LightsDialogBarEvents
{
public:
    SageStudioWindow(HINSTANCE instance);
    ~SageStudioWindow();

    void Show(int cmd_show);
    void Hide();

    HWND Handle() const { return handle_; }

private:
    void Register();
    void Unregister();
    void Create();
    void Delete();

    friend LRESULT CALLBACK SageStudioWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnWindowCreate();
    bool OnWindowClose();
    void OnWindowSize();

    void OnFileNew();
    void OnFileOpen();
    void OnFileSave();
    void OnFileSaveAs();
    void OnFileClose();
    bool OnFileExit();

    void OnEditInsert();
    void OnEditUndo();
    void OnEditRedo();
    void OnEditCut();
    void OnEditCopy();
    void OnEditPaste();
    void OnEditDelete();
    void OnEditProperties();

    void OnViewContentDirectoryManager();

    // StateMachineToolsDialogBarEvents
    void OnStateMachineSelect();
    void OnStateMachineInsertEntry();
    void OnStateMachineInsertExit();
    void OnStateMachineInsertState();
    void OnStateMachineInsertEvent();
    void OnStateMachineDelete();
    void OnStateMachineProperties();
    void OnStateMachineClearAll();

    // StateMachineWindowEvents
    void OnStateMachineContentsModified();
    void OnStateMachineDoubleClick();
    void OnStateMachineEntryNodeInserted(int id, const POINT& position);
    void OnStateMachineEntryNodeRemoved(int id);
    void OnStateMachineEntryNodeUpdated(int id, const POINT& position);
    void OnStateMachineExitNodeInserted(int id, const POINT& position);
    void OnStateMachineExitNodeRemoved(int id);
    void OnStateMachineExitNodeUpdated(int id, const POINT& position);
    void OnStateMachineStateNodeInserted(int id, const POINT& position, const std::string& name);
    void OnStateMachineStateNodeRemoved(int id);
    void OnStateMachineStateNodeUpdated(int id, const POINT& position, const std::string& name);
    void OnStateMachineEventNodeInserted(int id, const POINT& position, const std::string& name, int source_id, int dest_id);
    void OnStateMachineEventNodeRemoved(int id);
    void OnStateMachineEventNodeUpdated(int id, const POINT& position, const std::string& name, int source_id, int dest_id);

    // GameManagerDialogBarEvents
    void OnTreeItemGameSelected(const std::string& game_tree_item);
    void OnTreeItemGameTypeSelected(const std::string& game_type_tree_item);
    void OnTreeItemGameInstanceSelected(const std::string& game_type_tree_item, const std::string& game_instance_tree_item);
    void OnTreeItemGameRightClicked(const std::string& game_tree_item);
    void OnTreeItemGameTypeRightClicked(const std::string& game_type_tree_item);
    void OnTreeItemGameInstanceRightClicked(const std::string& game_type_tree_item, const std::string& game_instance_tree_item);

    // ResourceContextDialogBarEvents
    void OnResourceContextModified();

    // ImageWindowEvents
    void OnImageModified();
    void OnImageFrameMoved(const POINT& position);
    void OnImageFrameResized(const SIZE& size);

    // ImagePropertiesDialogBarEvents
    void OnImagePropertiesDialogBarModified();
    void OnImagePropertiesDialogBarTextureChanged(const std::string& filename);
    void OnImagePropertiesDialogBarPositionChanged(const POINT& position);
    void OnImagePropertiesDialogBarSizeChanged(const SIZE& size);
    void OnImageDialogFrameCountChanged(int frame_count);

    // ImageToolsDialogBarEvents
    void OnImageToolsDialogBarZoomChanged(int zoom);
    void OnImageToolsDialogBarToolChanged(ImageToolsDialogBarEvents::Tool tool);
    void OnImageToolsDialogBarEnableGridSnapChanged(bool enabled);
    void OnImageToolsDialogBarGridSnapChanged(int grid_snap);

    // CellMapWindowEvents
    void OnCellMapWindowZoomLevelChanged(int zoom);
    void OnCellMapWindowInsertCell(const CellIndex& index, CellType type, float cell_size, const std::string& ts);
    void OnCellMapWindowRemoveCell(const CellIndex& index);
    void OnCellMapWindowSetTextureSet(const CellIndex& index, const std::string& ts);
    void OnCellMapWindowSetSkyFlag(const CellIndex& index, bool sky);
    void OnCellMapLightsSelected(const std::vector<std::string>& light_names);
    void OnCellMapLightSelectNone();
    void OnCellMapLightPositionChanged(const std::vector<Light*>& lights);
    void OnCellMapLightRadiusChanged(const std::vector<Light*>& lights);

    // CellMapViewDialogBarEvents
    void OnCellMapViewDialogShowGridChanged(bool show);
    void OnCellMapViewDialogBarViewStyleChanged(ViewStyle vs);
    void OnCellMapViewDialogBarEditModeChanged(EditMode em);
    void OnCellMapViewDialogBarZoomLevelChanged(int zoom);

    // CellMapPropsDialogBarEvents
    void OnCellMapCellBrushChanged(CellBrush cell_brush);
    void OnCellMapSelectNone();
    void OnCellMapSetCellType();
    void OnCellMapSetTextureSet();

    // DefaultBindingsDialogBarEvents
    void OnDefaultBindingsChanged(const std::string& binding, const std::string& value);
    void OnDefaultBindingsRemoved(const std::string& binding);
    void OnAllDefaultBindingsCleared();

    // TextureSetDialogBarEvents
    void OnTextureSetBrushChanged(TextureSetBrush ts_brush);
    void OnTextureSetSelectNone();
    void OnTextureSetSetCellType();
    void OnTextureSetSetTextureSet();
    void OnTextureSetTextureSetChanged(const std::string& texure_set);
    void OnTextureSetDefaultTextureSetChanged(const std::string& default_texure_set);

    // TextureSetManagerDialogBarEvents
    void OnTextureSetSet(TextureSetChannel c, const std::string& filename);
    void OnTextureSetRemove(TextureSetChannel c);
    void OnTextureSetClearAll();
    void OnTextureSetSetLiquidColor(const Gfx::Color& color);

    // SkyDialogBarEvents
    void OnSkyDialogBarCurrentBrushChanged(SkyBrush sky_brush);
    void OnSkyDialogBarSetToSkyClicked();
    void OnSkyDialogBarClearSkyClicked();
    void OnSkyDialogBarChooseSkyTexClicked();

    // LightsDialogBarEvents
    void OnLightsDialogBarLightsSelected(const std::vector<std::string>& light_names);
    void OnLightsDialogBarCurrentLightToolChanged(LightTool tool);
    void OnLightsDialogBarInsertLight();
    void OnLightsDialogBarViewLight(const std::string& light_name);
    void OnLightsDialogBarSetSelectedLightsColor(const std::vector<std::string>& light_names);
    void OnLightsDialogBarRemoveLights(const std::vector<std::string>& light_names);
    void OnLightsDialogBarSetLightRadius(const std::vector<std::string>& light_names, int radius);
    void OnLightsDialogBarUseWorldAmbient();
    void OnLightsDialogBarSpecifyAmbient();
    void OnLightsDialogBarSetAmbientSpecificLight(int ambient);
    void OnLightsDialogBarSetAmbientWorldLight(int ambient);
    void OnLightsDialogBarCalculateLighting();

private:
    bool ModifiedCheck();
    void EnableMenuItems();
    void SetWindowTitle();

    void ShowEmptyStudioView();
    void ShowView(const std::string& view_name);
    void EnableView(const std::string& view_name, bool enable = true);
    void UpdateCurrentView();
    void HideAllViews();

    void SaveCurrentMap();
    void LoadNewMap(const std::string& map_name);

    void SynchroniseTextureSetResourceContexts();

    void OnBindTexturesTotal(int total);
    void OnBindTexturesStep(int value);

    void RecalcLighting(Light* l, bool repaint);

    StateMachineDocument* GetCurrentStateMachineDoc(std::string& game_instance_tree_item) const;
    MyTest::ResourceContextDocument* GetCurrentResourceContextDoc(std::string& game_instance_tree_item) const;
    ImageDocument* GetCurrentImageDoc(std::string& game_instance_tree_item) const;
    ImageAnimationDocument* GetCurrentImageAnimationDoc(std::string& game_instance_tree_item) const;
    CellMapDocument* GetCellMapDoc(std::string& game_instance_tree_item) const;
    CameraDocument* GetCurrentCameraDoc(std::string& game_instance_tree_item) const;
    TextureSetDocument* GetCurrentTextureSetDoc(std::string& game_instance_tree_item) const;

    void DoFileClose();
    void DoFileSave(const std::string& filename);
    void DoFirstTimeLoad();

    void OnInsertEntity();
    void OnInsertCamera();
    void OnInsertTextureSet();
    void OnInsertMusic();
    void OnInsertTrigger();
    void OnInsertMap();
    void OnInsertStateMachine();
    void OnInsertDialog();
    void OnInsertFont();
    void OnInsertResourceContext();
    void OnInsertImage();
    void OnInsertImageAnimation();
    void OnInsertEntityList();
    void OnInsertEngineParameter();

    void OnRemoveEntity(const std::string& game_instance_tree_item);
    void OnRemoveCamera(const std::string& game_instance_tree_item);
    void OnRemoveTextureSet(const std::string& game_instance_tree_item);
    void OnRemoveMusic(const std::string& game_instance_tree_item);
    void OnRemoveTrigger(const std::string& game_instance_tree_item);
    void OnRemoveMap(const std::string& game_instance_tree_item);
    void OnRemoveStateMachine(const std::string& game_instance_tree_item);
    void OnRemoveDialog(const std::string& game_instance_tree_item);
    void OnRemoveFont(const std::string& game_instance_tree_item);
    void OnRemoveResourceContext(const std::string& game_instance_tree_item);
    void OnRemoveImage(const std::string& game_instance_tree_item);
    void OnRemoveImageAnimation(const std::string& game_instance_tree_item);
    void OnRemoveEntityList(const std::string& game_instance_tree_item);
    void OnRemoveEngineParameter(const std::string& game_instance_tree_item);

    void OnPropertiesGame(const std::string& game_tree_item);
    void OnPropertiesCamera(const std::string& game_tree_item);
    void OnPropertiesTextureSet(const std::string& game_tree_item);
    void OnPropertiesEntity(const std::string& game_instance_tree_item);
    void OnPropertiesMusic(const std::string& game_instance_tree_item);
    void OnPropertiesTrigger(const std::string& game_instance_tree_item);
    void OnPropertiesMap(const std::string& game_instance_tree_item);
    void OnPropertiesStateMachine(const std::string& game_instance_tree_item);
    void OnPropertiesDialog(const std::string& game_instance_tree_item);
    void OnPropertiesFont(const std::string& game_instance_tree_item);
    void OnPropertiesResourceContext(const std::string& game_instance_tree_item);
    void OnPropertiesImage(const std::string& game_instance_tree_item);
    void OnPropertiesImageAnimation(const std::string& game_instance_tree_item);
    void OnPropertiesEntityList(const std::string& game_instance_tree_item);
    void OnPropertiesEngineParameter(const std::string& game_instance_tree_item);

    void OnCompileAllMaps();
    void OnCompileMap();

private:
    static const char* window_title_;
    static const char* class_name_;

    HINSTANCE instance_;
    HWND handle_;

    bool first_time_load_;

    GameDocument    doc_;
    CellMapDocument cell_map_;

    std::string current_view_;
    std::string game_instance_tree_item_;
    std::string content_dir_;

    Aud::Device audio_device_;
    MyTest::DibResources dib_resources_;

    StateMachineWindowPtr           state_machine_window_;
    EmptyStudioWindowPtr            empty_studio_window_;
    ImageWindowPtr                  image_window_;
    CellMapWindowPtr                cell_map_window_;

    GameManagerDialogBarPtr         game_manager_dialog_bar_;
    StateMachineToolsDialogBarPtr   state_machine_tools_dialog_bar_;
    ResourceContextDialogBarPtr     resource_context_dialog_bar_;
    ImagePropertiesDialogBarPtr     image_props_dialog_bar_;
    ImageToolsDialogBarPtr          image_tools_dialog_bar_;
    CellMapViewDialogBarPtr         cell_map_view_dialog_bar_;
    CellMapPropsDialogBarPtr        cell_map_props_dialog_bar_;
    TextureSetDialogBarPtr          texture_set_dialog_bar_;
    DefaultBindingsDialogBarPtr     default_bindings_dialog_bar_;
    TextureSetManagerDialogBarPtr   texture_set_manager_dialog_bar_;
    SkyDialogBarPtr                 sky_dialog_bar_;
    LightsDialogBarPtr              lights_dialog_bar_;
};

#endif  // INCLUDED_SAGESTUDIOWINDOW
