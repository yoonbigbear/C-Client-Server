#pragma once
#include "pre.h"

#pragma warning(push)
#pragma warning(disable: 4244)
#pragma warning(disable: 4458)
#pragma warning(disable: 4127)
#include "genCpp/genDebug.h"
#include "genCpp/genClassBase.h"
#include "genCpp/genObjectBase.h"
#pragma warning(pop)

using namespace Data;

class DataTable : public Singleton<DataTable>
{
public:
    void Load(std::string path);

public:
    DebugTable& debug() { return *debug_; }
    ClassBaseTable& classbase() { return *class_; }
    ObjectBaseTable& objectbase() { return *object_; }

private:
    Shared<DebugTable> debug_;
    Shared<ClassBaseTable> class_;
    Shared<ObjectBaseTable> object_;
};