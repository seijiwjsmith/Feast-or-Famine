// Header

#include "projectile.hpp"
#include "render.hpp"
#include "common.hpp"
#include <iostream>
#include <ai.hpp>

float EXPLOSION_FRAMES = 4.f;
float FLAMETHROWER_FRAMES = 3.f;
float ICEFIELD_FRAMES = 5.f;

entt::entity Projectile::createProjectile(entt::entity e_unit, entt::entity e_monster, int damage)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "projectile";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, projectiles_texture_path("projectile.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_PROJECTILES;

    auto monster_motion = registry.get<Motion>(e_monster);
    auto hunter_motion = registry.get<Motion>(e_unit);

    vec2 direction = normalize(monster_motion.position - hunter_motion.position);

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = atan2(direction.y, direction.x);
    motion.velocity = grid_to_pixel_velocity(direction * 20.f);
    motion.position = hunter_motion.position;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = scale_to_grid_units(vec2(-static_cast<vec2>(resource.texture.size).x, static_cast<vec2>(resource.texture.size).y), 0.3);
    motion.boundingbox = motion.scale;

    // Create and (empty) Projectile component to be able to refer to all Projectile
    Projectile& p = registry.emplace<Projectile>(entity);
    p.damage = damage;

    auto& sound = registry.emplace<SoundRef>(entity);
    //sound.sound_reference = Mix_LoadWAV(audio_path("projectile/impact.wav").c_str());
    sound.file_path = "projectile/impact.wav";
    sound.play_sound = false;
    sound.on_impact_destory = true;

    return entity;
}

entt::entity Snowball::createSnowball(entt::entity e_unit, entt::entity e_monster, int damage)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "rock_projectile";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, projectiles_texture_path("snow.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_PROJECTILES;

    auto monster_motion = registry.get<Motion>(e_monster);
    auto hunter_motion = registry.get<Motion>(e_unit);

    float dist = length(monster_motion.position - hunter_motion.position);
    vec2 monster_pos = AISystem::calculate_position(e_monster, dist);

    vec2 distance = (monster_pos - hunter_motion.position);

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = hunter_motion.position;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.velocity = grid_to_pixel_velocity(normalize(distance) * 20.f);
    motion.scale = scale_to_grid_units(vec2(-static_cast<vec2>(resource.texture.size).x, static_cast<vec2>(resource.texture.size).y), 0.3);
    motion.boundingbox = motion.scale;

    // Create and (empty) Projectile component to be able to refer to all Projectile
    Projectile& p = registry.emplace<Projectile>(entity);
    p.damage = damage;
    
    Snowball& snowball = registry.emplace<Snowball>(entity);

    auto& sound = registry.emplace<SoundRef>(entity);
    //sound.sound_reference = Mix_LoadWAV(audio_path("projectile/snowball_hit.wav").c_str());
    sound.file_path = "projectile/snowball_hit.wav";
    sound.play_sound = false;

    return entity;
}

entt::entity Flamethrower::createFlamethrower(entt::entity e_unit, entt::entity e_monster, int damage)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "flamethrower";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, projectiles_texture_path("flamethrower.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_PROJECTILES;

    auto& motion_h = registry.get<Motion>(e_unit);
    auto& motion_m = registry.get<Motion>(e_monster);
    vec2 direction = normalize(motion_m.position - motion_h.position) * 100.f;

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = motion_h.position + direction;
    motion.angle = atan2(-direction.y, -direction.x);
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = { static_cast<vec2>(resource.texture.size).x, static_cast<vec2>(resource.texture.size).y };
    motion.boundingbox = vec2({ motion.scale.x * 0.22f , motion.scale.y });

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0.f;
    animate.state = 0.f;
    animate.frame_num = FLAMETHROWER_FRAMES;
    animate.state_num = 1.f;

    // Create and (empty) Projectile component to be able to refer to all Projectile
    Projectile& p = registry.emplace<Projectile>(entity);
    p.damage = damage;

    auto& flame = registry.emplace<Flamethrower>(entity);
    flame.e_unit = e_unit;

    auto& sound = registry.emplace<SoundRef>(entity);
    //sound.sound_reference = Mix_LoadWAV(audio_path("projectile/flamethrower.wav").c_str());
    sound.file_path = "projectile/flamethrower.wav";
    sound.play_delay_ms = 3300;

    return entity;
}


