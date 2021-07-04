#ifndef INCLUDED_ENTITYDOCUMENT
#define INCLUDED_ENTITYDOCUMENT

#include <string>
#include <map>

class EntityDocument
{
public:
    EntityDocument() {}
    ~EntityDocument() {}
};

typedef std::map<std::string, EntityDocument> EntityDocumentMap;


#endif  // INCLUDED_ENTITYDOCUMENT
