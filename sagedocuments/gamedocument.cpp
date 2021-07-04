#include "gamedocument.h"
#include "../util/string.h"

#include <fstream>
#include <sstream>

using namespace boost::algorithm;

GameDocument::GameDocument()
: modified_(false)
, new_(false)
, clear_back_buffer_(false)
{
}

GameDocument::~GameDocument()
{
}

void GameDocument::New(const std::string& name)
{
    Close();
    name_       = name;
    new_        = true;
    modified_   = true;

    // Here I'll throw in my preferred bindings ;-)
    bindings_file_.Set("KEY_E", "Camera.Move.Forward");
    bindings_file_.Set("KEY_D", "Camera.Move.Backward");
    bindings_file_.Set("KEY_S", "Camera.Move.Left");
    bindings_file_.Set("KEY_F", "Camera.Move.Right");
    bindings_file_.Set("KEY_SPACE", "Camera.Move.Up");
    bindings_file_.Set("KEY_Z", "Camera.Move.Down");
    bindings_file_.Set("MSE_MOVE", "Camera.Look");
}

GameDocument::OpenError GameDocument::Open(const std::string& filename, const std::string& content_dir)
{
    std::ifstream file(filename.c_str());
    if(!file)
    {
        return OE_OPEN_FAILURE;
    }

    Close();
    filename_ = filename;

    std::string line;
    while(std::getline(file, line))
    {
        trim(line);
        std::vector<std::string> args(Util::Split(line));
        if(args.empty()) continue;
        if(args.size() == 3 && args[1] == "=")
        {
            // Then it's an assignment ie. a = b
            DoAssignment(args[0], args[2], content_dir);
        }
        else if(args.size() == 2 && args[1] == "=")
        {
            std::string variable = args[0];
            // Check to see if the next line is a block open line
            if(!std::getline(file, line))
            {
                break;  // eof
            }
            trim(line);
            if(line == "{")
            {
                std::vector<std::string> assignment_lines;
                // Read until a closing brace
                if(!std::getline(file, line))
                {
                    break;  // eof
                }
                trim(line);
                bool eof = false;
                while(line != "}" && !eof)
                {
                    assignment_lines.push_back(line);
                    if(!std::getline(file, line))
                    {
                        eof = true;
                    }
                    else
                    {
                        trim(line);
                    }
                }
                if(eof)
                {
                    break;
                }
                DoBlockAssignment(variable, assignment_lines, content_dir);
            }
        }
    }

    bindings_file_.Load(content_dir + "\\" + name_ + ".bindings");

    new_ = false;
    modified_ = false;
    return OE_OK;
}

