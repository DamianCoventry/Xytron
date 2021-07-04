#include "stdafx.h"
#include "highscores.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <fstream>
#include <sstream>
#include <iomanip>

namespace
{
    void Encrypt(unsigned char* bytes, int num_bytes)
    {
        for(int i = 0; i < num_bytes; i++)
        {
            bytes[i] -= '0';
        }
    }

    void Decrypt(unsigned char* bytes, int num_bytes)
    {
        for(int i = 0; i < num_bytes; i++)
        {
            bytes[i] += '0';
        }
    }
}

bool Highscores::Load(const std::string& filename)
{
    std::ifstream file(filename.c_str(), std::ios_base::binary);
    if(!file)
    {
        return false;
    }

    char id[4];
    file.read(id, 4 * sizeof(char));
    if(id[0] != 'H' || id[1] != 'I' || id[2] != 'S' || id[3] != 'C')
    {
        return false;
    }

    highscore_table_.clear();
    for(int i = 0; i < NUM_HIGHSCORES; i++)
    {
        Highscore highscore;

        // Read in the player's name
        unsigned char encrypted_text[128];
        unsigned char byte;
        file.read((char*)&byte, sizeof(unsigned char));
        int length = static_cast<int>(byte);
        file.read((char*)encrypted_text, sizeof(char)*length);
        Decrypt(encrypted_text, length);
        encrypted_text[length] = 0;
        highscore.name_ = (char*)encrypted_text;

        // Read in the date/time
        file.read((char*)&byte, sizeof(unsigned char));
        length = static_cast<int>(byte);
        file.read((char*)encrypted_text, sizeof(char)*length);
        Decrypt(encrypted_text, length);
        encrypted_text[length] = 0;
        highscore.date_time_ = (char*)encrypted_text;

        // Read in the level and the score
        file.read((char*)&highscore.level_, sizeof(unsigned int));
        file.read((char*)&highscore.score_, sizeof(unsigned long));

        highscore_table_.push_back(highscore);
    }

    return true;
}

bool Highscores::Save(const std::string& filename)
{
    std::ofstream file(filename.c_str(), std::ios_base::binary);
    if(!file)
    {
        return false;
    }

    file.write("HISC", 4);
    for(int i = 0; i < NUM_HIGHSCORES; i++)
    {
        // Write out the player's name
        unsigned char encrypted_text[128];
        int length = static_cast<int>(highscore_table_[i].name_.length());
        strncpy_s((char*)encrypted_text, 128, highscore_table_[i].name_.c_str(), length);
        Encrypt(encrypted_text, length);

        unsigned char byte = static_cast<unsigned char>(length);
        file.write((char*)&byte, sizeof(unsigned char));
        file.write((char*)encrypted_text, length * sizeof(char));

        // Write out the date/time
        length = static_cast<int>(highscore_table_[i].date_time_.length());
        strncpy_s((char*)encrypted_text, 128, highscore_table_[i].date_time_.c_str(), length);
        Encrypt(encrypted_text, length);

        byte = static_cast<unsigned char>(length);
        file.write((char*)&byte, sizeof(unsigned char));
        file.write((char*)encrypted_text, length * sizeof(char));

        // Write out the level and the score
        file.write((char*)&highscore_table_[i].level_, sizeof(unsigned int));
        file.write((char*)&highscore_table_[i].score_, sizeof(unsigned long));
    }

    return true;
}

void Highscores::Reset()
{
    const std::string names[] =
    { "Damian", "Robert", "Suzanne", "Aimee", "Josh", "Claire", "Simon", "Tammy" };

    SYSTEMTIME sys_time;
    GetLocalTime(&sys_time);
    std::ostringstream time_string;
    time_string << std::setw(2) << std::setfill('0') << sys_time.wHour << ":"
                << std::setw(2) << std::setfill('0') << sys_time.wMinute << ", "
                << std::setw(2) << std::setfill('0') << sys_time.wDay << "/"
                << std::setw(2) << std::setfill('0') << sys_time.wMonth << "/"
                << sys_time.wYear;

    highscore_table_.clear();
    unsigned int level  = 30;
    unsigned long score = 50000 - ((rand()%5000)+1000);
    for(int i = 0; i < NUM_HIGHSCORES; i++)
    {
        Highscore highscore;
        highscore.name_         = names[rand()%8];
        highscore.date_time_    = time_string.str();
        highscore.level_        = level;
        highscore.score_        = score;
        highscore_table_.push_back(highscore);

        level -= rand()%3+1;
        score -= rand()%5000+1000;
    }
}

int Highscores::Insert(const std::string& name, unsigned int level, unsigned long score)
{
    HighscoreTable::iterator itor;
    for(itor = highscore_table_.begin(); itor != highscore_table_.end(); ++itor)
    {
        if(score >= itor->score_) break;
    }
    if(itor == highscore_table_.end())
    {
        return -1;
    }

    int index = static_cast<int>(std::distance(highscore_table_.begin(), itor));

    SYSTEMTIME sys_time;
    GetLocalTime(&sys_time);
    std::ostringstream time_string;
    time_string << std::setw(2) << std::setfill('0') << sys_time.wHour << ":"
                << std::setw(2) << std::setfill('0') << sys_time.wMinute << ", "
                << std::setw(2) << std::setfill('0') << sys_time.wDay << "/"
                << std::setw(2) << std::setfill('0') << sys_time.wMonth << "/"
                << sys_time.wYear;

    Highscore highscore;
    highscore.name_         = name;
    highscore.date_time_    = time_string.str();
    highscore.level_        = level;
    highscore.score_        = score;

    highscore_table_.insert(itor, highscore);
    highscore_table_.pop_back();

    return index;
}

bool Highscores::IsHighscore(unsigned long score) const
{
    HighscoreTable::const_iterator itor;
    for(itor = highscore_table_.begin(); itor != highscore_table_.end(); ++itor)
    {
        if(score >= itor->score_) break;
    }
    return (itor != highscore_table_.end());
}
