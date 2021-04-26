// Header
#include "mob.hpp"
#include "render.hpp"
#include "config/monster_config.hpp"

const size_t WALK_FRAMES = 4.f;
const size_t ATTACK_FRAMES = 4.f;
const std::string DIR = "monsters/mob/";
const std::string WALK_SPRITE = DIR + "rabbit_walk.png";
const std::string ATTACK_SPRITE = DIR + "rabbit_attack.png";

entt::entity Mob::createMobEntt(int round_number)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "mob";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(WALK_SPRITE), "monster");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_MONSTERS + MOB;

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = grid_to_pixel_velocity(monster_velocities.at(MOB));
    motion.position = coord_to_pixel(FOREST_COORD);
    motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 0.8, WALK_FRAMES);
    // temporary fix
    //motion.boundingbox = vec2({ motion.scale.x , motion.scale.y });
    motion.boundingbox = vec2({ motion.scale.x*0.13f , motion.scale.y });
    auto& monster = registry.emplace<Monster>(entity);
    monster.max_health = monster_health.at(MOB) + sum_to_n(round_number) * MONSTER_SCALE_HEALTH_FACTOR;
    monster.health = monster.max_health;
    monster.village_damage = monster_village_damage.at(MOB) + floor(round_number / 2) * MONSTER_DAMAGE_SCALE_FACTOR;
    monster.damage = monster_damage.at(MOB) + round_number * MONSTER_DAMAGE_SCALE_FACTOR;
    monster.reward = monster_reward.at(MOB);
    monster.type = MOB;
    monster.sprite = WALK_SPRITE;
    monster.frames = WALK_FRAMES;
    monster.attack_frames = ATTACK_FRAMES; 
    monster.attack_sprite = ATTACK_SPRITE;
    monster.walk_frames = WALK_FRAMES;
    monster.walk_sprite = WALK_SPRITE;

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0;
    animate.state = 0;
    animate.frame_num = WALK_FRAMES;
    animate.state_num = 1;

    registry.emplace<Mob>(entity);
    registry.emplace<HitReaction>(entity);
    return entity;
}
