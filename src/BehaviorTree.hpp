#include <entt.hpp>
#pragma once

// The return type of behaviour tree processing
enum class BTState {
    Moving,
    Stopped,
    Attack,
    Failure,
    Dragon,
    Fireball
};

// The base class representing any node in our behaviour tree
class BTNode {
public:
    virtual ~BTNode() noexcept = default; // Needed for deletion-through-pointer

    virtual void init(entt::entity e) {};

    virtual BTState process(entt::entity e) = 0;
};

static void remove_unit_entity(unit_type type, entt::entity e_unit);
static BTState handle_monster_attack(entt::entity entity, Monster& monster, GridNode& next_node);
static BTState increment_monster_step(entt::entity entity);
