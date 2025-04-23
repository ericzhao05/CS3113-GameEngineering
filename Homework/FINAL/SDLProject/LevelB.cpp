#include "LevelB.h"
#include "Utility.h"
#include <iostream>
#include <cmath>

#define LEVEL_WIDTH 33
#define LEVEL_HEIGHT 16

constexpr char SPRITESHEET_FILEPATH[] = "assets/george_0.png",
           PLAYER2_SPRITESHEET_FILEPATH[] = "assets/char.png",
           PLATFORM_FILEPATH[]    = "assets/platformPack_tile027.png",
           ENEMY_FILEPATH[]       = "assets/soph.png",
           BULLET_FILEPATH[]      = "assets/bullet.png",
           FONT_FILEPATH[]        = "assets/font1.png",
           BLUE_HP_BAR_FILEPATH[]  = "assets/blueHP2.png",
           GREEN_HP_BAR_FILEPATH[] = "assets/greenHP2.png",
           RED_HP_BAR_FILEPATH[] = "assets/redHP2.png",
           BACKGROUND_FILEPATH[]  = "assets/backgroundA2.png";

const int MAX_PROJECTILES_B = 6;
const int enemy_num_B = 3;
GLuint g_font_texture_id_B;
GLuint background_texture_id_B;

unsigned int LEVEL_DATA_B[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2,
    7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
    7, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
    7, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 7,
    7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
    7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 7,
    7, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
    7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
    7, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 7,
    7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
    7, 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 7,
    7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
    7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
    7, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 7,
};

unsigned int BACKGROUND_DATA_B[] =
{
    1
};

LevelB::~LevelB()
{
    delete [] m_game_state.enemies;
    delete [] m_game_state.projectiles;
    delete    m_game_state.player;
    delete    m_game_state.player2;
    delete    m_game_state.map;
    delete    m_game_state.background_map;
    delete    m_game_state.p1_health_bar;
    delete    m_game_state.p2_health_bar;
    delete [] m_game_state.enemy_health_bars;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeChunk(m_game_state.shoot_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelB::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/DesertMod2.png");
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    GLuint player2_texture_id = Utility::load_texture(PLAYER2_SPRITESHEET_FILEPATH);
    g_font_texture_id_B = Utility::load_texture(FONT_FILEPATH);
    background_texture_id_B = Utility::load_texture(BACKGROUND_FILEPATH);
    GLuint green_health_bar_texture_id = Utility::load_texture(GREEN_HP_BAR_FILEPATH);
    GLuint blue_health_bar_texture_id = Utility::load_texture(BLUE_HP_BAR_FILEPATH);
    GLuint red_health_bar_texture_id = Utility::load_texture(RED_HP_BAR_FILEPATH);
    GLuint projectile_texture_id = Utility::load_texture(BULLET_FILEPATH);
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA_B, map_texture_id, 1.0f, 5, 4);
    m_game_state.background_map = new Map(1, 1, BACKGROUND_DATA_B, background_texture_id_B, 16.0f, 1, 1);
    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);
    
    
    int player_walking_animation[4][4] =
    {
        { 1, 5, 9, 13 },  // for George to move to the left,
        { 3, 7, 11, 15 }, // for George to move to the right,
        { 2, 6, 10, 14 }, // for George to move upwards,
        { 0, 4, 8, 12 }   // for George to move downwards
    };

    // Player 2 might have a different sprite layout but we'll use the same indices for now
    int player2_walking_animation[4][4] =
       {
           { 4, 5, 6, 7  },  // for George to move to the left,
           { 8, 9, 10, 11}, // for George to move to the right,
           { 0, 1, 2, 3  }, // for George to move upwards,
           { 0, 4, 8, 12 }   // for George to move downwards
       };
    
    int health_bar_animation[5][1] = {
        {0},  // four health
        {1},  // three health
        {2},  // two health
        {3},  // one health
        {4}   // empty health
    };
    
    // PLAYER 1 initialization =========================================================
    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        0.9f,                      // width
        0.9f,                      // height
        PLAYER
    );
    
    m_game_state.player->set_position(glm::vec3(4.0f, 8.0f, 0.0f));
    m_game_state.player->set_scene(this);
    m_game_state.player->set_jumping_power(6.0f);
    
    m_game_state.p1_health_bar = new Entity(
        blue_health_bar_texture_id,
        0.0f,
        glm::vec3(0.0f),
        0.0f,
        health_bar_animation,
        0.0f,
        5,
        0,
        5,
        1,
        2.0f,
        0.2f,
        HEALTH_BAR
    );

    glm::vec3 p1_pos = m_game_state.player->get_position();
    m_game_state.p1_health_bar->set_position(glm::vec3(p1_pos.x, p1_pos.y + 0.5f, 0));
    
    // =================================================================================

    // PLAYER 2 initialization =========================================================
    m_game_state.player2 = new Entity(
        player2_texture_id,        // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        player2_walking_animation, // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        0.9f,                      // width
        0.9f,                      // height
        PLAYER2
    );
    
    m_game_state.player2->set_position(glm::vec3(26.0f, 8.0f, 0.0f));
    m_game_state.player2->set_scene(this);
    m_game_state.player2->face_left();
    m_game_state.player2->set_jumping_power(6.0f);
    
    m_game_state.p2_health_bar = new Entity(
        green_health_bar_texture_id,
        0.0f,
        glm::vec3(0.0f),
        0.0f,
        health_bar_animation,
        0.0f,
        5,
        0,
        5,
        1,
        2.0f,
        0.2f,
        HEALTH_BAR
    );
    
    glm::vec3 p2_pos = m_game_state.player2->get_position();
    m_game_state.p2_health_bar->set_position(glm::vec3(p2_pos.x, p2_pos.y + 0.75f, 0));
    // =================================================================================

    // Enemies initialization ==========================================================
    m_game_state.enemies = new Entity[enemy_num_B];
    m_game_state.enemy_health_bars = new Entity[enemy_num_B];

    for (int i = 0; i < enemy_num_B; i++)
    {
        m_game_state.enemies[i] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
        m_game_state.enemies[i].set_scene(this);
        m_game_state.enemies[i].activate();
        m_game_state.enemy_health_bars[i] = Entity(
            red_health_bar_texture_id,
            0.0f,
            glm::vec3(0.0f),
            0.0f,
            health_bar_animation,
            0.0f,
            5,
            0,
            5,
            1,
            2.0f,
            0.2f,
            HEALTH_BAR
        );
        
        glm::vec3 enemy_pos = m_game_state.enemies[i].get_position();
        m_game_state.enemy_health_bars[i].set_position(glm::vec3(enemy_pos.x, enemy_pos.y + 1.0f, 0));
        m_game_state.enemy_health_bars[i].four_health();
    }

