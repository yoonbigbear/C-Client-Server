#include "datatable_mgr.h"

void DataTable::Load(std::string path)
{
    debug_ = std::make_shared<DebugTable>();
    debug_->Load(path + "Debug.csv");

    class_ = std::make_shared<ClassBaseTable>();
    class_->Load(path + "ClassBase.csv");

    object_ = std::make_shared<ObjectBaseTable>();
    object_->Load(path + "ObjectBase.csv");
}
