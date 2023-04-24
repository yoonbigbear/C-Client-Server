#include "ai_system.h"
#include "../move_system.h"
#include "../npc_system.h"

#include "components.h"

#include "world/region.h"
#include "navigation.h"


void Wander(Shared<Region> world, float dt)
{
    _ASSERT(world);
    auto navigation = world->navigation();
    _ASSERT(navigation);


    auto view = world->view<WanderComponent, Transform>(entt::exclude<Mover, PathList>);
    for (auto [entity, wandering, tf] : view.each())
    {
        //dwelling for remaining time
        if ((wandering.acc_time -= dt) > 0.f)
        {
            continue;
        }
        else
        {
            //find random destination
            float dest[3];
            if (!navigation->FindRandomPointInCircle(tf.v.v3, wandering.range, dest))
            {
                LOG_ERROR("failed to find random point in around");
                return;
            }

            //calculate straight path
            float start[3] = { tf.v.v3.x, tf.v.v3.z, tf.v.v3.y };
            Deque<Vec> path;
            if (!navigation->FindPath(start, dest, path))
            {
                LOG_ERROR("failed to find path to end");
                return;
            }

            auto& pathlist = world->emplace_or_replace<PathList>(entity);
            std::swap(pathlist.paths, path);
            pathlist.flag = Start;
            //next dwelling time
            wandering.acc_time = 2.f;
        }
    }
}
