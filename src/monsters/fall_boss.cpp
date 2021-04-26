// Header
#include "render.hpp"
#include "fall_boss.hpp"
#include "config/monster_config.hpp"

const std::string DIR = "monsters/fall/";
const std::string WALK_SPRITE = DIR+"bear_walk.png";
const std::string ATTACK_SPRITE = DIR+"bear_attack.png";
const int WALK_FRAMES = 4;
const int ATTACK_FRAMES = 4;

entt::entity FallBoss::createFallBossEntt(int round_number)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "fall_boss";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(WALK_SPRITE), "monster");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_MONSTERS + FALL_BOSS;
    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = grid_to_pixel_velocity(monster_velocities.at(FALL_BOSS));
    motion.position = coord_to_pixel(FOREST_COORD);
    motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 1, WALK_FRAMES);
    // scale down bounding box from .png file based on number of frames
    motion.boundingbox = vec2({ motion.scale.x *0.85f / WALK_FRAMES, motion.scale.y });
    motion.standing = true;
    auto& monster = registry.emplace<Monster>(entity);
    monster.max_health = monster_health.at(FALL_BOSS) + sum_to_n(round_number) * BOSS_SCALE_HEALTH_FACTOR;
    monster.health = monster.max_health;
    monster.village_damage = monster_village_damage.at(FALL_BOSS) + floor(round_number / 2) * BOSS_DAMAGE_SCALE_FACTOR;
    monster.damage = monster_damage.at(FALL_BOSS);
    monster.reward = monster_reward.at(FALL_BOSS);

    monster.type = FALL_BOSS;
    monster.hit = false;
    monster.speed_multiplier = 1.f;
    monster.sprite = WALK_SPRITE;
    monster.frames = WALK_FRAMES;
    monster.attack_frames = ATTACK_FRAMES;
    monster.attack_sprite = ATTACK_SPRITE;
    monster.walk_frames = WALK_FRAMES;
    monster.walk_sprite = WALK_SPRITE;
    monster.slow_walk = true;

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0;
    animate.state = 0;
    animate.frame_num = WALK_FRAMES;
    animate.state_num = 1;
    animate.update_interval = 2;

    registry.emplace<FallBoss>(entity);
    registry.emplace<HitReaction>(entity);

    return entity;
}
