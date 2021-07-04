#include "commanddialog.h"
#include "resource.h"
#include "dialogutil.h"
#include "chooseimagedialog.h"
#include "choosesounddialog.h"
#include "choosemusicdialog.h"
#include "chooseresourcecontextdialog.h"
#include "choosecellmapdialog.h"
#include "choosecameradialog.h"

#include <commctrl.h>
#include <windowsx.h>

#include <sstream>
#include <map>

#include <boost/shared_ptr.hpp>

#pragma warning(disable : 4311)     // 'reinterpret_cast' : pointer truncation from 'LPVOID' to 'LONG'
#pragma warning(disable : 4312)     // 'reinterpret_cast' : conversion from 'LONG' to 'CommandDialog *' of greater size


namespace
{
    struct Command
    {
        virtual ~Command() {}
        virtual std::string Name() const = 0;
        virtual void Parameters(const std::vector<std::string>& params) = 0;
        virtual const std::vector<std::string>& Parameters()  const = 0;
        virtual std::wstring BuildLinkText() const = 0;
        virtual std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id) = 0;
    };

    typedef boost::shared_ptr<Command> CommandPtr;
    typedef std::map<std::string, CommandPtr> CommandMap;
    CommandMap cmd_map_;

    std::wstring ToWide(const std::string& text)
    {
        wchar_t wbuffer[1024];
        memset(wbuffer, 0, sizeof(wchar_t)*1024);
        MultiByteToWideChar(CP_ACP, 0, text.c_str(), int(text.size()), wbuffer, 1024);
        return std::wstring(wbuffer);
    }
}

class LoadImageCommand
    : public Command
{
public:
    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* res_ctxt_map) { res_ctxt_map_ = res_ctxt_map; }
    void SetImageDocumentMap(ImageDocumentMap* image_map)                           { image_map_ = image_map; }
    void DibResources(MyTest::DibResources* dib_resources)                          { dib_resources_ = dib_resources; }
    std::string Name() const { return "Image.Load"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); params_.push_back(params[0]); params_.push_back(params[1]); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Load the image named <A ID=\"ImageId\">" << ToWide(params_[0])
            << L"</A> into the resource context named <A ID=\"ResCxtId\">" << ToWide(params_[1])
            << L"</A>";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        std::string rv;
        if(link_id == L"ImageId")
        {
            ChooseImageDialog dialog;
            dialog.SetImageDocumentMap(image_map_);
            dialog.DibResources(dib_resources_);
            dialog.ImageFilename(params_[0]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[0] = dialog.ImageFilename();
            }
        }
        else if(link_id == L"ResCxtId")
        {
            ChooseResourceContextDialog dialog;
            dialog.SetResourceContextDocumentMap(res_ctxt_map_);
            dialog.ResourceContextName(params_[1]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[1] = dialog.ResourceContextName();
            }
        }
        return rv;
    }
private:
    std::vector<std::string> params_;
    MyTest::ResourceContextDocumentMap* res_ctxt_map_;
    ImageDocumentMap* image_map_;
    MyTest::DibResources* dib_resources_;
};

class LoadImageAnimationCommand
    : public Command
{
public:
    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* res_ctxt_map) { res_ctxt_map_ = res_ctxt_map; }
    void SetImageAnimationDocumentMap(ImageAnimationDocumentMap* image_anim_map)    { image_anim_map_ = image_anim_map; }
    void DibResources(MyTest::DibResources* dib_resources)                          { dib_resources_ = dib_resources; }
    std::string Name() const { return "ImageAnim.Load"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); params_.push_back(params[0]); params_.push_back(params[1]); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Load the image animation named <A ID=\"ImageAnimationId\">" << ToWide(params_[0])
            << L"</A> into the resource context named <A ID=\"ResCxtId\">" << ToWide(params_[1])
            << L"</A>";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        std::string rv;
        if(link_id == L"ImageAnimationId")
        {
            ChooseImageDialog dialog;
            dialog.SetImageAnimationDocumentMap(image_anim_map_);
            dialog.DibResources(dib_resources_);
            dialog.ImageFilename(params_[0]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[0] = dialog.ImageFilename();
            }
        }
        else if(link_id == L"ResCxtId")
        {
            ChooseResourceContextDialog dialog;
            dialog.SetResourceContextDocumentMap(res_ctxt_map_);
            dialog.ResourceContextName(params_[1]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[1] = dialog.ResourceContextName();
            }
        }
        return rv;
    }
