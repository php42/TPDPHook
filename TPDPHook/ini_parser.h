#pragma once

#include <filesystem>
#include <unordered_map>
#include <string>

class IniFile
{
private:
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> sections_;

public:
    IniFile() = default;
    IniFile(const std::filesystem::path& path) { read(path); }

    void read(const std::filesystem::path& path);

    auto operator[](const std::string& k) { return sections_[k]; }

    static IniFile global;
};