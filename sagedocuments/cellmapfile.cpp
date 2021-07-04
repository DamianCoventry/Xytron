#include "cellmapfile.h"
#include "light.h"
#include <fstream>
#include <boost/scoped_array.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

const char CellMapFile::IDSTR[4]          = { 'C', 'M', '2', 'D' };
const unsigned int CellMapFile::VERSION   = 0x0101;

CellMapFile::CellMapFile()
{
}

CellMapFile::~CellMapFile()
{
}

bool CellMapFile::Load(const std::string& filename, CellMapDocument* document,
                       boost::function<void (int)> total_function, boost::function<void (int)> step_function)
{
    std::ifstream file(filename.c_str(), std::ios_base::binary);
    if(!file)
    {
        return false;
    }

    char file_id[4];
    file.read(file_id, sizeof(char)*4);
    if(memcmp(file_id, IDSTR, sizeof(IDSTR)) != 0)
    {
        return false;
    }

    unsigned int version;
    file.read((char*)&version, sizeof(unsigned int));
    if(version != VERSION)
    {
        return false;
    }

    unsigned short length;
    file.read((char*)&length, sizeof(unsigned short));
    if(length > 0)
    {
        boost::scoped_array<char> temp(new char[length+1]);
        file.read(temp.get(), sizeof(char)*length);
        temp[length] = 0;
        document->SkyTextureFilename(temp.get());
    }

    float cell_size;
    file.read((char*)&cell_size, sizeof(float));
    document->CellSize(cell_size);

    unsigned int num_cells = 0;
    file.read((char*)&num_cells, sizeof(unsigned int));

    document->ClearAllCells();
    total_function(num_cells);

    unsigned long time_prev = timeGetTime();
    unsigned int i;
    for(i = 0; i < num_cells; i++)
    {
        CellIndex cell_index;
        file.read((char*)&cell_index, sizeof(CellIndex));

        Cell c;
        file.read((char*)&c.type_, sizeof(CellType));
        file.read((char*)&c.min_x_, sizeof(float));
        file.read((char*)&c.max_x_, sizeof(float));
        file.read((char*)&c.min_z_, sizeof(float));
        file.read((char*)&c.max_z_, sizeof(float));

        unsigned short len;
        file.read((char*)&len, sizeof(unsigned short));
        if(len > 0)
        {
            boost::scoped_array<char> temp(new char[len+1]);
            file.read(temp.get(), sizeof(char)*len);
            temp[len] = 0;
            c.ts_name_ = temp.get();
        }

        file.read((char*)&c.sky_, sizeof(bool));

        document->InsertCell(cell_index, c);

        unsigned long now = timeGetTime();
        if(now - time_prev >= 100)
        {
            step_function(i);
            time_prev = now;
        }
    }
    step_function(num_cells);

    // Lights
    char world_ambient;
    file.read((char*)&world_ambient, sizeof(char));
    document->AmbientLight((int)world_ambient);

    LightList& ll = document->GetLightList();
    ll.clear();

    file.read((char*)&num_cells, sizeof(unsigned int));
    if(num_cells > 0)
    {
        total_function(num_cells);

        time_prev = timeGetTime();
        for(i = 0; i < num_cells; i++)
        {
            Light l;

            unsigned short len;
            file.read((char*)&len, sizeof(unsigned short));
            if(len > 0)
            {
                boost::scoped_array<char> temp(new char[len+1]);
                file.read(temp.get(), sizeof(char)*len);
                temp[len] = 0;
                l.Name(temp.get());
            }

            CellIndex ci;
            file.read((char*)&ci.x_, sizeof(int));
            file.read((char*)&ci.z_, sizeof(int));
            l.SetCellIndex(ci);

            Math::Vector v;
            file.read((char*)&v.x_, sizeof(float));
            file.read((char*)&v.y_, sizeof(float));
            file.read((char*)&v.z_, sizeof(float));
            l.Position(v, cell_size);

            float f;
            file.read((char*)&f, sizeof(float));
            l.Radius(f);

            bool use;
            file.read((char*)&use, sizeof(bool));
            l.UseWorldAmbient(use);

            int ambient;
            file.read((char*)&ambient, sizeof(int));
            l.Ambient(ambient);

            Gfx::Color C;
            file.read((char*)&C, sizeof(Gfx::Color));
            l.SetColor(C);

            ll.push_back(l);

            unsigned long now = timeGetTime();
            if(now - time_prev >= 100)
            {
                step_function(i);
                time_prev = now;
            }
        }

        step_function(num_cells);
    }

    return true;
}