private:
    std::vector<std::string> params_;
    MyTest::ResourceContextDocumentMap* res_ctxt_map_;
    ImageAnimationDocumentMap* image_anim_map_;
    MyTest::DibResources* dib_resources_;
};

class LoadSoundCommand
    : public Command
{
public:
    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* res_ctxt_map) { res_ctxt_map_ = res_ctxt_map; }
    void ContentDirectory(const std::string& content_dir)   { content_dir_ = content_dir; }
    std::string Name() const { return "Sound.Load"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); params_.push_back(params[0]); params_.push_back(params[1]); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Load the sound named <A ID=\"SoundId\">" << ToWide(params_[0])
            << L"</A> into the resource context named <A ID=\"ResCxtId\">" << ToWide(params_[1])
            << L"</A>";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        std::string rv;
        if(link_id == L"SoundId")
        {
            ChooseSoundDialog dialog;
            dialog.ContentDirectory(content_dir_);
            dialog.SoundFilename(params_[0]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[0] = dialog.SoundFilename();
            }
        }
        else if(link_id == L"ResCxtId")
        {
            ChooseResourceContextDialog dialog;
            dialog.SetResourceContextDocumentMap(res_ctxt_map_);
            dialog.ResourceContextName(params_[1]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[1] = dialog.ResourceContextName();
            }
        }
        return rv;
    }
private:
    std::vector<std::string> params_;
    MyTest::ResourceContextDocumentMap* res_ctxt_map_;
    std::string content_dir_;
};

class LoadMusicCommand
    : public Command
{
public:
    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* res_ctxt_map) { res_ctxt_map_ = res_ctxt_map; }
    void ContentDirectory(const std::string& content_dir)   { content_dir_ = content_dir; }
    std::string Name() const { return "Music.Load"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); params_.push_back(params[0]); params_.push_back(params[1]); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Load the music track named <A ID=\"MusicId\">" << ToWide(params_[0])
            << L"</A> into the resource context named <A ID=\"ResCxtId\">" << ToWide(params_[1])
            << L"</A>";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        std::string rv;
        if(link_id == L"MusicId")
        {
            ChooseMusicDialog dialog;
            dialog.ContentDirectory(content_dir_);
            dialog.MusicFilename(params_[0]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[0] = dialog.MusicFilename();
            }
        }
        else if(link_id == L"ResCxtId")
        {
            ChooseResourceContextDialog dialog;
            dialog.SetResourceContextDocumentMap(res_ctxt_map_);
            dialog.ResourceContextName(params_[1]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[1] = dialog.ResourceContextName();
            }
        }
        return rv;
    }
private:
    std::vector<std::string> params_;
    MyTest::ResourceContextDocumentMap* res_ctxt_map_;
    std::string content_dir_;
};

class LoadEntityCommand
    : public Command
{
public:
    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* res_ctxt_map) { res_ctxt_map_ = res_ctxt_map; }
    std::string Name() const { return "Entity.Load"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); params_.push_back(params[0]); params_.push_back(params[1]); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Load the entity named <A ID=\"EntityId\">" << ToWide(params_[0])
            << L"</A> into the resource context named <A ID=\"ResCxtId\">" << ToWide(params_[1])
            << L"</A>";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        std::string rv;
        if(link_id == L"EntityId")
        {
        }
        return rv;
    }
private:
    std::vector<std::string> params_;
    MyTest::ResourceContextDocumentMap* res_ctxt_map_;
};

class LoadResourceContextCommand
    : public Command
{
public:
    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* res_ctxt_map) { res_ctxt_map_ = res_ctxt_map; }
    std::string Name() const { return "ResCxt.Load"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); params_.push_back(params[0]); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Load the resource context named <A ID=\"ResourceContextId\">" << ToWide(params_[0]) << L"</A>";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        std::string rv;
        if(link_id == L"ResourceContextId")
        {
            ChooseResourceContextDialog dialog;
            dialog.SetResourceContextDocumentMap(res_ctxt_map_);
            dialog.ResourceContextName(params_[0]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[0] = dialog.ResourceContextName();
            }
        }
        return rv;
    }
private:
    std::vector<std::string> params_;
    MyTest::ResourceContextDocumentMap* res_ctxt_map_;
};

