#include "spider.hpp"
#include "config/monster_config.hpp"


void add_frames_IK(KeyFrames_IK& kf_ik);

//TODO: refactor to store multiple animations. vector?

//does not have a mesh, but a set of entities
entt::entity  Spider::createSpider() {
    auto entity = registry.create();

    // root entity acts like any other entity.
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = grid_to_pixel_velocity(monster_velocities.at(SPIDER));
    motion.scale = vec2(20,20);
    motion.position = coord_to_pixel(FOREST_COORD);
    motion.boundingbox = motion.scale*2.0f;

    //create entities/parts to be part of the kinematic chains -- requires setting position offset, pivot/origin of rotation, and intial angle
    auto body = Rig::createPart(entity, "face_box");

    auto L_upper_leg = Rig::createPart(entity,"arm_simple", vec2(1.0f, -0.5f), vec2(0, 0.5f), 0); // position offset, origin, angle
    auto L_lower_leg = Rig::createPart(entity, "arm_simple", vec2(), vec2(0, -0.5f), 3.14f);

    auto R_upper_leg = Rig::createPart(entity, "arm_simple", vec2(-1.0f, -0.5f), vec2(0, 0.5f), 0);
    auto R_lower_leg = Rig::createPart(entity, "arm_simple", vec2(), vec2(0, -0.5f), 3.14f);

   // auto ext = Rig::createPart(entity, "arm_simple", vec2(), vec2(0, -0.5f),3.14f);
    

    //create a component <Rig> to then point to these entities for later
    auto& rig = registry.emplace<Rig>(entity);
    rig.chains.push_back(Chain(entity, { body }));
    rig.chains.push_back(Chain(body, { L_upper_leg, L_lower_leg })); // added by the chain, leaf node last
    rig.chains.push_back(Chain(body,  { R_upper_leg, R_lower_leg }));

    //rig.chains.push_back(Chain(R_lower_leg, { ext }));


    //has a current_time var used to animate fk/ik systems
    auto& timeline = registry.emplace<Timeline>(entity);
    auto& animations = registry.emplace<IK_Animations>(entity);

    auto& keyframes_ik = animations.anims[animations.anim_state];
    add_frames_IK(keyframes_ik);


    auto& monster = registry.emplace<Monster>(entity);
    monster.type = SPIDER;
    monster.max_health = monster_health.at(SPIDER);
    monster.health = monster.max_health;
    monster.damage = monster_damage.at(SPIDER);
    monster.reward = monster_reward.at(SPIDER);
    registry.emplace<HitReaction>(entity);

    auto& transform = registry.emplace<Transform>(entity);
    transform.mat = glm::mat3(1.0);

    return entity;
}




// walk cycle
void add_frames_IK(KeyFrames_IK& kf_ik) { //pos defined relative to root_motion

    float x = 1.4f;
    float low = 1.2f;
    float hi = 0.5f;

    std::map<float, vec2> body_data;
    kf_ik.data.push_back(body_data);

    std::map<float, vec2> L_data;
    L_data.emplace(0.0f, vec2(x, hi));
    L_data.emplace(0.01f, vec2(x, hi));
    L_data.emplace(0.1f, vec2(x, low));
    L_data.emplace(0.2f, vec2(x, hi));
    kf_ik.data.push_back(L_data);

    std::map<float, vec2> R_data;
    R_data.emplace(0.0f, vec2(-x, low));
    R_data.emplace(0.1f, vec2(-x, hi));
    R_data.emplace(0.11f, vec2(-x, hi));
    R_data.emplace(0.2f,  vec2(-x, low));
    kf_ik.data.push_back(R_data);

    //std::map<float, vec2> ext_data;
    //ext_data.emplace(0.0f, vec2(-2,0.5f));
    //ext_data.emplace(0.2f, vec2(-2, -0.4f));
    //kf_ik.data.push_back(ext_data);
}


