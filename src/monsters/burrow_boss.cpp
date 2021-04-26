// Header
#include "render.hpp"
#include "burrow_boss.hpp"
#include "config/monster_config.hpp"

const std::string WALK_SPRITE = "burrow/burrow_walk.png";
const std::string ATTACK_SPRITE = "burrow/burrow_eat.png";
const int WALK_FRAMES = 13;
const int ATTACK_FRAMES = 4;

entt::entity BurrowBoss::createBurrowBossEntt()
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "burrow_boss";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(WALK_SPRITE), "monster");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    //ECS::registry<ShadedMeshRef>.emplace(entity, resource);
    registry.emplace<ShadedMeshRef>(entity, resource);
    // Initialize the position, scale, and physics components
    //auto& motion = ECS::registry<Motion>.emplace(entity);
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = grid_to_pixel_velocity(monster_velocities.at(BURROW_BOSS));
    motion.position = coord_to_pixel(FOREST_COORD);
    motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 1, WALK_FRAMES);
    // scale down bounding box from .png file based on number of frames
    motion.boundingbox = vec2({ motion.scale.x * (1.0f / WALK_FRAMES), motion.scale.y });

    auto& monster = registry.emplace<Monster>(entity);
    monster.max_health = monster_health.at(BURROW_BOSS);
    monster.health = monster.max_health;
    monster.damage = monster_damage.at(BURROW_BOSS);
    monster.reward = monster_reward.at(BURROW_BOSS);

    monster.type = BURROW_BOSS;
    monster.hit = false;
    monster.speed_multiplier = 6.f;
    monster.sprite = WALK_SPRITE;
    monster.frames = WALK_FRAMES;
    monster.attack_frames = ATTACK_FRAMES;
    monster.attack_sprite = ATTACK_SPRITE;
    monster.walk_frames = WALK_FRAMES;
    monster.walk_sprite = WALK_SPRITE;

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0;
    animate.state = 0;
    animate.frame_num = monster.frames;
    animate.state_num = 1;

    registry.emplace<BurrowBoss>(entity);
    registry.emplace<HitReaction>(entity);

    return entity;
}
