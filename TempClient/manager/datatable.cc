#include "datatable.h"


void DataTable::Load(const String& path)
{
    classtable.Load(path + "ClassBase.csv");
}
