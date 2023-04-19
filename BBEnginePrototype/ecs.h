#pragma once

#pragma warning( push )
#pragma warning(disable : 4819 )
#include <entt-3.11.1/entt/entt.hpp>
#pragma warning( pop )

using Entity = entt::entity;

namespace Entt
{
    class ECS : public entt::registry
    {
    public:
        template<typename ...Comps>
        inline  _NODISCARD bool Valid(const uint32_t entity) const
        {
            return Valid<Comps...>(entt::entity(entity));
        }
        template<typename ...Comps>
        inline _NODISCARD bool Valid(const entt::entity entity) const
        {
            if constexpr (std::_Is_any_of_v<Comps..., void>)
            {
                return valid(entity);
            }
            else
            {
                return (valid(entity) && all_of<Comps...>(entity));
            }
        }
    };
}