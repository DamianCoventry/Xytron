#ifndef INCLUDED_LIGHTSDIALOGBAR
#define INCLUDED_LIGHTSDIALOGBAR

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <boost/shared_ptr.hpp>
#include "worldtypes.h"

struct LightsDialogBarEvents
{
    virtual void OnLightsDialogBarLightsSelected(const std::vector<std::string>& light_names) = 0;
    virtual void OnLightsDialogBarCurrentLightToolChanged(LightTool tool) = 0;
    virtual void OnLightsDialogBarInsertLight() = 0;
    virtual void OnLightsDialogBarRemoveLights(const std::vector<std::string>& light_names) = 0;
    virtual void OnLightsDialogBarViewLight(const std::string& light_name) = 0;
    virtual void OnLightsDialogBarSetSelectedLightsColor(const std::vector<std::string>& light_names) = 0;
    virtual void OnLightsDialogBarSetLightRadius(const std::vector<std::string>& light_names, int radius) = 0;
    virtual void OnLightsDialogBarUseWorldAmbient() = 0;
    virtual void OnLightsDialogBarSpecifyAmbient() = 0;
    virtual void OnLightsDialogBarSetAmbientSpecificLight(int ambient) = 0;
    virtual void OnLightsDialogBarSetAmbientWorldLight(int ambient) = 0;
    virtual void OnLightsDialogBarCalculateLighting() = 0;
};

class CellMapDocument;

class LightsDialogBar
{
public:
    LightsDialogBar(HINSTANCE instance, HWND parent, LightsDialogBarEvents* event_handler, CellMapDocument* document);
    ~LightsDialogBar();

    void RepositionWithinParent();

    void InsertLight(const std::string& light);
    void SelectLights(const std::vector<std::string>& light_names);
    void SetLightRadius(int radius);
    void SetUseWorldAmbient();
    void SetSpecificAmbient(int ambient);
    void SetWorldAmbient(int ambient);
    void OnLightRadiusChanged();

    void Enable();
    void Disable();
    void Show();
    void Hide();
    void Clear();

private:
    friend INT_PTR CALLBACK LightsDialogBarProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam);

    void OnInitDialog();
    void OnCurrentLightChanged();
    void OnButtonInsertLight();
    void OnButtonRemoveLight();
    void OnButtonViewLight();
    void OnButtonSetColor();
    void OnCurrentLightToolChanged();
    void OnButtonSetLightRadius();
    void OnCheckBoxWorld();
    void OnCheckBoxSpecify();
    void OnButtonSetAmbientSpecificLight();
    void OnButtonSetAmbientWorldLight();

    void EnableDisableControls();

private:
    LightsDialogBarEvents* event_handler_;
    CellMapDocument* document_;
    HINSTANCE instance_;
    HWND handle_;
    LightTool current_tool_;
};

typedef boost::shared_ptr<LightsDialogBar> LightsDialogBarPtr;

#endif  // INCLUDED_LIGHTSDIALOGBAR