//  enemy 1
    m_game_state.enemies[0].set_position(glm::vec3(16.0f, 10.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(acceleration);
//  enemy 2
    m_game_state.enemies[1].set_position(glm::vec3(8.0f, 5.0f, 0.0f));
    m_game_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[1].set_acceleration(acceleration);
//  enemy 3
    m_game_state.enemies[2].set_position(glm::vec3(23.0f, 5.0f, 0.0f));
    m_game_state.enemies[2].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[2].set_acceleration(acceleration);
    // =================================================================================

    // BGM and SFX initialization ======================================================

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("assets/bgMusic_fixed.wav");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(30.0f);
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/bounce.wav");
    m_game_state.shoot_sfx = Mix_LoadWAV("assets/shoot.wav");
    // =================================================================================

    // Projectiles initialization ======================================================
    m_game_state.projectiles = new Entity[MAX_PROJECTILES_B];
    
    // Setup all projectiles (inactive by default)
    for (int i = 0; i < MAX_PROJECTILES_B; i++) {
        m_game_state.projectiles[i] = Entity(projectile_texture_id, 10.0f, 0.2f, 0.2f, PROJECTILE);
        m_game_state.projectiles[i].deactivate();
        m_game_state.projectiles[i].set_scene(this);
    }
    // =================================================================================
    


}

void LevelB::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, enemy_num_B, m_game_state.map);
    m_game_state.player2->update(delta_time, m_game_state.player2, m_game_state.enemies, enemy_num_B, m_game_state.map);
    
    update_health_bars(delta_time);
    update_enemy_health_bars(delta_time);
   
    // Check if either player has 1 life left and change music
    if (m_game_state.player->get_lives() == 1 || m_game_state.player2->get_lives() == 1) {
        // Change to danger music if not already playing
        bool danger_music_playing = false;
        if (!danger_music_playing) {

            m_game_state.bgm = Mix_LoadMUS("assets/intenseMusic_fixed.wav");
            Mix_PlayMusic(m_game_state.bgm, -1);
            Mix_VolumeMusic(40.0f);
            danger_music_playing = true;
        }
    }

    // Check if player is out of bounds ================================================
    if (m_game_state.player->get_position().y < -18.0f) {
        m_game_state.player->lose_life();
        m_game_state.player->set_position(glm::vec3(4.0f, 8.0f, 0.0f));
    }
   
    if (m_game_state.player2->get_position().y < -18.0f) {
        m_game_state.player2->lose_life();
        m_game_state.player2->set_position(glm::vec3(26.0f, 8.0f, 0.0f));
    }
    // =================================================================================

    // Let the enemies chase the closest player to them ================================
    for (int i = 0; i < enemy_num_B; i++)
    {
        if (m_game_state.enemies[i].get_is_active())
        {
            // Determine which player is closer to this enemy
            glm::vec3 enemy_pos = m_game_state.enemies[i].get_position();
            glm::vec3 player1_pos = m_game_state.player->get_position();
            glm::vec3 player2_pos = m_game_state.player2->get_position();
            
            // Calculate distances to both players
            float distance_to_player1 = sqrt(
                pow(enemy_pos.x - player1_pos.x, 2) +
                pow(enemy_pos.y - player1_pos.y, 2)
            );
            
            float distance_to_player2 = sqrt(
                pow(enemy_pos.x - player2_pos.x, 2) +
                pow(enemy_pos.y - player2_pos.y, 2)
            );
            
            // Update enemy AI based on which player is closer
            if (distance_to_player1 <= distance_to_player2)
            {
                // Player 1 is closer or equidistant, chase player 1
                m_game_state.enemies[i].update(delta_time, m_game_state.player, nullptr, 0, m_game_state.map);
            } else
            {
                // Player 2 is closer, chase player 2
                m_game_state.enemies[i].update(delta_time, m_game_state.player2, nullptr, 0, m_game_state.map);
            }
            
            // Check collisions with both players
            if (m_game_state.player->check_collision(&m_game_state.enemies[i]))
            {
                m_game_state.player->lose_life();
                m_game_state.player->set_position(glm::vec3(4.0f, 8.0f, 0.0f));
                
            }
            
            if (m_game_state.player2->check_collision(&m_game_state.enemies[i]))
            {
                m_game_state.player2->lose_life();
                m_game_state.player2->set_position(glm::vec3(26.0f, 8.0f, 0.0f));
                
            }
        }
    }
    // =================================================================================

    // Check the collision between projectiles and players =============================
    for (int i = 0; i < MAX_PROJECTILES_B; i++)
    {
        Entity* projectile = &m_game_state.projectiles[i];
        
        if (projectile->get_is_active()) {
            projectile->update(delta_time, nullptr, nullptr, 0, m_game_state.map);
            
            // Check for collisions with player 1, but only if projectile wasn't fired by player 1
            if (projectile->get_owner() != m_game_state.player &&
                projectile->check_collision(m_game_state.player)) {
                projectile->deactivate();
                m_game_state.player->lose_life();
                m_game_state.player->set_position(glm::vec3(4.0f, 8.0f, 0.0f));
            }
            
            // Check for collisions with player 2, but only if projectile wasn't fired by player 2
            if (projectile->get_owner() != m_game_state.player2 &&
                projectile->check_collision(m_game_state.player2)) {
                projectile->deactivate();
                m_game_state.player2->lose_life();
                m_game_state.player2->set_position(glm::vec3(26.0f, 8.0f, 0.0f));
            }
            
            // check collision with enemies
            for (int j = 0; j < enemy_num_B; j++) {
                if (m_game_state.enemies[j].get_is_active() &&
                    projectile->check_collision(&m_game_state.enemies[j])) {
                    projectile->deactivate();
                    m_game_state.enemies[j].take_damage();
                    Mix_PlayChannel(-1, m_game_state.shoot_sfx, 0);
                    break;
                }
            }
            
            // check collision with walls
            if (projectile->get_collided_left() || projectile->get_collided_right() || projectile->get_collided_top() || projectile->get_collided_bottom())
            {
                projectile->deactivate();
            }
            
            // prevent projectiles from going forever
            float distance_x = std::abs(projectile->get_position().x - m_game_state.player->get_position().x);
            float distance_y = std::abs(projectile->get_position().y - m_game_state.player->get_position().y);
            if (distance_x > 30.0f || distance_y > 30.0f) {
                projectile->deactivate();
            }
        }
    }
    // =================================================================================

}

