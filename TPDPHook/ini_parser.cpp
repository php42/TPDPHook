#include "ini_parser.h"
#include <fstream>
#include <cctype>

IniFile IniFile::global;

void IniFile::read(const std::filesystem::path& path)
{
    std::string section, line;
    std::ifstream f(path);

    sections_.clear();

    while(std::getline(f, line))
    {
        // strip comments
        auto pos = line.find(';');
        if(pos != std::string::npos)
            line.erase(pos);

        // strip whitespace
        for(auto it = line.begin(); it != line.end();)
        {
            if(std::isblank(*it))
                it = line.erase(it);
            else
                ++it;
        }

        // normalize to lowercase
        for(auto& it : line)
            it = (char)std::tolower(it);

        if(line.empty())
            continue;

        if(line[0] == '[')
        {
            line.erase(0, line.find_first_not_of('['));
            auto end = line.find_first_of(']');
            if(end != std::string::npos)
                line.erase(end);
            section = line;
            continue;
        }

        pos = line.find('=');

        if(pos == std::string::npos)
            continue;

        auto k = line.substr(0, pos);
        auto v = line.substr(pos+1);

        sections_[section][k] = v;
    }
}
