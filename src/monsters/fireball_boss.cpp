// Header
#include "fireball_boss.hpp"
#include "render.hpp"
#include "config/monster_config.hpp"

const std::string WALK_SPRITE = "monsters/final/fireball.png";
const std::string ATTACK_SPRITE = "NA";
const int WALK_FRAMES = 3;
const int ATTACK_FRAMES = 0;

entt::entity FireballBoss::createFireballBossEntt()
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "fireball";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(WALK_SPRITE), "monster");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_MONSTERS + FIREBALL_BOSS;

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 200.f;
    motion.velocity = monster_velocities.at(FIREBALL_BOSS);
    motion.position = coord_to_pixel(vec2(6, DRAGON_COORD.y + 1.5));
    motion.scale = 5.0f*scale_to_grid_units(vec2(-static_cast<vec2>(resource.texture.size).x, static_cast<vec2>(resource.texture.size).y), .75f);
    motion.boundingbox = { 75, 75 };

    auto& monster = registry.emplace<Monster>(entity);
    monster.max_health = monster_health.at(FIREBALL_BOSS);
    monster.health = monster.max_health;
    monster.damage = monster_damage.at(FIREBALL_BOSS);
    monster.reward = monster_reward.at(FIREBALL_BOSS);

    monster.hit = false;
    monster.type = FIREBALL_BOSS;
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

    registry.emplace<FireballBoss>(entity);
    registry.emplace<HitReaction>(entity);

    return entity;
}
