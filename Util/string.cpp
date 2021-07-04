#include "stdafx.h"
#include "string.h"

#include <boost/algorithm/string.hpp>

using namespace boost::algorithm;

std::vector<std::string> Util::Split(const std::string& line)
{
    std::vector<std::string> args;
    std::string::size_type begin = 0;
    std::string::size_type end = 0;
    bool done = false;
    bool quotes = false;
    while(end < line.length())
    {
        switch(line[end])
        {
        case '"':
            quotes = !quotes;
            ++end;
            break;
        case '\t':
        case ' ':
            if(!quotes)
            {
                std::string temp(line.substr(begin, end-begin));
                trim_if(temp, is_any_of(" \t"));
                args.push_back(temp);
                ++end;
                begin = end;
            }
            else
            {
                ++end;
            }
            break;
        default:
            ++end;
            break;
        }
    }

    std::string temp(line.substr(begin, end-begin));
    trim_if(temp, is_any_of(" \t"));
    args.push_back(temp);

    return args;
}