GameDocument::SaveError GameDocument::Save(const std::string& filename, const std::string& content_dir)
{
    filename_ = filename;

    std::ofstream file(filename.c_str());
    if(!file)
    {
        return SE_OPEN_FAILURE;
    }

    file << "Game.Name = \"" << name_ << "\"\n";
    file << "Game.StateMachine = \"" << state_machine_ << "\"\n";
    file << "Game.ClearBackBuffer = " << (clear_back_buffer_ ? "True" : "False") << "\n";

    // State machines
    StateMachineDocumentMap::iterator sm_itor;
    for(sm_itor = state_machine_documents_.begin(); sm_itor != state_machine_documents_.end(); ++sm_itor)
    {
        SmEntryDocumentList& entry_docs = sm_itor->second.EntryDocuments();
        SmEntryDocumentList::iterator entry_itor;
        for(entry_itor = entry_docs.begin(); entry_itor != entry_docs.end(); ++entry_itor)
        {
            file << "StateMachine[\"" << sm_itor->first << "\"].EntryNodes[" << entry_itor->Id() << "].Position = (" << entry_itor->Position().x << "," << entry_itor->Position().y << ")\n";
        }

        SmExitDocumentList& exit_docs = sm_itor->second.ExitDocuments();
        SmExitDocumentList::iterator exit_itor;
        for(exit_itor = exit_docs.begin(); exit_itor != exit_docs.end(); ++exit_itor)
        {
            file << "StateMachine[\"" << sm_itor->first << "\"].ExitNodes[" << exit_itor->Id() << "].Position = (" << exit_itor->Position().x << "," << exit_itor->Position().y << ")\n";
        }

        SmStateDocumentList& state_docs = sm_itor->second.StateDocuments();
        SmStateDocumentList::iterator state_itor;
        for(state_itor = state_docs.begin(); state_itor != state_docs.end(); ++state_itor)
        {
            file << "StateMachine[\"" << sm_itor->first << "\"].StateNodes[" << state_itor->Id() << "].Position = (" << state_itor->Position().x << "," << state_itor->Position().y << ")\n";
            file << "StateMachine[\"" << sm_itor->first << "\"].StateNodes[" << state_itor->Id() << "].Name = \"" << state_itor->Name() << "\"\n";

            DocCommandLineSets& ecls = state_itor->EventCommandLineSets();
            DocCommandLineSets::iterator ecls_itor;
            for(ecls_itor = ecls.begin(); ecls_itor != ecls.end(); ++ecls_itor)
            {
                DocCommandLineSet& cls = ecls_itor->second;
                file << "StateMachine[\"" << sm_itor->first << "\"].StateNodes[" << state_itor->Id() << "].CommandLineSet[\"" << ecls_itor->first << "\"] =\n"
                     << "{\n";

                DocCommandLineSet::iterator cls_itor;
                for(cls_itor = cls.begin(); cls_itor != cls.end(); ++cls_itor)
                {
                    if(!cls_itor->empty())
                    {
                        DocCommandLine::iterator cl_itor = cls_itor->begin();
                        file << *cl_itor;
                        ++cl_itor;

                        for( ; cl_itor != cls_itor->end(); ++cl_itor)
                        {
                            file << " \"" << *cl_itor << "\"";
                        }

                        file << "\n";
                    }
                }

                file << "}\n";
            }
        }

        SmEventDocumentList& event_docs = sm_itor->second.EventDocuments();
        SmEventDocumentList::iterator event_itor;
        for(event_itor = event_docs.begin(); event_itor != event_docs.end(); ++event_itor)
        {
            file << "StateMachine[\"" << sm_itor->first << "\"].EventNodes[" << event_itor->Id() << "].Position = (" << event_itor->Position().x << "," << event_itor->Position().y << ")\n";
            file << "StateMachine[\"" << sm_itor->first << "\"].EventNodes[" << event_itor->Id() << "].Name = \"" << event_itor->Name() << "\"\n";
            file << "StateMachine[\"" << sm_itor->first << "\"].EventNodes[" << event_itor->Id() << "].SourceNode.Id = " << event_itor->SourceNodeId() << "\n";
            file << "StateMachine[\"" << sm_itor->first << "\"].EventNodes[" << event_itor->Id() << "].DestNode.Id = " << event_itor->DestNodeId() << "\n";
        }
    }

    // Resouce contexts
    MyTest::ResourceContextDocumentMap::iterator rc_itor;
    for(rc_itor = resource_context_documents_.begin(); rc_itor != resource_context_documents_.end(); ++rc_itor)
    {
        file << "ResourceContext[\"" << rc_itor->first << "\"] = (";
        MyTest::ResourceContextDocument& rc_doc = rc_itor->second;
        MyTest::ResourceContextDocument::iterator rc_doc_itor = rc_doc.begin();
        while(rc_doc_itor != rc_doc.end())
        {
            switch(rc_doc_itor->second.type_)
            {
            case MyTest::RT_IMAGE:              file << "Image:\"" << rc_doc_itor->first << "\""; break;
            case MyTest::RT_IMAGE_ANIMATION:    file << "ImageAnim:\"" << rc_doc_itor->first << "\""; break;
            case MyTest::RT_SOUND:              file << "Sound:\"" << rc_doc_itor->first << "\""; break;
            case MyTest::RT_MUSIC:              file << "Music:\"" << rc_doc_itor->first << "\""; break;
            case MyTest::RT_TEXTURE:            file << "Texture:\"" << rc_doc_itor->first << "\""; break;
            }
            ++rc_doc_itor;
            if(rc_doc_itor != rc_doc.end()) file << ",";
        }
        file << ")\n";
    }

    // Images
    ImageDocumentMap::iterator img_itor;
    for(img_itor = image_documents_.begin(); img_itor != image_documents_.end(); ++img_itor)
    {
        file << "Image[\"" << img_itor->first << "\"].FrameX = " << img_itor->second.FrameX() << "\n";
        file << "Image[\"" << img_itor->first << "\"].FrameY = " << img_itor->second.FrameY() << "\n";
        file << "Image[\"" << img_itor->first << "\"].FrameWidth = " << img_itor->second.FrameWidth() << "\n";
        file << "Image[\"" << img_itor->first << "\"].FrameHeight = " << img_itor->second.FrameHeight() << "\n";
        file << "Image[\"" << img_itor->first << "\"].TextureFilename = \"" << img_itor->second.TextureFilename() << "\"\n";
    }

    // Image Animations
    ImageAnimationDocumentMap::iterator img_anim_itor;
    for(img_anim_itor = image_animation_documents_.begin(); img_anim_itor != image_animation_documents_.end(); ++img_anim_itor)
    {
        file << "ImageAnimation[\"" << img_anim_itor->first << "\"].StartX = " << img_anim_itor->second.StartX() << "\n";
        file << "ImageAnimation[\"" << img_anim_itor->first << "\"].StartY = " << img_anim_itor->second.StartY() << "\n";
        file << "ImageAnimation[\"" << img_anim_itor->first << "\"].FrameWidth = " << img_anim_itor->second.FrameWidth() << "\n";
        file << "ImageAnimation[\"" << img_anim_itor->first << "\"].FrameHeight = " << img_anim_itor->second.FrameHeight() << "\n";
        file << "ImageAnimation[\"" << img_anim_itor->first << "\"].FrameCount = " << img_anim_itor->second.FrameCount() << "\n";
        file << "ImageAnimation[\"" << img_anim_itor->first << "\"].TextureFilename = \"" << img_anim_itor->second.TextureFilename() << "\"\n";
    }

    // Cell Maps
    CellMapDocumentList::iterator cell_map_itor;
    int index = 0;
    for(cell_map_itor = cell_map_documents_.begin(); cell_map_itor != cell_map_documents_.end(); ++cell_map_itor)
    {
        file << "CellMap[" << index++ << "] = \"" << *cell_map_itor << "\"\n";
    }

    // Cameras
    CameraDocumentMap::iterator cam_itor;
    for(cam_itor = camera_documents_.begin(); cam_itor != camera_documents_.end(); ++cam_itor)
    {
        CameraDocument& c = cam_itor->second;
        file << "Camera[\"" << c.Name() << "\"].Position = (" << c.Position().x_ << "," << c.Position().y_ << "," << c.Position().z_ << ")\n";
        file << "Camera[\"" << c.Name() << "\"].Rotation = (" << c.Rotation().x_ << "," << c.Rotation().y_ << "," << c.Rotation().z_ << ")\n";
    }

    // Texture Sets
    TextureSetDocumentMap::iterator ts_itor;
    for(ts_itor = textureset_documents_.begin(); ts_itor != textureset_documents_.end(); ++ts_itor)
    {
        TextureSetDocument& ts = ts_itor->second;
        file << "TextureSet[\"" << ts_itor->first << "\"].ResCxt = \"" << ts.ResCxt() << "\"\n";
        file << "TextureSet[\"" << ts_itor->first << "\"].TexWall = \"" << ts.TexWall() << "\"\n";
        file << "TextureSet[\"" << ts_itor->first << "\"].TexCeiling = \"" << ts.TexCeiling() << "\"\n";
        file << "TextureSet[\"" << ts_itor->first << "\"].TexFloor = \"" << ts.TexFloor() << "\"\n";
        file << "TextureSet[\"" << ts_itor->first << "\"].TexLiquid = \"" << ts.TexLiquid() << "\"\n";
        file << "TextureSet[\"" << ts_itor->first << "\"].TexDoor = \"" << ts.TexDoor() << "\"\n";
        file << "TextureSet[\"" << ts_itor->first << "\"].TexTransWall = \"" << ts.TexTransWall() << "\"\n";
        file << "TextureSet[\"" << ts_itor->first << "\"].TexTransDoor = \"" << ts.TexTransDoor() << "\"\n";
        file << "TextureSet[\"" << ts_itor->first << "\"].LiquidColor = (" << ts.LiquidColor().r_ << "," << ts.LiquidColor().g_ << "," << ts.LiquidColor().b_ << ")\n";
    }

    bindings_file_.Save(content_dir + "\\" + name_ + ".bindings");

    modified_ = false;
    new_ = false;
    return SE_OK;
}

