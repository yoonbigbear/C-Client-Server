#pragma once

#include "pre.h"

#pragma warning(push)
#pragma warning(disable: 4244)
#pragma warning(disable: 4458)
#pragma warning(disable: 4127)
#include "genCpp/genClassBase.h"
#pragma warning(pop)

class DataTable : public Singleton<DataTable>
{
public:
    void Load(const String& path);

    Data::ClassBaseTable classtable;
};