class LoadCellMapCommand
    : public Command
{
public:
    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* res_ctxt_map) { res_ctxt_map_ = res_ctxt_map; }
    void SetCellMapDocumentList(CellMapDocumentList* cell_map_list) { cell_map_list_ = cell_map_list; }
    std::string Name() const { return "CellMap.Load"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); params_.push_back(params[0]); params_.push_back(params[1]); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Load the cell map named <A ID=\"CellMapId\">" << ToWide(params_[0])
            << L"</A>, bind Texture Sets to Resource Context <A ID=\"ResCxtId\">" << ToWide(params_[1])
            << L"</A>";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        std::string rv;
        if(link_id == L"CellMapId")
        {
            ChooseCellMapDialog dialog;
            dialog.SetCellMapDocumentList(cell_map_list_);
            dialog.CellMapName(params_[0]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[0] = dialog.CellMapName();
            }
        }
        else if(link_id == L"ResCxtId")
        {
            ChooseResourceContextDialog dialog;
            dialog.SetResourceContextDocumentMap(res_ctxt_map_);
            dialog.ResourceContextName(params_[1]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[1] = dialog.ResourceContextName();
            }
        }
        return rv;
    }
private:
    std::vector<std::string> params_;
    CellMapDocumentList* cell_map_list_;
    MyTest::ResourceContextDocumentMap* res_ctxt_map_;
};

class UnloadImageCommand
    : public Command
{
public:
    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* res_ctxt_map) { res_ctxt_map_ = res_ctxt_map; }
    void SetImageDocumentMap(ImageDocumentMap* image_map)                           { image_map_ = image_map; }
    void DibResources(MyTest::DibResources* dib_resources)                          { dib_resources_ = dib_resources; }
    std::string Name() const { return "Image.Unload"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); params_.push_back(params[0]); params_.push_back(params[1]); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Unload the image named <A ID=\"ImageId\">" << ToWide(params_[0])
            << L"</A> from the resource context named <A ID=\"ResCxtId\">" << ToWide(params_[1])
            << L"</A>";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        std::string rv;
        if(link_id == L"ImageId")
        {
            ChooseImageDialog dialog;
            dialog.SetImageDocumentMap(image_map_);
            dialog.DibResources(dib_resources_);
            dialog.ImageFilename(params_[0]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[0] = dialog.ImageFilename();
            }
        }
        else if(link_id == L"ResCxtId")
        {
            ChooseResourceContextDialog dialog;
            dialog.SetResourceContextDocumentMap(res_ctxt_map_);
            dialog.ResourceContextName(params_[1]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[1] = dialog.ResourceContextName();
            }
        }
        return rv;
    }
private:
    std::vector<std::string> params_;
    MyTest::ResourceContextDocumentMap* res_ctxt_map_;
    ImageDocumentMap* image_map_;
    MyTest::DibResources* dib_resources_;
};

class UnloadImageAnimationCommand
    : public Command
{
public:
    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* res_ctxt_map) { res_ctxt_map_ = res_ctxt_map; }
    void SetImageAnimationDocumentMap(ImageAnimationDocumentMap* image_anim_map)    { image_anim_map_ = image_anim_map; }
    void DibResources(MyTest::DibResources* dib_resources)                          { dib_resources_ = dib_resources; }
    std::string Name() const { return "ImageAnim.Unload"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); params_.push_back(params[0]); params_.push_back(params[1]); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Unload the image animation named <A ID=\"ImageAnimationId\">" << ToWide(params_[0])
            << L"</A> from the resource context named <A ID=\"ResCxtId\">" << ToWide(params_[1])
            << L"</A>";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        std::string rv;
        if(link_id == L"ImageAnimationId")
        {
            ChooseImageDialog dialog;
            dialog.SetImageAnimationDocumentMap(image_anim_map_);
            dialog.DibResources(dib_resources_);
            dialog.ImageFilename(params_[0]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[0] = dialog.ImageFilename();
            }
        }
        else if(link_id == L"ResCxtId")
        {
            ChooseResourceContextDialog dialog;
            dialog.SetResourceContextDocumentMap(res_ctxt_map_);
            dialog.ResourceContextName(params_[1]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[1] = dialog.ResourceContextName();
            }
        }
        return rv;
    }
private:
    std::vector<std::string> params_;
    MyTest::ResourceContextDocumentMap* res_ctxt_map_;
    ImageAnimationDocumentMap* image_anim_map_;
    MyTest::DibResources* dib_resources_;
};