void LevelB::render(ShaderProgram *g_shader_program)
{
    m_game_state.background_map->render(g_shader_program);
    
    // Then render the rest of the elements
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    m_game_state.player2->render(g_shader_program);
    m_game_state.p1_health_bar->render(g_shader_program);
    m_game_state.p2_health_bar->render(g_shader_program);
    
    // Render enemies (only active ones)
    for (int i = 0; i < enemy_num_B; i++) {
        if (m_game_state.enemies[i].get_is_active()) {
            m_game_state.enemies[i].render(g_shader_program);
            // Render enemy health bar
            m_game_state.enemy_health_bars[i].render(g_shader_program);
        }
    }
    
    // Render projectiles directly in the render method
    for (int i = 0; i < MAX_PROJECTILES_B; i++) {
        if (m_game_state.projectiles[i].get_is_active()) {
            m_game_state.projectiles[i].render(g_shader_program);
        }
    }
    
//    glm::vec3 player1_position = m_game_state.player->get_position();
//    glm::vec3 player2_position = m_game_state.player2->get_position();
//
//    int current_p1_lives = m_game_state.player->get_lives();
//    int current_p2_lives = m_game_state.player2->get_lives();
//
//    float text_size = 0.8f;
//    float spacing = 0.0001f;
    
    // Utility::draw_text(g_shader_program, g_font_texture_id_A,
    //                     "HP:" + std::to_string(current_p1_lives),
    //                     text_size,
    //                     spacing,
    //                     glm::vec3(player1_position.x - 2.0f, player1_position.y + 1.0f, 0));
    // Utility::draw_text(g_shader_program, g_font_texture_id_A,
    //                     "HP:" + std::to_string(current_p2_lives),
    //                     text_size,
    //                     spacing,
    //                     glm::vec3(player2_position.x - 3.0f, player2_position.y + 1.0f, 0));

    // Debug player position ================================================
    // Utility::draw_text(g_shader_program, g_font_texture_id_A,
    //                 "X:" + std::to_string(int(player1_position.x)) + "\nY:" + std::to_string(int(player1_position.y)),
    //                 text_size,
    //                 spacing,
    //                 glm::vec3(player1_position.x + 2.0f, player1_position.y + 2.0f, 0));
    // Utility::draw_text(g_shader_program, g_font_texture_id_A,
    //                     "X:" + std::to_string(int(player2_position.x)) + "\nY:" + std::to_string(int(player2_position.y)),
    //                     text_size,
    //                     spacing,
    //                     glm::vec3(player2_position.x - 3.0f, player2_position.y + 2.0f, 0));
    // ======================================================================

    // // Display enemy health if active
    // for (int i = 0; i < enemy_num; i++) {
    //     if (m_game_state.enemies[i].get_is_active()) {
    //         glm::vec3 enemy_position = m_game_state.enemies[i].get_position();
    //         Utility::draw_text(g_shader_program, g_font_texture_id_A,
    //                           "HP:" + std::to_string(m_game_state.enemies[i].get_enemy_lives()),
    //                           text_size,
    //                           spacing,
    //                           glm::vec3(enemy_position.x - 2.0f, enemy_position.y + 1.0f, 0));
    //     }
    // }
}

