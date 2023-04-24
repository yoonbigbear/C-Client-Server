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
        Entity ToEntity(uint32_t entity) { return static_cast<entt::entity>(entity); }

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
        decltype(auto) Destroy(entt::entity entity)
        {
            return destroy(entity);
        }
        decltype(auto) Destroy(uint32_t entity)
        {
            return Destroy(entt::entity(entity));
        }
        template<typename Comp>
        decltype(auto) Emplace_or_Replace(entt::entity entity)
        {
            return emplace_or_replace<Comp>(entity);
        }
        template<typename Comp>
        decltype(auto) Emplace_or_Replace(uint32_t entity)
        {
            return Emplace_or_Replace<Comp>(entity);
        }
        template<typename Comp>
        decltype(auto) Get(entt::entity entity)
        {
            return get<Comp>(entity);
        }
        template<typename Comp>
        decltype(auto) Get(uint32_t entity)
        {
            return Get<Comp>(entity);
        }

#ifdef _CLIENT
#include <unordered_map>
    public:
        Entity Create(uint32_t server_eid)
        {
            auto entity = create();
            mapped_eid_[server_eid] = entity;
            return entity;
        }
        Entity ServerEidToClientEid(uint32_t server_eid)
        {
            return mapped_eid_.contains(server_eid) ? mapped_eid_[server_eid] : entt::null;
        }
        std::unordered_map<uint32_t, entt::entity> mapped_eid_;
#endif

    };
}