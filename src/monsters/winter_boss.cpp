// Header
#include "render.hpp"
#include "winter_boss.hpp"
#include "config/monster_config.hpp"

const std::string DIR = "monsters/winter/";
const std::string WALK_SPRITE = DIR + "penguin_walk.png";
const std::string ATTACK_SPRITE = DIR + "penguin_attack.png";
const int WALK_FRAMES = 4;
const int ATTACK_FRAMES = 4;

entt::entity WinterBoss::createWinterBossEntt(int round_number)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "winter_boss";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(WALK_SPRITE), "monster");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    //ECS::registry<ShadedMeshRef>.emplace(entity, resource);
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_MONSTERS + WINTER_BOSS;
    // Initialize the position, scale, and physics components
    //auto& motion = ECS::registry<Motion>.emplace(entity);
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = grid_to_pixel_velocity(monster_velocities.at(WINTER_BOSS));
    motion.position = coord_to_pixel(FOREST_COORD);
    motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 1, WALK_FRAMES);
    motion.standing = true;
    // scale down bounding box from .png file based on number of frames
    //motion.boundingbox = vec2({ motion.scale.x * (1 / WALK_FRAMES), motion.scale.y });
    motion.boundingbox = vec2({ motion.scale.x * 0.85f / WALK_FRAMES, motion.scale.y });
    auto& monster = registry.emplace<Monster>(entity);

    monster.max_health = monster_health.at(WINTER_BOSS) + sum_to_n(round_number) * BOSS_SCALE_HEALTH_FACTOR;
    monster.health = monster.max_health;
    monster.village_damage = monster_village_damage.at(WINTER_BOSS) + floor(round_number / 2) * BOSS_DAMAGE_SCALE_FACTOR;
    monster.damage = monster_damage.at(WINTER_BOSS);
    monster.reward = monster_reward.at(WINTER_BOSS);

    monster.hit = false;
    monster.type = WINTER_BOSS;
    monster.speed_multiplier = 1.f;
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
    animate.update_interval = 2;
    
    registry.emplace<WinterBoss>(entity);
    registry.emplace<HitReaction>(entity);

    return entity;
}
