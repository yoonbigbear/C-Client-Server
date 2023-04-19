#include "schedule_system.h"

#include "level/region.h"

void UpdateTimer(Shared<class Region> scene, float dt)
{
    auto view = scene->view<TimerComponent>();
    for (auto [entity, timer] : view.each())
    {
        if (scene->valid(entity))
        {
            timer.sec -= dt;
            if (timer.sec < 0.f)
            {
                timer.action();
                scene->remove<TimerComponent>(entity);
            }
        }
        else
        {
            ADD_ERROR("Invalid Entity {}", static_cast<uint32_t>(entity));
        }
    }

}