bool CellMapFile::Save(const std::string& filename, CellMapDocument* document,
                       boost::function<void (int)> total_function, boost::function<void (int)> step_function)
{
    std::ofstream file(filename.c_str(), std::ios_base::binary);
    if(!file)
    {
        return false;
    }

    file.write(IDSTR, sizeof(char)*4);
    file.write((char*)&VERSION, sizeof(unsigned int));

    unsigned short length = static_cast<unsigned short>(document->SkyTextureFilename().size());
    file.write((const char*)&length, sizeof(unsigned short));
    if(length > 0)
    {
        file.write(document->SkyTextureFilename().c_str(), sizeof(char)*length);
    }

    float temp = document->CellSize();
    file.write((char*)&temp, sizeof(float));

    unsigned int num_cells = static_cast<unsigned int>(document->GetCellMap().Cells_.size());
    file.write((char*)&num_cells, sizeof(unsigned int));

    total_function(num_cells + int(document->GetLightList().size()));

    unsigned long time_prev = timeGetTime();
    CellContainer::iterator cell_itor;
    int i = 0;
    for(cell_itor = document->GetCellMap().Cells_.begin(); cell_itor != document->GetCellMap().Cells_.end(); ++cell_itor, ++i)
    {
        file.write((char*)&cell_itor->first, sizeof(CellIndex));

        Cell& c = cell_itor->second;
        file.write((const char*)&c.type_, sizeof(CellType));
        file.write((const char*)&c.min_x_, sizeof(float));
        file.write((const char*)&c.max_x_, sizeof(float));
        file.write((const char*)&c.min_z_, sizeof(float));
        file.write((const char*)&c.max_z_, sizeof(float));

        unsigned short len = static_cast<unsigned short>(c.ts_name_.size());
        file.write((const char*)&len, sizeof(unsigned short));
        if(len > 0)
        {
            file.write(c.ts_name_.c_str(), sizeof(char)*len);
        }

        file.write((const char*)&c.sky_, sizeof(bool));

        unsigned long now = timeGetTime();
        if(now - time_prev >= 100)
        {
            step_function(i);
            time_prev = now;
        }
    }

    // Lights
    char world_ambient = (char)document->AmbientLight();
    file.write((char*)&world_ambient, sizeof(char));

    LightList& ll = document->GetLightList();
    num_cells = static_cast<unsigned int>(ll.size());
    file.write((char*)&num_cells, sizeof(unsigned int));

    LightList::iterator ll_itor;
    time_prev = timeGetTime();
    for(ll_itor = ll.begin(); ll_itor != ll.end(); ++ll_itor, ++i)
    {
        unsigned short len = static_cast<unsigned short>(ll_itor->Name().size());
        file.write((const char*)&len, sizeof(unsigned short));
        if(len > 0)
        {
            file.write(ll_itor->Name().c_str(), sizeof(char)*len);
        }

        file.write((const char*)&ll_itor->GetCellIndex().x_, sizeof(int));
        file.write((const char*)&ll_itor->GetCellIndex().z_, sizeof(int));
        file.write((const char*)&ll_itor->Position().x_, sizeof(float));
        file.write((const char*)&ll_itor->Position().y_, sizeof(float));
        file.write((const char*)&ll_itor->Position().z_, sizeof(float));
        float f = ll_itor->Radius();
        file.write((const char*)&f, sizeof(float));
        bool b = ll_itor->UseWorldAmbient();
        file.write((const char*)&b, sizeof(bool));
        int amb = ll_itor->Ambient();
        file.write((const char*)&amb, sizeof(int));
        Gfx::Color C = ll_itor->GetColor();
        file.write((const char*)&C, sizeof(Gfx::Color));

        unsigned long now = timeGetTime();
        if(now - time_prev >= 100)
        {
            step_function(i);
            time_prev = now;
        }
    }

    step_function(num_cells + int(document->GetLightList().size()));
    return true;
}