class UnloadSoundCommand
    : public Command
{
public:
    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* res_ctxt_map) { res_ctxt_map_ = res_ctxt_map; }
    void ContentDirectory(const std::string& content_dir)   { content_dir_ = content_dir; }
    std::string Name() const { return "Sound.Unload"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); params_.push_back(params[0]); params_.push_back(params[1]); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Unload the sound named <A ID=\"SoundId\">" << ToWide(params_[1])
            << L"</A> from the resource context named <A ID=\"ResCxtId\">" << ToWide(params_[0])
            << L"</A>";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        std::string rv;
        if(link_id == L"SoundId")
        {
            ChooseSoundDialog dialog;
            dialog.ContentDirectory(content_dir_);
            dialog.SoundFilename(params_[0]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[0] = dialog.SoundFilename();
            }
        }
        else if(link_id == L"ResCxtId")
        {
            ChooseResourceContextDialog dialog;
            dialog.SetResourceContextDocumentMap(res_ctxt_map_);
            dialog.ResourceContextName(params_[1]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[1] = dialog.ResourceContextName();
            }
        }
        return rv;
    }
private:
    std::vector<std::string> params_;
    MyTest::ResourceContextDocumentMap* res_ctxt_map_;
    std::string content_dir_;
};

class UnloadMusicCommand
    : public Command
{
public:
    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* res_ctxt_map) { res_ctxt_map_ = res_ctxt_map; }
    void ContentDirectory(const std::string& content_dir)   { content_dir_ = content_dir; }
    std::string Name() const { return "Music.Unload"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); params_.push_back(params[0]); params_.push_back(params[1]); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Unload the music track named <A ID=\"MusicId\">" << ToWide(params_[0])
            << L"</A> from the resource context named <A ID=\"ResCxtId\">" << ToWide(params_[1])
            << L"</A>";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        std::string rv;
        if(link_id == L"MusicId")
        {
            ChooseMusicDialog dialog;
            dialog.ContentDirectory(content_dir_);
            dialog.MusicFilename(params_[0]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[0] = dialog.MusicFilename();
            }
        }
        else if(link_id == L"ResCxtId")
        {
            ChooseResourceContextDialog dialog;
            dialog.SetResourceContextDocumentMap(res_ctxt_map_);
            dialog.ResourceContextName(params_[1]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[1] = dialog.ResourceContextName();
            }
        }
        return rv;
    }
private:
    std::vector<std::string> params_;
    MyTest::ResourceContextDocumentMap* res_ctxt_map_;
    std::string content_dir_;
};

class UnloadEntityCommand
    : public Command
{
public:
    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* res_ctxt_map) { res_ctxt_map_ = res_ctxt_map; }
    std::string Name() const { return "Entity.Unload"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); params_.push_back(params[0]); params_.push_back(params[1]); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Unload the entity named <A ID=\"EntityId\">" << ToWide(params_[0])
            << L"</A> from the resource context named <A ID=\"ResCxtId\">" << ToWide(params_[1])
            << L"</A>";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        std::string rv;
        if(link_id == L"EntityId")
        {
        }
        return rv;
    }
private:
    std::vector<std::string> params_;
    MyTest::ResourceContextDocumentMap* res_ctxt_map_;
};

class UnloadResourceContextCommand
    : public Command
{
public:
    void SetResourceContextDocumentMap(MyTest::ResourceContextDocumentMap* res_ctxt_map) { res_ctxt_map_ = res_ctxt_map; }
    std::string Name() const { return "ResCxt.Unload"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); params_.push_back(params[0]); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Unload the resource context named <A ID=\"ResourceContextId\">" << ToWide(params_[0]) << L"</A>";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        std::string rv;
        if(link_id == L"ResourceContextId")
        {
            ChooseResourceContextDialog dialog;
            dialog.SetResourceContextDocumentMap(res_ctxt_map_);
            dialog.ResourceContextName(params_[0]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[0] = dialog.ResourceContextName();
            }
        }
        return rv;
    }
private:
    std::vector<std::string> params_;
    MyTest::ResourceContextDocumentMap* res_ctxt_map_;
};

class UnloadCellMapCommand
    : public Command
{
public:
    void SetCellMapDocumentList(CellMapDocumentList* cell_map_list) { cell_map_list_ = cell_map_list; }
    std::string Name() const { return "CellMap.Unload"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Unload the current cell map";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        return std::string();
    }
private:
    std::vector<std::string> params_;
    CellMapDocumentList* cell_map_list_;
};

