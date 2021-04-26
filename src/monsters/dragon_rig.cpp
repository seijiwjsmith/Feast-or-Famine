#include "dragon_rig.hpp"


const std::string DRAGON_HEAD = "monsters/dragon_rig/head.png";
const std::string DRAGON_MOUTH = "monsters/dragon_rig/jaw.png";
const std::string DRAGON_BODY = "monsters/dragon_rig/body.png";
const std::string DRAGON_NECK = "monsters/dragon_rig/neck.png";
const std::string DRAGON_OUTERPAW= "monsters/dragon_rig/outerpaw.png";
const std::string DRAGON_INNERPAW = "monsters/dragon_rig/innerpaw.png";
const std::string DRAGON_OUTERWING = "monsters/dragon_rig/outerwing.png";
const std::string DRAGON_INNERWING = "monsters/dragon_rig/innerwing.png";

static void add_idle(entt::entity, Rig rig, FK_Animations& fk_animations);
static void add_attack(entt::entity, Rig rig, FK_Animations& fk_animations);
static void add_intro(entt::entity, Rig rig, FK_Animations& fk_animations);

//does not have a mesh, but a set of entities
entt::entity  DragonRig::createDragon(int round_number) {

    auto entity = registry.create();

    /*
        dependencies
    */
    //has a current_time var used to animate fk/ik systems
    auto& timeline = registry.emplace<Timeline>(entity);
    auto& fk_animations = registry.emplace<FK_Animations>(entity);
    registry.emplace<DragonRig>(entity);
    registry.emplace<HitReaction>(entity);
    registry.emplace<Attack_bool>(entity);
    registry.emplace<Animation_timing>(entity);

    /*
        adds motion
        adds monster
    */
    // root entity acts like any other entity.
    auto& motion = registry.emplace<Motion>(entity);
    motion.angle = 0.f;
    motion.velocity = normalize(vec2(45, 535) - coord_to_pixel(DRAGON_COORD)) * 2.f;
    motion.scale = vec2(150, 150);
    motion.position = coord_to_pixel(DRAGON_COORD);
    motion.boundingbox = motion.scale * 5.0f;

    auto& monster = registry.emplace<Monster>(entity);
    monster.max_health = 50000;
    monster.health = monster.max_health;
    monster.damage = 0;
    monster.reward = 10000;



    /*
        Create rig
    */
    auto& transform = registry.emplace<Transform>(entity);
    transform.mat = glm::mat3(1.0);
    //create entities/parts to be part of the kinematic chains -- requires setting position offset, pivot/origin of rotation, and intial angle
    auto body = Rig::createPart(entity, "face_box", vec2(), vec2(),0);

    auto neck = Rig::createPart(entity, "arm_simple", vec2(0.6f,0.05), vec2(0, -0.5), 3.14/2.0f);
    auto head = Rig::createPart(entity, "arm_simple", vec2(-0.1, -0.1), vec2(0, 0.5), 3.14f);

    auto outer_wing = Rig::createPart(entity, "arm_simple", vec2(0.3,-0.1), vec2(0, -0.5), 3.14f); 
    auto inner_wing = Rig::createPart(entity, "arm_simple", vec2(0.4, 0.0), vec2(0, -0.5), 3.14f);

    auto outer_arm = Rig::createPart(entity, "arm_simple", vec2(0,0.5), vec2(0, -0.5f), 0);
    auto inner_arm = Rig::createPart(entity, "arm_simple", vec2(0.2, 0.2), vec2(0, -0.5f), 0);

    auto mouth = Rig::createPart(entity, "arm_simple", vec2(0,-0.1), vec2(0, 0.5f), 3.14 / 2.0f);



    //create a component <Rig> to then point to these entities for later
    auto& rig = registry.emplace<Rig>(entity);
    rig.chains.push_back(Chain(entity, { body }));
    rig.chains.push_back(Chain(body, { neck, head }));
    rig.chains.push_back(Chain(body, { outer_wing }));
    rig.chains.push_back(Chain(body, { outer_arm }));
    rig.chains.push_back(Chain(head, { mouth }));
    rig.chains.push_back(Chain(body, { inner_wing }));
    rig.chains.push_back(Chain(body, { inner_arm }));

    int dragon_layer = LAYER_MONSTERS + 5;

    auto head_texture = Rig::createPartTextured(head, DRAGON_HEAD, vec2(-0.2f, 0.1f), -3.14 / 2.0f, 2.0f * vec2(1, 1), dragon_layer + 1);
    auto mouth_texture = Rig::createPartTextured(mouth, DRAGON_MOUTH, vec2(0, 0.2f), -3.14 / 2.0f, 1.5f * vec2(1, 1), dragon_layer - 1 );

    auto neck_texture = Rig::createPartTextured(neck, DRAGON_NECK, vec2(0.05, 0), 3.14/2.0f, 1.5f * vec2(1, 1), dragon_layer - 2);

    auto body_texture = Rig::createPartTextured(body, DRAGON_BODY, vec2(0, 0), 0.0f, 2.0f * vec2(1, 1), dragon_layer - 3);

    
    auto outer_wing_texture = Rig::createPartTextured(outer_wing, DRAGON_OUTERWING, vec2(0, 0.4f), 3.10f, 2.0f * vec2(1, 1), dragon_layer - 2);
    auto inner_wing_texture = Rig::createPartTextured(inner_wing, DRAGON_INNERWING, vec2(0, 0.4f), 3.10f, 2.0f * vec2(1, 1), dragon_layer - 4);

    auto outer_arm_texture = Rig::createPartTextured(outer_arm, DRAGON_OUTERPAW, vec2(0, 0), 1.0f, vec2(1, 1), dragon_layer - 2);
    auto inner_arm_texture = Rig::createPartTextured(inner_arm, DRAGON_INNERPAW, vec2(0, 0), 1.0f, vec2(1, 1), dragon_layer - 4);
   
    rig.textures.push_back(head_texture);
    rig.textures.push_back(mouth_texture);
    rig.textures.push_back(neck_texture);
    rig.textures.push_back(body_texture);
    rig.textures.push_back(outer_wing_texture);
    rig.textures.push_back(outer_arm_texture);
    rig.textures.push_back(inner_wing_texture);
    rig.textures.push_back(inner_arm_texture);

    /*
       add animations 
       initialize pose
    */

    add_intro(entity, rig, fk_animations);
    add_idle(entity, rig, fk_animations);
    add_attack(entity, rig, fk_animations);



    RigSystem::animate_rig_fk(entity, 1);
    RigSystem::update_rig(entity);


    /*
        create and attach rope
    */

    auto& rope_attachement = registry.emplace<Rope_attachement>(entity);
    rope_attachement.rope_rig = RopeRig::createRope(entity, 10, vec2(-0.9, 0.5f));

    return entity;
}

