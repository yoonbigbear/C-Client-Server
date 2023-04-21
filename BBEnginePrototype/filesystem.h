#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>

namespace FileSystem
{
    inline void create(const char* path)
    {
        std::ofstream fs(path);
    }
    inline auto create_directories(const char* path)
    {
        std::filesystem::create_directories(path);
    }
    inline bool exist(const char* path)
    {
        return std::filesystem::exists(path);
    }

    inline bool is_file(const char* path)
    {
        return std::filesystem::is_regular_file(path);
    }
    inline auto to_path(const char* path)
    {
        return std::filesystem::path(path);
    }
    inline auto current_path()
    {
        return std::filesystem::current_path();
    }
    inline auto relative_path(const std::filesystem::path& path)
    {
        return path.relative_path();
    }
    inline auto relative_path(const char* path)
    {
        return relative_path(std::filesystem::path(path));
    }
    inline auto absolute_path(const std::filesystem::path& path)
    {
        return std::filesystem::canonical(path);
    }
    inline auto absolute_path(const char* path)
    {
        return absolute_path(to_path(path));
    }

    inline auto open(const char* path)
    {
        std::ofstream f(path);
        return f;
    }

    inline auto open_all(const char* dir)
    {
        return std::filesystem::directory_iterator(dir);
    }
};