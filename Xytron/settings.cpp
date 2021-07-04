#include "stdafx.h"
#include "settings.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

Settings::Settings()
: fullscreen_graphics_(true)
, sound_enabled_(true)
, music_enabled_(true)
, sound_volume_(100)
, music_volume_(100)
{
}

bool Settings::Load(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if(!file)
    {
        return false;
    }

    using namespace boost::algorithm;

    std::string line;
    while(std::getline(file, line))
    {
        trim(line);
        if(!line.empty())
        {
            std::string::size_type pos = line.find_first_of("=");
            if(pos != std::string::npos)
            {
                std::string key(line.substr(0, pos));
                trim(key);

                std::string value(line.substr(pos+1));
                trim(value);

                if(key == "FullScreenGraphics")
                {
                    fullscreen_graphics_ = iequals(value, "True");
                }
                else if(key == "SoundEnabled")
                {
                    sound_enabled_ = iequals(value, "True");
                }
                else if(key == "MusicEnabled")
                {
                    music_enabled_ = iequals(value, "True");
                }
                else if(key == "SoundVolume")
                {
                    sound_volume_ = boost::lexical_cast<unsigned long>(value);
                }
                else if(key == "MusicVolume")
                {
                    music_volume_ = boost::lexical_cast<unsigned long>(value);
                }
            }
        }
    }

    return true;
}

bool Settings::Save(const std::string& filename)
{
    std::ofstream file(filename.c_str());
    if(!file)
    {
        return false;
    }

    file << "FullScreenGraphics = " << (fullscreen_graphics_ ? "True" : "False") << "\n";
    file << "SoundEnabled = " << (sound_enabled_ ? "True" : "False") << "\n";
    file << "MusicEnabled = " << (music_enabled_ ? "True" : "False") << "\n";
    file << "SoundVolume = " << sound_volume_ << "\n";
    file << "MusicVolume = " << music_volume_ << "\n";

    return true;
}
