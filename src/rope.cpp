#include "rope.hpp"
#include "rig.hpp"
#include <iostream>
#include "camera.hpp"

/* Physics Based Animation
Implement time stepping based physical simulation which can either serve as a background effects (e.g. water, 
smoke implemented using particles) or as active game elements (throwing a ball, swinging a rope, etc.). 
A subset of the game entities (main or background) should possess non-trivial physics properties 
such as momentum (linear or angular) and acceleration, and act based on those.
*/


const std::string link_top = "monsters/dragon_rig/chain_top_small.png";
const std::string link_side = "monsters/dragon_rig/chain_side_small.png";

entt::entity RopeRig::createRope(entt::entity anchor, int length, vec2 offset) {
    auto entity = registry.create();
    auto& rope = registry.emplace<RopeRig>(entity);

    rope.anchor = anchor;
    rope.offset = offset;

    for (int i = 0; i < length; i++) {
        if (i % 2 == 0) {
            rope.chain.push_back(createRopePart(vec2(10 * i, 0), link_side)); // add links to a vector
        }
        else {
            rope.chain.push_back(createRopePart(vec2(10 * i, 0), link_top)); // add links to a vector
        }        
    }
   
    RopeSystem::update_rig(entity); //initialize rig
    return entity;
}

entt::entity RopeRig::createRopePart(vec2 pos, std::string name) {
    
    auto entity = registry.create();

    std::string key = name;
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(name), "monster");
    }

    ShadedMeshRef& mesh_ref = registry.emplace<ShadedMeshRef>(entity, resource);

    if(name == link_side)
        mesh_ref.layer = LAYER_MONSTERS + 5 + 1;
    else 
        mesh_ref.layer = LAYER_MONSTERS + 5;
 
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.0f;
    motion.velocity = { 0, 0 };
    motion.origin = { 0,0 };
    motion.scale = vec2(25,25); 
    motion.position = pos;
    motion.boundingbox = motion.scale;
    motion.acceleration = { 0,150}; //TODO: gravity value
    return entity;
}


void update_helper(RopeRig ropeRig , vec2 anchor_pos) {

    //fix first chain link position -- 'anchor it'
    auto& link_motion = registry.get<Motion>(ropeRig.chain[0]);
    link_motion.position = anchor_pos;

    
    //fix first chain part to mouse
    //auto& mouse = registry.get<MouseMovement>(camera).mouse_pos;
    //auto link = ropeRig.chain[0];
    //auto& link_motion = registry.get<Motion>(link);
    //link_motion.position = mouse;

    for (int i = 0; i < ropeRig.chain.size() - 1; i++) {
        auto link0 = ropeRig.chain[i];
        auto link1 = ropeRig.chain[(int)i + 1];

        auto& link0_motion = registry.get<Motion>(link0);
        auto& link1_motion = registry.get<Motion>(link1);

        vec2 p1p0 = link1_motion.position - link0_motion.position;
        vec2 dir = normalize(p1p0);
        float dist = length(p1p0);

        //update dist
        float dif = abs(dist - 10.0f);
        link0_motion.position = link0_motion.position + dir * dif / 2.0f;
        link1_motion.position = link1_motion.position - dir * dif / 2.0f;
        //link1_motion.position = link1_motion.position - dir * dif ;

        //update rotation
        //float angle = atan(dir.y / dir.x);
        //link1_motion.angle = angle;
   
        vec2 y_axis = vec2(0, 1);
        float angle = acos(dot(dir, y_axis));

        if (dir.x < 0) {
            link1_motion.angle = angle;
        }
        else if (dir.x > 0) {
            link1_motion.angle = -(angle) ;
        }
    }
}


vec2 get_anchor_pos(RopeRig ropeRig) {
    auto& anchor_motion = registry.get<Motion>(ropeRig.anchor);
   
    Motion root_motion = registry.get<Motion>(ropeRig.anchor);
    auto& rig = registry.get<Rig>(ropeRig.anchor);

    Transform transform;
    transform.mat = mat3(1.0f);
    transform.translate(root_motion.position);
    transform.rotate(root_motion.angle);

    auto part = rig.chains[1].chain_vector[0]; // neck

    auto& motion = registry.get<Motion>(part);
    const auto& entity_transform = registry.get<Transform>(part);

    vec3 anchor_pt = transform.mat * entity_transform.mat * vec3(ropeRig.offset.x , ropeRig.offset.y, 1.0f);


    return anchor_pt;
}


//normalizes link distances and fixes position of 'anchor'
void RopeSystem::update_rig(entt::entity rope_rig) {
    auto& ropeRig = registry.get<RopeRig>(rope_rig);
    vec2 anchor_pos = get_anchor_pos(ropeRig);
    // iterative solver.
    for (int i = 0; i < 10; i++) { 
        update_helper(ropeRig, anchor_pos);
    }
}

//should apply drag/disspative forces, gravity, and angular and linear momentum D:
void RopeSystem::update_physics(entt::entity rope_rig,float elapsed_ms) {
    float step_seconds = elapsed_ms / 1000.0f;

    auto& ropeRig = registry.get<RopeRig>(rope_rig);

    for (int i = 0; i < ropeRig.chain.size(); i++) {
        auto link = ropeRig.chain[i];
        auto& motion = registry.get<Motion>(link);
        
        vec2 velocity = (motion.position - motion.origin); // pos_current - pos_old ***this is pos_delta/step_period -> can add velocity directly to pos.
        motion.origin = motion.position; // using origin to store old position
        motion.position += velocity; //assumes uniform motion from last physics step.
        motion.position += motion.acceleration * step_seconds; //grab the gravity value and apply it as a velocity*step_period.
    }
}


void RopeRig::delete_rope(entt::entity rope_entity) {
    auto& ropeRig = registry.get<RopeRig>(rope_entity);

    for (auto link : ropeRig.chain) {
        registry.destroy(link);
    }
    registry.destroy(rope_entity);
}