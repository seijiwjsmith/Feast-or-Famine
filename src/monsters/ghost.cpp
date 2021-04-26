// Header
#include "ghost.hpp"
#include "render.hpp"

const size_t WALK_FRAMES = 1.f;
const std::string DIR = "monsters/";
const std::string WALK_SPRITE = DIR + "ghost.png";

entt::entity Ghost::createGhostEntt(vec2 position)
{
    // Reserve en entity
    auto entity = registry.create();

    // Create the rendering components
    std::string key = "ghost";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(WALK_SPRITE), "monster");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef& shaded_mesh = registry.emplace<ShadedMeshRef>(entity, resource);
    shaded_mesh.layer = LAYER_MONSTERS + GHOST;

    // Initialize the position, scale, and physics components
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.position = position;
    motion.scale = scale_to_grid_units(static_cast<vec2>(resource.texture.size), 0.8, WALK_FRAMES);
    motion.velocity = grid_to_pixel_velocity(vec2(0, -1));
    // temporary fix
    //motion.boundingbox = vec2({ motion.scale.x , motion.scale.y });
    motion.boundingbox = vec2({ motion.scale.x*0.13f , motion.scale.y });

    Animate& animate = registry.emplace<Animate>(entity);
    animate.frame = 0;
    animate.state = 0;
    animate.frame_num = WALK_FRAMES;
    animate.state_num = 1;

    registry.emplace<Ghost>(entity);
    return entity;
}
