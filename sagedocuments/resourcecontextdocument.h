#ifndef INCLUDED_MYTEST_RESOURCECONTEXTDOCUMENT
#define INCLUDED_MYTEST_RESOURCECONTEXTDOCUMENT

#include <string>
#include <map>

namespace MyTest
{

enum ResourceType { RT_IMAGE, RT_IMAGE_ANIMATION, RT_SOUND, RT_MUSIC, RT_TEXTURE };

struct ResourceInfo
{
    ResourceType type_;
    int list_box_item_;
};
typedef std::map<std::string, ResourceInfo> ResourceContextDocument;
typedef std::map<std::string, ResourceContextDocument> ResourceContextDocumentMap;

}       // namespace MyTest

#endif  // INCLUDED_MYTEST_RESOURCECONTEXTDOCUMENT