class ThinkCellMapCommand
    : public Command
{
public:
    std::string Name() const { return "CellMap.Think"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Allow the currently loaded 2D Cell Map to think";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        return std::string();
    }
private:
    std::vector<std::string> params_;
};

class DrawCellMapCommand
    : public Command
{
public:
    std::string Name() const { return "CellMap.Draw"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Draw the currently loaded 2D Cell Map";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        return std::string();
    }
private:
    std::vector<std::string> params_;
};

class ActivateBindingsCommand
    : public Command
{
public:
    std::string Name() const { return "Bindings.Activate"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Activate the input bindings";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        return std::string();
    }
private:
    std::vector<std::string> params_;
};

class DeactivateBindingsCommand
    : public Command
{
public:
    std::string Name() const { return "Bindings.Deactivate"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Deactivate the input bindings";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        return std::string();
    }
private:
    std::vector<std::string> params_;
};

class SetCurrentCameraCommand
    : public Command
{
public:
    void SetCameraDocuments(CameraDocumentMap* docs) { docs_ = docs; }
    std::string Name() const { return "Camera.Current.Set"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); params_.push_back(params[0]); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Set the game's current camera to the camera named <A ID=\"CameraId\">" << ToWide(params_[0]) << L"</A>";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        std::string rv;
        if(link_id == L"CameraId")
        {
            ChooseCameraDialog dialog;
            dialog.CameraDocuments(docs_);
            dialog.Camera(params_[0]);
            if(dialog.Run(instance, parent))
            {
                rv = params_[0] = dialog.Camera();
            }
        }
        return rv;
    }
private:
    std::vector<std::string> params_;
    CameraDocumentMap* docs_;
};

class CameraToggleCollisionCommand
    : public Command
{
public:
    std::string Name() const { return "Camera.ToggleCollision"; }
    void Parameters(const std::vector<std::string>& params) { params_.clear(); }
    const std::vector<std::string>& Parameters()  const     { return params_; }
    std::wstring BuildLinkText() const
    {
        std::wostringstream oss;
        oss << L"Toggle whether or not cameras collide with the map object";
        return oss.str();
    }
    std::string RunChooseDialog(HINSTANCE instance, HWND parent, const std::wstring& link_id)
    {
        return std::string();
    }
private:
    std::vector<std::string> params_;
};







const int CommandDialog::IDC_LINK_ID = 1001;

CommandDialog::CommandDialog()
: image_anim_map_(NULL)
, image_map_(NULL)
{
}

bool CommandDialog::Run(HINSTANCE instance, HWND parent)
{
    return DialogBoxParam(instance, MAKEINTRESOURCE(IDD_COMMAND_DIALOG), parent,
        CommandDialogProc, reinterpret_cast<LPARAM>(this)) == IDOK;
}





INT_PTR CALLBACK CommandDialogProc(HWND dialog, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LONG data = GetWindowLong(dialog, GWL_USERDATA);
    CommandDialog* this_ = reinterpret_cast<CommandDialog*>(data);

    switch(msg)
    {
    case WM_INITDIALOG:
        {
            DialogUtil::CenterInParent(dialog);

            CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
            SetWindowLong(dialog, GWL_USERDATA, static_cast<LONG>(lparam));

            this_ = reinterpret_cast<CommandDialog*>(lparam);
            this_->dialog_ = dialog;
            this_->OnInitDialog();

            return TRUE;        // Let the system call SetFocus()
        }
    case WM_CLOSE:
        EndDialog(dialog, IDCANCEL);
        break;
    case WM_COMMAND:
        switch(LOWORD(wparam))
        {
        case IDC_COMMANDTYPE:
            if(HIWORD(wparam) == CBN_SELCHANGE)
            {
                this_->OnComboCommandTypeChanged();
            }
            break;
        case IDOK:
            this_->OnButtonOK();
            break;
        case IDCANCEL:
            EndDialog(dialog, IDCANCEL);
            break;
        }
        break;
    case WM_NOTIFY:
        {
            NMHDR* header_info = reinterpret_cast<NMHDR*>(lparam);
            switch(header_info->code)
            {
            case NM_RETURN:
            case NM_CLICK:
                {
                    NMLINK* link = reinterpret_cast<NMLINK*>(lparam);
                    this_->OnClickLinkControl(link->item.szID);
                    break;
                }
            }
            break;
        }
    }

    return FALSE;
}




