#ifndef INCLUDED_GAMEDOCUMENT
#define INCLUDED_GAMEDOCUMENT

#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "statemachinedocument.h"
#include "resourcecontextdocument.h"
#include "imagedocument.h"
#include "imageanimationdocument.h"
#include "cellmapdocument.h"
#include "entitydocument.h"
#include "cameradocument.h"
#include "bindingsfile.h"
#include "texturesetdocument.h"

#include "../math/vector.h"
#include "../gfx/color.h"

class GameDocument
{
public:
    GameDocument();
    ~GameDocument();

    void New(const std::string& name);
    bool IsNew() const { return new_; }

    enum OpenError { OE_OK, OE_OPEN_FAILURE, OE_INVALID_FILE };
    OpenError Open(const std::string& filename, const std::string& content_dir);
    bool Open() const { return (new_ || !filename_.empty()); }

    enum SaveError { SE_OK, SE_OPEN_FAILURE };
    SaveError Save(const std::string& filename, const std::string& content_dir);

    void Close();

    void Modified(bool modified)    { modified_ = modified; }
    bool Modified() const           { return Open() && modified_; }

    void Name(const std::string& name)                  { name_ = name; }
    void StateMachine(const std::string& state_machine) { state_machine_ = state_machine; }
    void ClearBackBuffer(bool clear_back_buffer)        { clear_back_buffer_ = clear_back_buffer; }

    const std::string& Filename() const     { return filename_; }
    const std::string& Name() const         { return name_; }
    const std::string& StateMachine() const { return state_machine_; }
    bool ClearBackBuffer() const            { return clear_back_buffer_; }

    void UpdateResourceContexts(const std::string& old_name, const std::string& new_name);

    StateMachineDocumentMap& StateMachineDocuments() const                  { return (StateMachineDocumentMap&)state_machine_documents_; }
    MyTest::ResourceContextDocumentMap& ResourceContextDocuments() const    { return (MyTest::ResourceContextDocumentMap&)resource_context_documents_; }
    ImageDocumentMap& ImageDocuments() const                                { return (ImageDocumentMap&)image_documents_; }
    ImageAnimationDocumentMap& ImageAnimationDocuments() const              { return (ImageAnimationDocumentMap&)image_animation_documents_; }
    CellMapDocumentList& CellMapDocuments() const                           { return (CellMapDocumentList&)cell_map_documents_; }
    EntityDocumentMap& EntityDocuments() const                              { return (EntityDocumentMap&)entity_documents_; }
    CameraDocumentMap& CameraDocuments() const                              { return (CameraDocumentMap&)camera_documents_; }
    BindingsFile&      BindingsDocument() const                             { return (BindingsFile&)bindings_file_; }
    TextureSetDocumentMap& TextureSetDocuments() const                      { return (TextureSetDocumentMap&)textureset_documents_; }

private:
    std::vector<std::string> Split(const std::string& line);
    void DoAssignment(const std::string& variable, const std::string& value, const std::string& content_dir);
    void DoBlockAssignment(const std::string& variable, const std::vector<std::string>& assignment_lines, const std::string& content_dir);

    std::string ExtractString(const std::string& value) const;
    std::vector<std::string> ExtractStringList(const std::string& value) const;
    int ExtractInteger(const std::string& value) const;
    float ExtractFloat(const std::string& value) const;
    POINT Extract2dVector(const std::string& value) const;
    Math::Vector Extract3dVector(const std::string& value) const;
    Gfx::Color ExtractColor(const std::string& value) const;

private:
    bool new_;
    bool modified_;
    std::string name_;
    std::string filename_;
    std::string state_machine_;
    bool clear_back_buffer_;

    StateMachineDocumentMap             state_machine_documents_;
    MyTest::ResourceContextDocumentMap  resource_context_documents_;
    ImageDocumentMap                    image_documents_;
    ImageAnimationDocumentMap           image_animation_documents_;
    CellMapDocumentList                 cell_map_documents_;
    EntityDocumentMap                   entity_documents_;
    CameraDocumentMap                   camera_documents_;
    BindingsFile                        bindings_file_;
    TextureSetDocumentMap               textureset_documents_;
};

#endif  // INCLUDED_GAMEDOCUMENT
