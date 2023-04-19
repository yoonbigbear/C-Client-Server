#pragma once

#include <csv-parser-master/single_include/csv.hpp>
#include "filesystem.h"

using namespace csv;

class CsvParser : public CSVReader
{
public:
    CsvParser(const char* path)
        :CSVReader(path) {}
    CsvParser(std::string str)
        :CSVReader(str) {}
};