void LevelB::shoot_projectile(Entity* shooter)
{
    // Find an inactive projectile
    for (int i = 0; i < MAX_PROJECTILES_B; i++) {
        Entity* projectile = &m_game_state.projectiles[i];
        
        if (!projectile->get_is_active()) {
            // Set position to shooter's position
            glm::vec3 position = shooter->get_position();
            projectile->set_position(position);
            
            // Set the owner of the projectile to the shooter
            projectile->set_owner(shooter);
            
            // Set velocity based on the direction the shooter is facing
            glm::vec3 direction(0);
            
            // Compare animation indices directly with the walking animations
            // This tells us which way the player is facing
            const int* current_anim = shooter->get_animation_indices();
            const int* left_anim = shooter->get_walking_indices(LEFT);
            const int* right_anim = shooter->get_walking_indices(RIGHT);
            const int* up_anim = shooter->get_walking_indices(UP);
            const int* down_anim = shooter->get_walking_indices(DOWN);
            
            bool facing_left = false;
            bool facing_right = false;
            
            // Compare the first frame of each animation direction
            if (current_anim && left_anim && current_anim[0] == left_anim[0]) {
                direction.x = -1.0f;
                facing_left = true;
            }
            else if (current_anim && right_anim && current_anim[0] == right_anim[0]) {
                direction.x = 1.0f;
                facing_right = true;
            }
            else if (current_anim && up_anim && current_anim[0] == up_anim[0]) {
                direction.y = 1.0f;
            }
            else if (current_anim && down_anim && current_anim[0] == down_anim[0]) {
                direction.y = -1.0f;
            }
            
            
            // Offset the projectile position slightly in front of the shooter
            position.x += direction.x * 0.5f;
            position.y += direction.y * 0.5f;
            projectile->set_position(position);
            
            
            projectile->set_movement(direction);
            projectile->set_velocity(direction * 10.0f);
            projectile->activate();
           
            Mix_PlayChannel(-1, m_game_state.shoot_sfx, 0);
            
            break;
        }
    }
}

