// Header
#include "render.hpp"
#include "final_boss.hpp"
#include "config/monster_config.hpp"

const std::string WALK_SPRITE = "NA";
const std::string ATTACK_SPRITE = "monsters/final/final_boss2.png";
const int WALK_FRAMES = 0;
const int ATTACK_FRAMES = 39;

entt::entity FinalBoss::createFinalBossEntt()
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "final_boss";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(ATTACK_SPRITE), "monster");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_MONSTERS + FINAL_BOSS;

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = monster_velocities.at(FINAL_BOSS);
    motion.position = coord_to_pixel(DRAGON_COORD);
    motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 7, ATTACK_FRAMES);
    motion.boundingbox = vec2({ motion.scale.x *.8f / ATTACK_FRAMES, motion.scale.y });

    auto& monster = registry.emplace<Monster>(entity);
    monster.max_health = monster_health.at(FINAL_BOSS);
    monster.health = monster.max_health;
    monster.damage = monster_damage.at(FINAL_BOSS);
    monster.reward = monster_reward.at(FINAL_BOSS);

    monster.hit = false;
    monster.type = FINAL_BOSS;
    monster.speed_multiplier = 1.f;
    monster.sprite = ATTACK_SPRITE;
    monster.frames = ATTACK_FRAMES;
    monster.attack_frames = ATTACK_FRAMES;
    monster.attack_sprite = ATTACK_SPRITE;
    monster.walk_frames = WALK_FRAMES;
    monster.walk_sprite = WALK_SPRITE;

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0;
    animate.state = 0;
    animate.frame_num = ATTACK_FRAMES;
    animate.state_num = 1;

    registry.emplace<FinalBoss>(entity);
    registry.emplace<HitReaction>(entity);

    return entity;
}