entt::entity LaserBeam::createLaserBeam(entt::entity e_unit, entt::entity e_monster, int damage)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "laserbeam";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0) {

        resource = ShadedMesh();
        RenderSystem::createSprite(resource, projectiles_texture_path("laserbeam.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_PROJECTILES;

    auto& hunter_position = registry.get<Motion>(e_unit).position;
    auto& monster_position = registry.get<Motion>(e_monster).position;

    vec2 direction = normalize(monster_position - hunter_position);

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = hunter_position + direction * static_cast<vec2>(resource.texture.size).x / 2.f;
    motion.angle = atan2(direction.y, direction.x);
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2(-static_cast<vec2>(resource.texture.size).x, static_cast<vec2>(resource.texture.size).y);
    motion.boundingbox = motion.scale;

    // Create and (empty) Projectile component to be able to refer to all Projectile
    Projectile& p = registry.emplace<Projectile>(entity);
    p.damage = damage;

    auto& beam = registry.emplace<LaserBeam>(entity);
    beam.e_unit = e_monster;
    beam.unit_pos = hunter_position;
    
    auto& sound = registry.emplace<SoundRef>(entity);
    //sound.sound_reference = Mix_LoadWAV(audio_path("projectile/laser.wav").c_str());
    sound.file_path = "projectile/laser.wav";
    sound.play_delay_ms = 1000;
    return entity;
}


entt::entity Missile::createMissile(entt::entity e_unit, entt::entity e_monster, int damage)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "missile";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, projectiles_texture_path("missile.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_PROJECTILES;

    auto monster_motion = registry.get<Motion>(e_monster);
    auto hunter_motion = registry.get<Motion>(e_unit);

    vec2 direction = normalize(monster_motion.position - hunter_motion.position);

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = atan2(direction.y, direction.x);
    motion.velocity = grid_to_pixel_velocity(direction * 20.f);
    motion.position = hunter_motion.position;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = scale_to_grid_units(vec2(-static_cast<vec2>(resource.texture.size).x, static_cast<vec2>(resource.texture.size).y), 0.3);
    motion.boundingbox = motion.scale;

    // Create and (empty) Projectile component to be able to refer to all Projectile
    Projectile& p = registry.emplace<Projectile>(entity);
    p.damage = damage;

    play_sound("projectile/rocket_launcher_begin.wav");

    registry.emplace<Missile>(entity);

    return entity;
}

entt::entity Explosion::createExplosion(entt::entity e_projectile, int damage)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "Explosion";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, projectiles_texture_path("explosion_animate.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_PROJECTILES;

    auto hunter_motion = registry.get<Motion>(e_projectile);

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.position = hunter_motion.position;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = scale_to_grid_units(vec2(-static_cast<vec2>(resource.texture.size).x, static_cast<vec2>(resource.texture.size).y), 4);
    motion.boundingbox = vec2({ motion.scale.x * 0.22f , motion.scale.y });

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0.f;
    animate.state = 0.f;
    animate.frame_num = EXPLOSION_FRAMES;
    animate.state_num = 1.f;

    // Create and (empty) Projectile component to be able to refer to all Projectile
    Projectile& p = registry.emplace<Projectile>(entity);
    p.damage = damage;

    auto& explosion = registry.emplace<Explosion>(entity);
    explosion.e_unit = e_projectile;

    play_sound("projectile/explosion.wav");

    return entity;
}

entt::entity IceField::createIceField(entt::entity e_unit, entt::entity e_monster, int damage)
{
    // Reserve an entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "ice_field";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, projectiles_texture_path("ice_field.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_MONSTERS - 1;

    auto monster_motion = registry.get<Motion>(e_monster);
    auto hunter_motion = registry.get<Motion>(e_unit);

    vec2 direction = normalize(monster_motion.position - hunter_motion.position);

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = atan2(direction.y, direction.x);
    motion.position = hunter_motion.position;
    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2(static_cast<vec2>(resource.texture.size).x * 1.5, static_cast<vec2>(resource.texture.size).y * 1.5);
    motion.boundingbox = vec2({ motion.scale.x * 0.18f , motion.scale.y });

    // Create and (empty) Projectile component to be able to refer to all Projectile
    Projectile& p = registry.emplace<Projectile>(entity);
    p.damage = damage;

    /*Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0.f;
    animate.state = 0.f;
    animate.frame_num = ICEFIELD_FRAMES;
    animate.state_num = 1.f;*/

    play_sound("projectile/ice_field.wav");

    auto& field = registry.emplace<IceField>(entity);
    field.e_unit = e_unit;

    return entity;
}