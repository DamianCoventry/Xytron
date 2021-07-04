#include "bindingsfile.h"
#include <fstream>
#include <boost/algorithm/string.hpp>

using namespace boost::algorithm;

void BindingsFile::Load(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if(file)
    {
        input_bindings_.clear();

        std::string line;
        while(std::getline(file, line))
        {
            trim(line);
            if(!line.empty())
            {
                std::string::size_type pos = line.find_first_of(' ');
                if(pos != std::string::npos)
                {
                    std::string binding = line.substr(0, pos);
                    std::string value   = line.substr(pos+1);
                    input_bindings_[binding] = value;
                }
            }
        }
    }
}

void BindingsFile::Save(const std::string& filename)
{
    if(!input_bindings_.empty())
    {
        std::ofstream file(filename.c_str());
        if(file)
        {
            Iterator i;
            for(i = Begin(); i != End(); ++i)
            {
                file << i->first << " " << i->second << "\n";
            }
        }
    }
}

void BindingsFile::Set(const std::string& binding, const std::string& value)
{
    input_bindings_[binding] = value;
}

void BindingsFile::Clear(const std::string& binding)
{
    InputBindings::iterator i = input_bindings_.find(binding);
    if(i != input_bindings_.end())
    {
        input_bindings_.erase(i);
    }
}

void BindingsFile::ClearAll()
{
    input_bindings_.clear();
}