void GameDocument::Close()
{
    name_.clear();
    filename_.clear();
    state_machine_.clear();

    state_machine_documents_.clear();
    resource_context_documents_.clear();
    image_documents_.clear();
    image_animation_documents_.clear();
    cell_map_documents_.clear();
    entity_documents_.clear();
    camera_documents_.clear();
    bindings_file_.ClearAll();

    modified_           = false;
    new_                = false;
    clear_back_buffer_  = false;
}

void GameDocument::UpdateResourceContexts(const std::string& old_name, const std::string& new_name)
{
    // This image/image anim may be referenced in one or more resource contexts, so we have to
    // find each instance and renamed it.

    MyTest::ResourceContextDocumentMap::iterator rc_itor;
    for(rc_itor = resource_context_documents_.begin(); rc_itor != resource_context_documents_.end(); ++rc_itor)
    {
        MyTest::ResourceContextDocument& rc_doc = rc_itor->second;
        MyTest::ResourceContextDocument::iterator doc_itor = rc_doc.find(old_name);
        if(doc_itor != rc_doc.end())
        {
            MyTest::ResourceInfo copy = doc_itor->second;
            rc_doc.erase(doc_itor);
            rc_doc.insert(std::make_pair(new_name, copy));
        }
    }
}

void GameDocument::DoAssignment(const std::string& variable, const std::string& value, const std::string& content_dir)
{
    std::vector<std::string> output;
    split(output, variable, is_any_of("[]."));
    if(output.empty())
    {
        return;
    }

    if(output[0] == "Game")
    {
        if(output[1] == "Name")
        {
            name_ = value;
            trim_if(name_, is_any_of("\""));
        }
        else if(output[1] == "StateMachine")
        {
            state_machine_ = value;
            trim_if(state_machine_, is_any_of("\""));
        }
        else if(output[1] == "ClearBackBuffer")
        {
            clear_back_buffer_ = iequals(value, "True");
        }
    }
    else if(output[0] == "StateMachine" && output.size() >= 7)
    {
        std::string sm_name = output[1];
        trim_if(sm_name, is_any_of("\""));

        // Get an iterator to this state machine, create it if it doesn't exist.
        StateMachineDocumentMap::iterator sm_itor = state_machine_documents_.find(sm_name);
        if(sm_itor == state_machine_documents_.end())
        {
            state_machine_documents_[sm_name] = StateMachineDocument();
            sm_itor = state_machine_documents_.find(sm_name);
        }

        int id = boost::lexical_cast<int>(output[4]);

        if(output[3] == "EntryNodes")
        {
            SmEntryDocumentList& entry_docs = sm_itor->second.EntryDocuments();
            // Get an iterator to this node, create it if it doesn't exist.
            SmEntryDocumentList::iterator node_itor;
            for(node_itor = entry_docs.begin(); node_itor != entry_docs.end(); ++node_itor)
            {
                if(node_itor->Id() == id) break;
            }
            if(node_itor == entry_docs.end())
            {
                SmEntryDocument doc;
                doc.Id(id);
                entry_docs.push_back(doc);
                for(node_itor = entry_docs.begin(); node_itor != entry_docs.end(); ++node_itor)
                {
                    if(node_itor->Id() == id) break;
                }
            }
            if(output[6] == "Position")
            {
                node_itor->Position(Extract2dVector(value));
            }
        }
        else if(output[3] == "ExitNodes")
        {
            SmExitDocumentList& exit_docs = sm_itor->second.ExitDocuments();
            // Get an iterator to this node, create it if it doesn't exist.
            SmExitDocumentList::iterator node_itor;
            for(node_itor = exit_docs.begin(); node_itor != exit_docs.end(); ++node_itor)
            {
                if(node_itor->Id() == id) break;
            }
            if(node_itor == exit_docs.end())
            {
                SmExitDocument doc;
                doc.Id(id);
                exit_docs.push_back(doc);
                for(node_itor = exit_docs.begin(); node_itor != exit_docs.end(); ++node_itor)
                {
                    if(node_itor->Id() == id) break;
                }
            }
            if(output[6] == "Position")
            {
                node_itor->Position(Extract2dVector(value));
            }
        }
        else if(output[3] == "StateNodes")
        {
            SmStateDocumentList& state_docs = sm_itor->second.StateDocuments();
            // Get an iterator to this node, create it if it doesn't exist.
            SmStateDocumentList::iterator node_itor;
            for(node_itor = state_docs.begin(); node_itor != state_docs.end(); ++node_itor)
            {
                if(node_itor->Id() == id) break;
            }
            if(node_itor == state_docs.end())
            {
                SmStateDocument doc;
                doc.Id(id);
                state_docs.push_back(doc);
                for(node_itor = state_docs.begin(); node_itor != state_docs.end(); ++node_itor)
                {
                    if(node_itor->Id() == id) break;
                }
            }
            if(output[6] == "Position")
            {
                node_itor->Position(Extract2dVector(value));
            }
            else if(output[6] == "Name")
            {
                node_itor->Name(ExtractString(value));
            }
        }
        else if(output[3] == "EventNodes")
        {
            SmEventDocumentList& event_docs = sm_itor->second.EventDocuments();
            // Get an iterator to this node, create it if it doesn't exist.
            SmEventDocumentList::iterator node_itor;
            for(node_itor = event_docs.begin(); node_itor != event_docs.end(); ++node_itor)
            {
                if(node_itor->Id() == id) break;
            }
            if(node_itor == event_docs.end())
            {
                SmEventDocument doc;
                doc.Id(id);
                event_docs.push_back(doc);
                for(node_itor = event_docs.begin(); node_itor != event_docs.end(); ++node_itor)
                {
                    if(node_itor->Id() == id) break;
                }
            }
            if(output[6] == "Position")
            {
                node_itor->Position(Extract2dVector(value));
            }
            else if(output[6] == "Name")
            {
                node_itor->Name(ExtractString(value));
            }
            else if(output[6] == "SourceNode" && output.size() >= 8)
            {
                if(output[7] == "Id")
                {
                    node_itor->SourceNodeId(ExtractInteger(value));
                }
            }
            else if(output[6] == "DestNode" && output.size() >= 8)
            {
                if(output[7] == "Id")
                {
                    node_itor->DestNodeId(ExtractInteger(value));
                }
            }
        }
    }
    else if(output[0] == "ResourceContext" && output.size() >= 2)
    {
        std::string rc_name = output[1];
        trim_if(rc_name, is_any_of("\""));

        resource_context_documents_[rc_name] = MyTest::ResourceContextDocument();
        MyTest::ResourceContextDocumentMap::iterator rc_itor = resource_context_documents_.find(rc_name);

        std::vector<std::string> strings = ExtractStringList(value);
        std::vector<std::string>::iterator itor;
        for(itor = strings.begin(); itor != strings.end(); ++itor)
        {
            std::vector<std::string> kv_pair;
            split(kv_pair, *itor, is_any_of(":"));
            if(kv_pair.size() == 2)
            {
                MyTest::ResourceInfo info;
                info.list_box_item_ = -1;
                if(kv_pair[0] == "Image") info.type_ = MyTest::RT_IMAGE;
                else if(kv_pair[0] == "ImageAnim") info.type_ = MyTest::RT_IMAGE_ANIMATION;
                else if(kv_pair[0] == "Sound") info.type_ = MyTest::RT_SOUND;
                else if(kv_pair[0] == "Music") info.type_ = MyTest::RT_MUSIC;
                else if(kv_pair[0] == "Texture") info.type_ = MyTest::RT_TEXTURE;
                std::string name(kv_pair[1]);
                trim_if(name, is_any_of("\""));
                rc_itor->second.insert(std::make_pair(name, info));
            }
        }
    }
    else if(output[0] == "Image" && output.size() >= 4)
    {
        std::string img_name = output[1];
        trim_if(img_name, is_any_of("\""));

        // Get an iterator to this state machine, create it if it doesn't exist.
        ImageDocumentMap::iterator img_itor = image_documents_.find(img_name);
        if(img_itor == image_documents_.end())
        {
            image_documents_[img_name] = ImageDocument();
            img_itor = image_documents_.find(img_name);
        }

        if(output[3] == "FrameX")
        {
            img_itor->second.FrameX(ExtractInteger(value));
        }
        else if(output[3] == "FrameY")
        {
            img_itor->second.FrameY(ExtractInteger(value));
        }
        else if(output[3] == "FrameWidth")
        {
            img_itor->second.FrameWidth(ExtractInteger(value));
        }
        else if(output[3] == "FrameHeight")
        {
            img_itor->second.FrameHeight(ExtractInteger(value));
        }
        else if(output[3] == "TextureFilename")
        {
            img_itor->second.TextureFilename(ExtractString(value));
        }
    }
    else if(output[0] == "ImageAnimation" && output.size() >= 4)
    {
        std::string img_anim_name = output[1];
        trim_if(img_anim_name, is_any_of("\""));

        // Get an iterator to this state machine, create it if it doesn't exist.
        ImageAnimationDocumentMap::iterator img_anim_itor = image_animation_documents_.find(img_anim_name);
        if(img_anim_itor == image_animation_documents_.end())
        {
            image_animation_documents_[img_anim_name] = ImageAnimationDocument();
            img_anim_itor = image_animation_documents_.find(img_anim_name);
        }

        if(output[3] == "StartX")
        {
            img_anim_itor->second.StartX(ExtractInteger(value));
        }
        else if(output[3] == "StartY")
        {
            img_anim_itor->second.StartY(ExtractInteger(value));
        }
        else if(output[3] == "FrameWidth")
        {
            img_anim_itor->second.FrameWidth(ExtractInteger(value));
        }
        else if(output[3] == "FrameHeight")
        {
            img_anim_itor->second.FrameHeight(ExtractInteger(value));
        }
        else if(output[3] == "FrameCount")
        {
            img_anim_itor->second.FrameCount(ExtractInteger(value));
        }
        else if(output[3] == "TextureFilename")
        {
            img_anim_itor->second.TextureFilename(ExtractString(value));
        }
    }
    else if(output[0] == "CellMap")
    {
        std::string name = value;
        trim_if(name, is_any_of("\""));

        // Don't add duplicates
        CellMapDocumentList::iterator i = std::find(cell_map_documents_.begin(), cell_map_documents_.end(), name);
        if(i == cell_map_documents_.end())
        {
            cell_map_documents_.push_back(name);
        }
    }
    else if(output[0] == "Camera")
    {
        std::string name = output[1];
        trim_if(name, is_any_of("\""));

        // Get an iterator to this state machine, create it if it doesn't exist.
        CameraDocumentMap::iterator i = camera_documents_.find(name);
        if(i == camera_documents_.end())
        {
            camera_documents_[name] = CameraDocument();
            i = camera_documents_.find(name);
        }

        i->second.Name(name);

        if(output[3] == "Position")
        {
            i->second.Position(Extract3dVector(value));
        }
        else if(output[3] == "Rotation")
        {
            i->second.Rotation(Extract3dVector(value));
        }
    }
    else if(output[0] == "TextureSet")
    {
        std::string name = output[1];
        trim_if(name, is_any_of("\""));

        // Get an iterator to this state machine, create it if it doesn't exist.
        TextureSetDocumentMap::iterator i = textureset_documents_.find(name);
        if(i == textureset_documents_.end())
        {
            textureset_documents_[name] = TextureSetDocument();
            i = textureset_documents_.find(name);
        }

        if(output[3] == "ResCxt")
        {
            i->second.ResCxt(ExtractString(value));
        }
        else if(output[3] == "TexWall")
        {
            i->second.TexWall(ExtractString(value));
        }
        else if(output[3] == "TexCeiling")
        {
            i->second.TexCeiling(ExtractString(value));
        }
        else if(output[3] == "TexFloor")
        {
            i->second.TexFloor(ExtractString(value));
        }
        else if(output[3] == "TexLiquid")
        {
            i->second.TexLiquid(ExtractString(value));
        }
        else if(output[3] == "TexDoor")
        {
            i->second.TexDoor(ExtractString(value));
        }
        else if(output[3] == "TexTransWall")
        {
            i->second.TexTransWall(ExtractString(value));
        }
        else if(output[3] == "TexTransDoor")
        {
            i->second.TexTransDoor(ExtractString(value));
        }
        else if(output[3] == "LiquidColor")
        {
            i->second.LiquidColor(ExtractColor(value));
        }
    }
    // else ...
}

