#ifndef INCLUDED_STATEMACHINEDOCUMENT
#define INCLUDED_STATEMACHINEDOCUMENT

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>

#include <string>
#include <vector>
#include <map>

class SmEntryDocument
{
public:
    void Id(int id)                         { id_ = id; }
    int Id() const                          { return id_; }
    void Position(const POINT& position)    { position_ = position; }
    const POINT& Position() const           { return position_; }
private:
    int id_;
    POINT position_;
};
typedef std::vector<SmEntryDocument> SmEntryDocumentList;

class SmExitDocument
{
public:
    void Id(int id)                         { id_ = id; }
    int Id() const                          { return id_; }
    void Position(const POINT& position)    { position_ = position; }
    const POINT& Position() const           { return position_; }
private:
    int id_;
    POINT position_;
};
typedef std::vector<SmExitDocument> SmExitDocumentList;

typedef std::vector<std::string> DocCommandLine;
typedef std::vector<DocCommandLine> DocCommandLineSet;
typedef std::map<std::string, DocCommandLineSet> DocCommandLineSets;

class SmStateDocument
{
public:
    SmStateDocument();
    void Id(int id)                         { id_ = id; }
    int Id() const                          { return id_; }
    void Name(const std::string& name)      { name_ = name; }
    const std::string& Name() const         { return name_; }
    void Position(const POINT& position)    { position_ = position; }
    const POINT& Position() const           { return position_; }
    DocCommandLineSets& EventCommandLineSets() const   { return (DocCommandLineSets&)command_line_sets_; }
private:
    int id_;
    POINT position_;
    std::string name_;
    DocCommandLineSets command_line_sets_;
};
typedef std::vector<SmStateDocument> SmStateDocumentList;

class SmEventDocument
{
public:
    void Id(int id)                             { id_ = id; }
    int Id() const                              { return id_; }
    void Name(const std::string& name)          { name_ = name; }
    const std::string& Name() const             { return name_; }
    void Position(const POINT& position)        { position_ = position; }
    const POINT& Position() const               { return position_; }
    void SourceNodeId(int id)                   { source_id_ = id; }
    int SourceNodeId() const                    { return source_id_; }
    void DestNodeId(int id)                     { dest_id_ = id; }
    int DestNodeId() const                      { return dest_id_; }
private:
    int id_;
    POINT position_;
    std::string name_;
    int source_id_;
    int dest_id_;
};
typedef std::vector<SmEventDocument> SmEventDocumentList;

class StateMachineDocument
{
public:
    StateMachineDocument() { Clear(); }
    void Clear();

    SmEntryDocumentList& EntryDocuments() const     { return (SmEntryDocumentList&)entry_documents_; }
    SmExitDocumentList& ExitDocuments() const       { return (SmExitDocumentList&)exit_documents_; }
    SmStateDocumentList& StateDocuments() const     { return (SmStateDocumentList&)state_documents_; }
    SmEventDocumentList& EventDocuments() const     { return (SmEventDocumentList&)event_documents_; }

private:
    SmEntryDocumentList entry_documents_;
    SmExitDocumentList  exit_documents_;
    SmStateDocumentList state_documents_;
    SmEventDocumentList event_documents_;
};
typedef std::map<std::string, StateMachineDocument> StateMachineDocumentMap;

#endif  // INCLUDED_STATEMACHINEDOCUMENT