void add_idle(entt::entity entity, Rig rig, FK_Animations& fk_animations) {

    //neck
    Joint_angles j0;
    j0.data.emplace(0.0f, -2.3);
    j0.data.emplace(1.0f, -2.3f);
    j0.data.emplace(2.0f, -2.3f);
    j0.data.emplace(5.0f, -2.3f);

    //head
    Joint_angles j1;
    j1.data.emplace(0.0f, 3.9f);
    j1.data.emplace(1.0f, 3.8f);
    j1.data.emplace(2.0f, 3.5f);
    j1.data.emplace(4.0f, 3.8f);

    //wing
    Joint_angles j2;
    j2.data.emplace(0.0f, -3.6f);
    j2.data.emplace(1.0f, -3.1f);
    j2.data.emplace(2.0f, -3.3f);
    j2.data.emplace(4.0f, -3.5f);

    //arm
    Joint_angles j3;
    j3.data.emplace(0.0f, -0.3f);
    j3.data.emplace(1.0f, -0.3f);
    j3.data.emplace(2.0f, -0.3f);
    j3.data.emplace(4.0f, -0.5f);

    //mouth -- 0 = close, 1 == open
    Joint_angles j4;
    j4.data.emplace(0.0f, 0.3f);
    j4.data.emplace(1.0f, 0.3f);
    j4.data.emplace(2.0f, 0.3f);
    j4.data.emplace(4.0f, 0.5f);

    Keyframes_FK kfs;
    kfs.anim.emplace(rig.chains[1].chain_vector[0], j0);
    kfs.anim.emplace(rig.chains[1].chain_vector[1], j1);
    kfs.anim.emplace(rig.chains[2].chain_vector[0], j2);
    kfs.anim.emplace(rig.chains[3].chain_vector[0], j3);
    kfs.anim.emplace(rig.chains[4].chain_vector[0], j4);


    fk_animations.anims.push_back(kfs);
}


