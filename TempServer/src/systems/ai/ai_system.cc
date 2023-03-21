#include "ai_system.h"

#include "components.h"

#include "world/world.h"
#include "share/navigation.h"


void Wander(Weak<World> world, float dt)
{
    auto world_ptr = world.lock();
    _ASSERT(world_ptr);

    auto navigation = world_ptr->navigation().lock();
    _ASSERT(navigation);


    auto view = world_ptr->view<WanderComponent, Transform>(entt::exclude<PathList>);
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
            List<Vec> path;
            if (!navigation->FindPath(start, dest, path))
            {
                LOG_ERROR("failed to find path to end");
                return;
            }

            auto& pathlist = world_ptr->emplace_or_replace<PathList>(entity);
            std::swap(pathlist.paths, path);

            //next dwelling time
            wandering.acc_time = 2.f;
        }
    }
}
