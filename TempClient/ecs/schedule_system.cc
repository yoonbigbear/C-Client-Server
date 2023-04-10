#include "schedule_system.h"

#include "level/scene.h"

void UpdateTimer(Shared<class Scene> scene, float dt)
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