std::string GameDocument::ExtractString(const std::string& value) const
{
    if(value.find_first_of('"') != std::string::npos)
    {
        std::string rv(value);
        trim_if(rv, is_any_of("\""));
        return rv;
    }
    return std::string();
}

std::vector<std::string> GameDocument::ExtractStringList(const std::string& value) const
{
    std::string no_brackets(value);
    trim_if(no_brackets, is_any_of("()"));

    std::vector<std::string> output;
    split(output, no_brackets, is_any_of(","));

    return output;
}

int GameDocument::ExtractInteger(const std::string& value) const
{
    return boost::lexical_cast<int>(value);
}

float GameDocument::ExtractFloat(const std::string& value) const
{
    return boost::lexical_cast<float>(value);
}

POINT GameDocument::Extract2dVector(const std::string& value) const
{
    std::string no_brackets(value);
    trim_if(no_brackets, is_any_of("()"));

    std::vector<std::string> output;
    split(output, no_brackets, is_any_of(","));

    POINT rv;
    rv.x = rv.y = 0;
    if(output.size() == 2)
    {
        rv.x = boost::lexical_cast<int>(output[0]);
        rv.y = boost::lexical_cast<int>(output[1]);
    }
    return rv;
}

Math::Vector GameDocument::Extract3dVector(const std::string& value) const
{
    std::string no_brackets(value);
    trim_if(no_brackets, is_any_of("()"));

    std::vector<std::string> output;
    split(output, no_brackets, is_any_of(","));

    Math::Vector rv;
    if(output.size() == 3)
    {
        rv.x_ = boost::lexical_cast<float>(output[0]);
        rv.y_ = boost::lexical_cast<float>(output[1]);
        rv.z_ = boost::lexical_cast<float>(output[2]);
    }
    return rv;
}

