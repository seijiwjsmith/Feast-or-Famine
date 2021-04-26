#pragma once

#include "common.hpp"
#include "entt.hpp"


inline std::string projectiles_texture_path(const std::string& name) { return data_path() + "/textures/projectiles/" + name; };

// Projectile from
struct Projectile
{
// Creates all the associated render resources and default transform
    static entt::entity createProjectile(entt::entity e_unit, entt::entity e_monster, int damage);
    int damage;
};

struct Snowball
{
    static entt::entity createSnowball(entt::entity e_unit, entt::entity e_monster, int damage);
    std::vector<vec2> bezier_points = std::vector<vec2>();
    int current_step = 0;
};

struct Flamethrower
{
    static entt::entity createFlamethrower(entt::entity e_unit, entt::entity e_monster, int damage);
    float active_timer = 1000;
    entt::entity e_unit; 
};

struct LaserBeam
{
    static entt::entity createLaserBeam(entt::entity e_unit, entt::entity e_monster, int damage);
    float active_timer = 1000;
    entt::entity e_unit;
    vec2 unit_pos;
};

struct Missile
{
    static entt::entity createMissile(entt::entity e_unit, entt::entity e_monster, int damage);
};

struct Explosion
{
    static entt::entity createExplosion(entt::entity e_projectile, int damage);
    float active_timer = 1000;
    entt::entity e_unit;
};

struct IceField
{
    static entt::entity createIceField(entt::entity e_unit, entt::entity e_monster, int damage);
    float active_timer = 1000;
    entt::entity e_unit;
};