void LevelB::update_health_bars(const float& delta_time)
{
    // Get player positions
    glm::vec3 p1_position = m_game_state.player->get_position();
    glm::vec3 p2_position = m_game_state.player2->get_position();
    
    // Position health bars above players
    m_game_state.p1_health_bar->set_position(glm::vec3(p1_position.x, p1_position.y + 0.5f, 0));
    m_game_state.p2_health_bar->set_position(glm::vec3(p2_position.x, p2_position.y + 0.75f, 0));
    
    // Get current health values
    int p1_lives = m_game_state.player->get_lives();
    int p2_lives = m_game_state.player2->get_lives();

    int p1_health_frame = 4 - p1_lives;
    int p2_health_frame = 4 - p2_lives;

    if (p1_health_frame == 0){m_game_state.p1_health_bar->four_health();}
    else if (p1_health_frame == 1){m_game_state.p1_health_bar->three_health();}
    else if (p1_health_frame == 2){m_game_state.p1_health_bar->two_health();}
    else if (p1_health_frame == 3){m_game_state.p1_health_bar->one_health();}
    else {m_game_state.p1_health_bar->empty_health();}

    if (p2_health_frame == 0){m_game_state.p2_health_bar->four_health();}
    else if (p2_health_frame == 1){m_game_state.p2_health_bar->three_health();}
    else if (p2_health_frame == 2){m_game_state.p2_health_bar->two_health();}
    else if (p2_health_frame == 3){m_game_state.p2_health_bar->one_health();}
    else {m_game_state.p2_health_bar->empty_health();}

    m_game_state.p1_health_bar->update(delta_time, nullptr, nullptr, 0, m_game_state.map);
    m_game_state.p2_health_bar->update(delta_time, nullptr, nullptr, 0, m_game_state.map);
}

void LevelB::update_enemy_health_bars(const float& delta_time)
{
    // Update health bars for each active enemy
    for (int i = 0; i < enemy_num_B; i++) {
        if (m_game_state.enemies[i].get_is_active()) {
            glm::vec3 enemy_pos = m_game_state.enemies[i].get_position();
            m_game_state.enemy_health_bars[i].set_position(glm::vec3(enemy_pos.x, enemy_pos.y + 1.0f, 0));
            int enemy_lives = m_game_state.enemies[i].get_enemy_lives();
            
            // Calculate health bar frame based on enemy health (assuming max health of 10)
            int enemy_health_percentage = (enemy_lives * 100) / 10;
            int health_frame;
            
            if (enemy_health_percentage > 75) {health_frame = 0;}
            else if (enemy_health_percentage > 50) {health_frame = 1;}
            else if (enemy_health_percentage > 25) {health_frame = 2;}
            else if (enemy_health_percentage > 0) {health_frame = 3;}
            else {health_frame = 4;}
            
            if (health_frame == 0){ m_game_state.enemy_health_bars[i].four_health();}
            else if (health_frame == 1){ m_game_state.enemy_health_bars[i].three_health();}
            else if (health_frame == 2){ m_game_state.enemy_health_bars[i].two_health();}
            else if (health_frame == 3){ m_game_state.enemy_health_bars[i].one_health();}
            else {m_game_state.enemy_health_bars[i].empty_health();}
            

            m_game_state.enemy_health_bars[i].update(delta_time, nullptr, nullptr, 0, m_game_state.map);
        }
    }
}