void CommandDialog::OnInitDialog()
{
    RECT rect;
    GetWindowRect(GetDlgItem(dialog_, IDC_LINKAREA), &rect);
    int width   = rect.right - rect.left;
    int height  = rect.bottom - rect.top;
    ScreenToClient(dialog_, (POINT*)&rect.left);
    ScreenToClient(dialog_, (POINT*)&rect.right);

    link_control_ = CreateWindowExW(
        WS_EX_CLIENTEDGE, WC_LINK, L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        rect.left, rect.top, width, height, dialog_,
        reinterpret_cast<HMENU>(IDC_LINK_ID), GetWindowInstance(dialog_), NULL);

    HWND combo = GetDlgItem(dialog_, IDC_COMMANDTYPE);

    CommandPtr cmd(new LoadImageCommand);
    LoadImageCommand* loadimage_ptr = static_cast<LoadImageCommand*>(cmd.get());
    loadimage_ptr->SetResourceContextDocumentMap(res_ctxt_map_);
    loadimage_ptr->SetImageDocumentMap(image_map_);
    loadimage_ptr->DibResources(dib_resources_);
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new LoadImageAnimationCommand);
    LoadImageAnimationCommand* loadimageanimation_ptr = static_cast<LoadImageAnimationCommand*>(cmd.get());
    loadimageanimation_ptr->SetResourceContextDocumentMap(res_ctxt_map_);
    loadimageanimation_ptr->SetImageAnimationDocumentMap(image_anim_map_);
    loadimageanimation_ptr->DibResources(dib_resources_);
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new LoadSoundCommand);
    LoadSoundCommand* loadsound_ptr = static_cast<LoadSoundCommand*>(cmd.get());
    loadsound_ptr->SetResourceContextDocumentMap(res_ctxt_map_);
    loadsound_ptr->ContentDirectory(content_dir_);
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new LoadMusicCommand);
    LoadMusicCommand* loadmusic_ptr = static_cast<LoadMusicCommand*>(cmd.get());
    loadmusic_ptr->SetResourceContextDocumentMap(res_ctxt_map_);
    loadmusic_ptr->ContentDirectory(content_dir_);
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new LoadEntityCommand);
    LoadEntityCommand* loadentity_ptr = static_cast<LoadEntityCommand*>(cmd.get());
    loadentity_ptr->SetResourceContextDocumentMap(res_ctxt_map_);
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new LoadResourceContextCommand);
    LoadResourceContextCommand* loadresourcecontext_ptr = static_cast<LoadResourceContextCommand*>(cmd.get());
    loadresourcecontext_ptr->SetResourceContextDocumentMap(res_ctxt_map_);
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new LoadCellMapCommand);
    LoadCellMapCommand* loadcellmap_ptr = static_cast<LoadCellMapCommand*>(cmd.get());
    loadcellmap_ptr->SetCellMapDocumentList(cell_map_list_);
    loadcellmap_ptr->SetResourceContextDocumentMap(res_ctxt_map_);
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new UnloadImageCommand);
    UnloadImageCommand* unloadimage_ptr = static_cast<UnloadImageCommand*>(cmd.get());
    unloadimage_ptr->SetResourceContextDocumentMap(res_ctxt_map_);
    unloadimage_ptr->SetImageDocumentMap(image_map_);
    unloadimage_ptr->DibResources(dib_resources_);
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new UnloadImageAnimationCommand);
    UnloadImageAnimationCommand* unloadimageanimation_ptr = static_cast<UnloadImageAnimationCommand*>(cmd.get());
    unloadimageanimation_ptr->SetResourceContextDocumentMap(res_ctxt_map_);
    unloadimageanimation_ptr->SetImageAnimationDocumentMap(image_anim_map_);
    unloadimageanimation_ptr->DibResources(dib_resources_);
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new UnloadSoundCommand);
    UnloadSoundCommand* unloadsound_ptr = static_cast<UnloadSoundCommand*>(cmd.get());
    unloadsound_ptr->SetResourceContextDocumentMap(res_ctxt_map_);
    unloadsound_ptr->ContentDirectory(content_dir_);
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new UnloadMusicCommand);
    UnloadMusicCommand* unloadmusic_ptr = static_cast<UnloadMusicCommand*>(cmd.get());
    unloadmusic_ptr->SetResourceContextDocumentMap(res_ctxt_map_);
    unloadmusic_ptr->ContentDirectory(content_dir_);
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new UnloadEntityCommand);
    UnloadEntityCommand* unloadentity_ptr = static_cast<UnloadEntityCommand*>(cmd.get());
    unloadentity_ptr->SetResourceContextDocumentMap(res_ctxt_map_);
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new UnloadResourceContextCommand);
    UnloadResourceContextCommand* unloadresourcecontext_ptr = static_cast<UnloadResourceContextCommand*>(cmd.get());
    unloadresourcecontext_ptr->SetResourceContextDocumentMap(res_ctxt_map_);
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new UnloadCellMapCommand);
    UnloadCellMapCommand* unloadcellmap_ptr = static_cast<UnloadCellMapCommand*>(cmd.get());
    unloadcellmap_ptr->SetCellMapDocumentList(cell_map_list_);
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new ThinkCellMapCommand);
    ThinkCellMapCommand* thinkcellmap_ptr = static_cast<ThinkCellMapCommand*>(cmd.get());
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new DrawCellMapCommand);
    DrawCellMapCommand* drawcellmap_ptr = static_cast<DrawCellMapCommand*>(cmd.get());
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new ActivateBindingsCommand);
    ActivateBindingsCommand* activatebindings_ptr = static_cast<ActivateBindingsCommand*>(cmd.get());
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new DeactivateBindingsCommand);
    DeactivateBindingsCommand* deactivatebindings_ptr = static_cast<DeactivateBindingsCommand*>(cmd.get());
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new SetCurrentCameraCommand);
    SetCurrentCameraCommand* setcurrentcamera_ptr = static_cast<SetCurrentCameraCommand*>(cmd.get());
    setcurrentcamera_ptr->SetCameraDocuments(cam_map_);
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    cmd.reset(new CameraToggleCollisionCommand);
    CameraToggleCollisionCommand* cameratogglecollision_ptr = static_cast<CameraToggleCollisionCommand*>(cmd.get());
    cmd_map_[cmd->Name()] = cmd;
    ComboBox_AddString(combo, cmd->Name().c_str());

    if(command_.empty())
    {
        ComboBox_SetCurSel(combo, 0);
        OnComboCommandTypeChanged();
    }
    else
    {
        ComboBox_SelectString(combo, -1, command_.c_str());

        std::vector<std::string> params;
        if(parameters_.empty())
        {
            params = std::vector<std::string>(6, "Click Me");
        }
        else
        {
            params = parameters_;
        }

        CommandMap::iterator itor = cmd_map_.find(command_);
        if(itor != cmd_map_.end())
        {
            itor->second->Parameters(params);
            std::wstring text = itor->second->BuildLinkText();
            SetDlgItemTextW(dialog_, IDC_LINK_ID, text.c_str());
        }
    }
}