Gfx::Color GameDocument::ExtractColor(const std::string& value) const
{
    std::string no_brackets(value);
    trim_if(no_brackets, is_any_of("()"));

    std::vector<std::string> output;
    split(output, no_brackets, is_any_of(","));

    Gfx::Color rv;
    if(output.size() == 3)
    {
        rv.r_ = boost::lexical_cast<float>(output[0]);
        rv.g_ = boost::lexical_cast<float>(output[1]);
        rv.b_ = boost::lexical_cast<float>(output[2]);
    }
    return rv;
}

void GameDocument::DoBlockAssignment(const std::string& variable, const std::vector<std::string>& assignment_lines, const std::string& content_dir)
{
    std::vector<std::string> output;
    split(output, variable, is_any_of("[]."));
    if(output.empty())
    {
        return;
    }

    if(output[0] == "StateMachine" && output.size() >= 5)
    {
        std::string sm_name = output[1];
        trim_if(sm_name, is_any_of("\""));

        // Get an iterator to this state machine, create it if it doesn't exist.
        StateMachineDocumentMap::iterator sm_itor = state_machine_documents_.find(sm_name);
        if(sm_itor == state_machine_documents_.end())
        {
            state_machine_documents_[sm_name] = StateMachineDocument();
            sm_itor = state_machine_documents_.find(sm_name);
        }

        int id = boost::lexical_cast<int>(output[4]);

        if(output[3] == "StateNodes")
        {
            SmStateDocumentList& state_docs = sm_itor->second.StateDocuments();
            // Get an iterator to this node, create it if it doesn't exist.
            SmStateDocumentList::iterator node_itor;
            for(node_itor = state_docs.begin(); node_itor != state_docs.end(); ++node_itor)
            {
                if(node_itor->Id() == id) break;
            }
            if(node_itor == state_docs.end())
            {
                SmStateDocument doc;
                doc.Id(id);
                state_docs.push_back(doc);
                for(node_itor = state_docs.begin(); node_itor != state_docs.end(); ++node_itor)
                {
                    if(node_itor->Id() == id) break;
                }
            }
            if(output[6] == "CommandLineSet")
            {
                std::string es_name = output[7];
                trim_if(es_name, is_any_of("\""));

                DocCommandLineSets& ecls = node_itor->EventCommandLineSets();
                DocCommandLineSets::iterator ecls_itor = ecls.find(es_name);
                if(ecls_itor == ecls.end())
                {
                    ecls[es_name] = DocCommandLineSet();
                    ecls_itor = ecls.find(es_name);
                }

                DocCommandLineSet& cls = ecls_itor->second;

                std::vector<std::string>::const_iterator al_itor;
                for(al_itor = assignment_lines.begin(); al_itor != assignment_lines.end(); ++al_itor)
                {
                    DocCommandLine cl = Util::Split(*al_itor);
                    DocCommandLine::iterator cl_itor;
                    for(cl_itor = cl.begin(); cl_itor != cl.end(); ++cl_itor)
                    {
                        trim_if(*cl_itor, is_any_of("\""));
                    }
                    cls.push_back(cl);
                }
            }
        }
    }
}
