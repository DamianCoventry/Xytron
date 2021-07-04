#ifndef INCLUDED_CELLMAPFILE
#define INCLUDED_CELLMAPFILE

#include <string>
#include "cellmapdocument.h"

class CellMapFile
{
public:
    CellMapFile();
    ~CellMapFile();

    bool Load(const std::string& filename, CellMapDocument* document,
        boost::function<void (int)> total_function, boost::function<void (int)> step_function);
    bool Save(const std::string& filename, CellMapDocument* document,
        boost::function<void (int)> total_function, boost::function<void (int)> step_function);

private:
    static const char IDSTR[4];
    static const unsigned int VERSION;
};

#endif  // INCLUDED_CELLMAPFILE