//attack animation should start from idle pose
void add_attack(entt::entity entity, Rig rig, FK_Animations& fk_animations) {
    
    auto& timing = registry.get<Animation_timing>(entity);
    float loop_start = timing.loop_start;
    float t_fireball = timing.t_fireball;
    float loop_end = timing.loop_end;

    //body
    Joint_angles j_body;
    j_body.data.emplace(loop_start, 0);
    j_body.data.emplace(t_fireball, 0.1f);
    //j_body.data.emplace(t_fireball + 0.1f, 0);
    j_body.data.emplace(t_fireball + 0.2f, 0);
    j_body.data.emplace(loop_end, 0);


    //neck 
    // -2.3 = up
    //-1.5 = fireball; down
    Joint_angles j0;
    j0.data.emplace(loop_start, -2.3f);
    j0.data.emplace(t_fireball, -1.5f);
    j0.data.emplace(t_fireball+0.1f, -1.6f);
   // j0.data.emplace(t_fireball + 0.2f, -1.65f);
    j0.data.emplace(loop_end, -2.3f);


    //head
    // 3.9 = normal
    // 2.9 = fireball; head tilt when neck is down
    Joint_angles j1;
    j1.data.emplace(loop_start, 3.9f);
    j1.data.emplace(t_fireball, 2.9f);
    j1.data.emplace(loop_end, 3.9f);


    //wing
    Joint_angles j2;
    j2.data.emplace(loop_start, -3.6f);
    j2.data.emplace(t_fireball + 0.1f, -3.3f);
    j2.data.emplace(t_fireball + 0.2f, -3.35f);
    j2.data.emplace(loop_end, -3.6f);

    //inner wing
    Joint_angles j5;
    j5.data.emplace(loop_start, -3.6f);
    j5.data.emplace(t_fireball+0.1f, -3.4f);
    j5.data.emplace(t_fireball + 0.2f, -3.4f);
    j5.data.emplace(loop_end, -3.6f);



    //mouth -- 0 = close, 0.4f == open
    Joint_angles j4;
    j4.data.emplace(loop_start, 0.0f);
    j4.data.emplace(t_fireball - 0.1f, 0.0f);
    j4.data.emplace(t_fireball, 0.4f);
    j4.data.emplace(t_fireball+0.1f, 0.4f);
    j4.data.emplace(loop_end, 0.0f);




    // inner arm
    Joint_angles j6;
    j6.data.emplace(loop_start, -1.0f);
    j6.data.emplace(t_fireball+0.1, -1.1f);
    j6.data.emplace(loop_end, -1.0f);
    //arm
    Joint_angles j3;
    j3.data.emplace(loop_start, -0.3f);
    j3.data.emplace(t_fireball+0.1, -0.5f);
    j3.data.emplace(loop_end, -0.3f);


    //add everything to keyframe_FK structure
    Keyframes_FK kfs;
    kfs.anim.emplace(rig.chains[0].chain_vector[0], j_body);
    kfs.anim.emplace(rig.chains[1].chain_vector[0], j0);
    kfs.anim.emplace(rig.chains[1].chain_vector[1], j1);
    kfs.anim.emplace(rig.chains[2].chain_vector[0], j2);
    kfs.anim.emplace(rig.chains[3].chain_vector[0], j3);
    kfs.anim.emplace(rig.chains[4].chain_vector[0], j4);
    kfs.anim.emplace(rig.chains[5].chain_vector[0], j5);
    kfs.anim.emplace(rig.chains[6].chain_vector[0], j6);

    fk_animations.anims.push_back(kfs);
}

void add_intro(entt::entity entity, Rig rig, FK_Animations& fk_animations) {

    //auto& timing = registry.get<Animation_timing>(entity);
    float loop_start = 0.0f;
  
    float loop_end = 2.0f;


    //neck 
    // -2.3 = up
    //-1.5 = fireball; down
    Joint_angles j0;
    j0.data.emplace(loop_start, -1.3f);
    j0.data.emplace(loop_start+1.0f, -1.3f);
    j0.data.emplace(loop_end, -2.3f);


    //head
    // 3.9 = normal
    // 2.9 = fireball; head tilt when neck is down
    Joint_angles j1;
    j1.data.emplace(loop_start, 2.9f);
    j1.data.emplace(loop_start+1.0f, 2.9f);
    j1.data.emplace(loop_end, 3.9f);


    //wing
    Joint_angles j2;
    j2.data.emplace(loop_start, -3.8f);
    j2.data.emplace(loop_start+1.0f, -3.8f);
    j2.data.emplace(loop_end, -3.6f);

    //inner wing
    Joint_angles j5;
    j5.data.emplace(loop_start, -3.8f);
    j5.data.emplace(loop_start+1.0f, -3.8f);
    j5.data.emplace(loop_end, -3.6f);


    //arm
    Joint_angles j3;
    j3.data.emplace(loop_start, -0.3f);
    j3.data.emplace(loop_end, -0.3f);


    //mouth -- 0 = close, 0.4f == open
    Joint_angles j4;
    j4.data.emplace(loop_start, 0.0f);
    j4.data.emplace(loop_end, 0.0f);




    // inner arm
    Joint_angles j6;
    j6.data.emplace(loop_start, -1.0f);
    j6.data.emplace(loop_end, -1.0f);


    //add everything to keyframe_FK structure
    Keyframes_FK kfs;

    kfs.anim.emplace(rig.chains[1].chain_vector[0], j0);
    kfs.anim.emplace(rig.chains[1].chain_vector[1], j1);
    kfs.anim.emplace(rig.chains[2].chain_vector[0], j2);
    kfs.anim.emplace(rig.chains[3].chain_vector[0], j3);
    kfs.anim.emplace(rig.chains[4].chain_vector[0], j4);
    kfs.anim.emplace(rig.chains[5].chain_vector[0], j5);
    kfs.anim.emplace(rig.chains[6].chain_vector[0], j6);

    fk_animations.anims.push_back(kfs);
}