void CommandDialog::OnButtonOK()
{
    EndDialog(dialog_, IDOK);
}

void CommandDialog::OnComboCommandTypeChanged()
{
    char buffer[128];
    GetDlgItemText(dialog_, IDC_COMMANDTYPE, buffer, 128);
    command_ = buffer;
    parameters_.clear();

    CommandMap::iterator itor = cmd_map_.find(command_);
    if(itor != cmd_map_.end())
    {
        std::vector<std::string> params(6, "Click Me");
        itor->second->Parameters(params);
        std::wstring text = itor->second->BuildLinkText();
        SetDlgItemTextW(dialog_, IDC_LINK_ID, text.c_str());
    }
}
    

void CommandDialog::OnClickLinkControl(const std::wstring& link_id)
{
    char buffer[128];
    GetDlgItemText(dialog_, IDC_COMMANDTYPE, buffer, 128);
    command_ = buffer;

    CommandMap::iterator itor = cmd_map_.find(command_);
    if(itor != cmd_map_.end())
    {
        std::string chosen_item = itor->second->RunChooseDialog(GetWindowInstance(dialog_), dialog_, link_id);
        if(!chosen_item.empty())
        {
            std::wstring link_text = itor->second->BuildLinkText();
            SetDlgItemTextW(dialog_, IDC_LINK_ID, link_text.c_str());

            parameters_.clear();
            parameters_ = itor->second->Parameters();
        }
    }
}
