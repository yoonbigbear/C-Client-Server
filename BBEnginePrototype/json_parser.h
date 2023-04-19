#pragma once

#include <nlohmann/json.hpp>
#include "filesystem.h"

using json = nlohmann::json;

class JsonObject
{
public:
    ~JsonObject()
    {
        std::ofstream o(filepath);
        o << data;
    }

    inline void load_or_create(const char* path)
    {
        if (FileSystem::exist(path))
        {
            std::ifstream f(path);
            try
            {
                data = json::parse(f);
            }
            catch (std::exception& e)
            {
                std::cerr << e.what();
            }
        }
        else
        {
            FileSystem::create(path);
        }
        filepath = path;
    }

    template<typename T>
    void write(const char* key, T& value)
    {
        data[key] = value;
    }

    auto& read(const char* key)
    {
        return data[key];
    }

private:
    json data;
    std::string filepath;
};