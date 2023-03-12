#include "systems.h"

#include "fbb/chat_generated.h"
#include "fbb/packets_generated.h"
#include "fbb/world_generated.h"

#include "manager/scene_manager.h"

void Chat_Sync(Vector<uint8_t>& data)
{
    auto sync = flatbuffers::GetRoot<ChatSync>(data.data());
    Gui::instance().log.AddLog("[info] received ChatSync");
    Gui::instance().console.AddLog(sync->chat()->c_str());
}

void Sight_Sync(Vector<uint8_t>& data)
{
    auto sync = flatbuffers::GetRoot<SightSync>(data.data());
    auto t = sync->UnPack();
    Gui::instance().log.AddLog("[info] received SightSync");

    auto scene = SceneManager::instance().Get(0).lock();
    if (scene)
    {
        for (auto& e : t->entities)
        {
            scene->Enter(e);
        }
    }
}

void Move_Sync(Vector<uint8_t>& data)
{
    auto sync = flatbuffers::GetRoot<MoveSync>(data.data());
    auto t = sync->UnPack();
    auto scene = SceneManager::instance().Get(0).lock();
    if (scene)
    {
        vec dest;
        memcpy(&dest, &t->dest, sizeof(vec));
        vec dir;
        memcpy(&dir, &t->dir, sizeof(vec));
        scene->Move(t->eid, dest, dir, t->spd);
    }
    Gui::instance().log.AddLog("[info] received MoveSync");
}
