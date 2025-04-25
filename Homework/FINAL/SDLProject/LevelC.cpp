#include "LevelC.h"
#include "Utility.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <random>

using namespace std;

#define LEVEL_WIDTH 33
#define LEVEL_HEIGHT 16

constexpr char SPRITESHEET_FILEPATH[] = "assets/player1Sprite.png",
           PLAYER2_SPRITESHEET_FILEPATH[] = "assets/char2Sprite.png",
           PLATFORM_FILEPATH[]    = "assets/platformPack_tile027.png",
           ENEMY_FILEPATH[]       = "assets/001.png",
           BULLET_FILEPATH[]      = "assets/redLaser.png",
           FONT_FILEPATH[]        = "assets/font1.png",
           BLUE_HP_BAR_FILEPATH[]  = "assets/blueHP2.png",
           GREEN_HP_BAR_FILEPATH[] = "assets/greenHP2.png",
           RED_HP_BAR_FILEPATH[] = "assets/redHP2.png",
           BACKGROUND_FILEPATH[]  = "assets/spacebackground.png";

// List of enemy sprite file paths
vector<string> ALL_ENEMY_FILEPATHS = {
    "assets/001.png",
    "assets/002.png",
    "assets/003.png",
    "assets/004.png",
    "assets/005.png"
};

const int MAX_PROJECTILES_C = 6;
const int enemy_num_C = 20;
GLuint g_font_texture_id_C;
GLuint background_texture_id_C;

// Random number generator
mt19937 rng(random_device{}());

unsigned int LEVEL_DATA_C[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    5, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 5,
    5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    5, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 5,
    5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    5, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    5, 0, 0, 0, 0, 0, 0, 0,11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11, 0, 0, 0, 0, 0, 0, 0, 5,
    5, 0, 0, 0, 0, 0, 0,11, 0, 0, 0, 0, 0, 0, 0, 0,10,10, 0, 0, 0, 0, 0, 0, 0,11, 0, 0, 0, 0, 0, 0, 5,
    5, 0, 0, 0, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0,10, 0, 0,10, 0, 0, 0, 0, 0, 0, 0, 7, 8, 9, 0, 0, 0, 5,
    5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,10, 0, 0, 0, 0,10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0
};


LevelC::~LevelC()
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

void LevelC::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/spaceFloor.png");
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    GLuint player2_texture_id = Utility::load_texture(PLAYER2_SPRITESHEET_FILEPATH);
    g_font_texture_id_C = Utility::load_texture(FONT_FILEPATH);
    background_texture_id_C = Utility::load_texture(BACKGROUND_FILEPATH);
    GLuint green_health_bar_texture_id = Utility::load_texture(GREEN_HP_BAR_FILEPATH);
    GLuint blue_health_bar_texture_id = Utility::load_texture(BLUE_HP_BAR_FILEPATH);
    GLuint red_health_bar_texture_id = Utility::load_texture(RED_HP_BAR_FILEPATH);
    GLuint projectile_texture_id = Utility::load_texture(BULLET_FILEPATH);
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    
    for (const string& filepath : ALL_ENEMY_FILEPATHS) {
        m_enemy_texture_ids.push_back(Utility::load_texture(filepath.c_str()));
    }
    
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA_C, map_texture_id, 1.0f, 12, 1);
//    m_game_state.background_map = new Map(1, 1, BACKGROUND_DATA_C, background_texture_id_C, 16.0f, 1, 1);
    glm::vec3 acceleration = glm::vec3(0.0f, -2.00f, 0.0f);
    
    
    int player_walking_animation[4][4] =
    {
        { 1, 5, 9, 13 },  // for George to move to the left,
        { 3, 7, 11, 15 }, // for George to move to the right,
        { 2, 6, 10, 14 }, // for George to move upwards,
        { 0, 4, 8, 12 }   // for George to move downwards
    };
    
    int player2_walking_animation[4][4] =
    {
        { 4, 5, 6, 7  },  // for George to move to the left,
        { 8, 9, 10, 11}, // for George to move to the right,
        { 0, 1, 2, 3  }, // for George to move upwards,
        { 0, 4, 8, 12 }   // for George to move downwards
    };
    
    int health_bar_animation[5][1] = {
        {0},    
        {1},  
        {2},  
        {3},  
        {4}   
    };
    
    // Background initialize ===========================================================
    m_game_state.background_map = new Entity(background_texture_id_C, 0.0f, 33.0f, 30.0f, BACKGROUND);
    m_game_state.background_map->set_position(glm::vec3(16.0f, -5.0f, 0.0f));
    
    
    // =================================================================================
    
    // PLAYER 1 initialization =========================================================
    m_game_state.player = new Entity(
         player_texture_id,         // texture id
         5.0f,                      // speed
         acceleration,              // acceleration
         5.0f,                      // jumping power
         player2_walking_animation,  // animation index sets
         0.0f,                      // animation time
         4,                         // animation frame amount
         0,                         // current animation index
         4,                         // animation column amount
         4,                         // animation row amount
         0.75f,                      // width
         0.999f,                      // height
         PLAYER
     );
    
    m_game_state.player->set_position(glm::vec3(5.0f, 3.0f, 0.0f));
    m_game_state.player->set_scene(this);
    m_game_state.player->set_jumping_power(6.0f);
    m_game_state.player->set_lives(4);
    
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
    m_game_state.p1_health_bar->set_position(glm::vec3(p1_pos.x, p1_pos.y + 0.8f, 0));
    
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
      0.75f,                      // width
      0.999f,                      // height
      PLAYER2
  );
    
    m_game_state.player2->set_position(glm::vec3(28.0f, 3.0f, 0.0f));
    m_game_state.player2->set_scene(this);
    m_game_state.player2->face_left();
    m_game_state.player2->set_jumping_power(6.0f);
    m_game_state.player2->set_lives(4);
    
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
    m_game_state.enemies = new Entity[enemy_num_C];
    m_game_state.enemy_health_bars = new Entity[enemy_num_C];
    
    for (int i = 0; i < enemy_num_C; i++)
    {
        m_game_state.enemies[i] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY);
        m_game_state.enemies[i].set_scene(this);
        m_game_state.enemies[i].deactivate(); 
        m_game_state.enemies[i].set_enemy_lives(10);
        m_game_state.enemies[i].set_ai_type(GUARD); 
        m_game_state.enemies[i].set_ai_state(IDLE);
        m_game_state.enemies[i].set_speed(2.0f);
        m_game_state.enemies[i].set_movement(glm::vec3(0.0f));
        m_game_state.enemies[i].set_acceleration(glm::vec3(0.0f, -1.75f, 0.0f));
        
        // Initialize health bars for enemies
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
        
        // glm::vec3 enemy_pos = m_game_state.enemies[i].get_position();
        // m_game_state.enemy_health_bars[i].set_position(glm::vec3(enemy_pos.x, enemy_pos.y + 1.0f, 0));
        m_game_state.enemy_health_bars[i].four_health();
    }
    // =================================================================================
    
    // BGM and SFX initialization ======================================================
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("assets/battle.wav");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(30.0f);
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/bounce.wav");
    m_game_state.shoot_sfx = Mix_LoadWAV("assets/laserSound.wav");
    // =================================================================================
    
    // Projectiles initialization ======================================================
    m_game_state.projectiles = new Entity[MAX_PROJECTILES_C];
    
    // Setup all projectiles (inactive by default)
    for (int i = 0; i < MAX_PROJECTILES_C; i++) {
        m_game_state.projectiles[i] = Entity(projectile_texture_id, 10.0f, 1.0f, 0.2f, PROJECTILE);
        m_game_state.projectiles[i].deactivate();
        m_game_state.projectiles[i].set_scene(this);
    }
    // =================================================================================
    
   
    
}

void LevelC::update(float delta_time)
{
    m_total_game_time += delta_time;
    m_spawn_timer += delta_time;
    m_spawn_interval = 1.0f;
    if (m_total_game_time > 30.0f) {
        m_spawn_interval = 0.75f;
    }
    
    // Spawn enemies based on interval
    if (m_spawn_timer >= m_spawn_interval) {
        m_spawn_timer = 0;
        spawn_enemy_above_player(m_game_state.player);
        spawn_enemy_above_player(m_game_state.player2);
    }
    
    m_game_state.background_map->update(delta_time, nullptr, nullptr, 0, m_game_state.map);
    
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, enemy_num_C, m_game_state.map);
    m_game_state.player2->update(delta_time, m_game_state.player2, m_game_state.enemies, enemy_num_C, m_game_state.map);
    
    update_health_bars(delta_time);
    update_enemy_health_bars(delta_time);
   
    if ((m_game_state.player->get_lives() == 1 || m_game_state.player2->get_lives() == 1) && !m_danger_music_playing) {
        Mix_HaltMusic();
        Mix_FreeMusic(m_game_state.bgm);
        m_game_state.bgm = Mix_LoadMUS("assets/forestintensemusic.wav");
        Mix_PlayMusic(m_game_state.bgm, -1);
        Mix_VolumeMusic(50.0f);
        m_danger_music_playing = true;
    }

    // Check if player is out of bounds ================================================
    if (m_game_state.player->get_position().y < -18.0f) {
        m_game_state.player->lose_life();
        m_game_state.player->set_position(glm::vec3(5.0f, 3.0f, 0.0f));
    }
   
    if (m_game_state.player2->get_position().y < -18.0f) {
        m_game_state.player2->lose_life();
        m_game_state.player2->set_position(glm::vec3(28.0f, 3.0f, 0.0f));
    }
    // =================================================================================

    // Update each active enemy ========================================================
    for (int i = 0; i < enemy_num_C; i++)
    {
        if (m_game_state.enemies[i].get_is_active())
        {
            // Get the target player this enemy should chase
            Entity* target_player = m_game_state.enemies[i].get_owner();
            
            // If we have a target player, activate AI to chase them
            if (target_player) {
                m_game_state.enemies[i].ai_activate(target_player);
                m_game_state.enemies[i].update(delta_time, target_player, nullptr, 0, m_game_state.map);
                
                // Update the health bar position for this enemy
                glm::vec3 enemy_pos = m_game_state.enemies[i].get_position();
                m_game_state.enemy_health_bars[i].set_position(glm::vec3(enemy_pos.x, enemy_pos.y + 1.0f, 0));
                
        
                // Check collision with player 1
                if (m_game_state.player->check_collision(&m_game_state.enemies[i]))
                {
                    m_game_state.player->lose_life();
                    m_game_state.player->set_position(glm::vec3(5.0f, 5.0f, 0.0f));
                    
                    m_game_state.enemies[i].deactivate();
                    m_game_state.enemy_health_bars[i].deactivate();
                }
                // Check collision with player 2
                else if (m_game_state.player2->check_collision(&m_game_state.enemies[i]))
                {
                    m_game_state.player2->lose_life();
                    m_game_state.player2->set_position(glm::vec3(28.0f, 12.0f, 0.0f));
                    
                    // Deactivate the enemy after collision
                    m_game_state.enemies[i].deactivate();
                    m_game_state.enemy_health_bars[i].deactivate();
                }
            } else {
              
                m_game_state.enemies[i].update(delta_time, nullptr, nullptr, 0, m_game_state.map);
                glm::vec3 enemy_pos = m_game_state.enemies[i].get_position();
                m_game_state.enemy_health_bars[i].set_position(glm::vec3(enemy_pos.x, enemy_pos.y + 1.0f, 0));
            }
            
    
        }
    }
    // =================================================================================


    for (int i = 0; i < enemy_num_C; i++) {
        if (m_game_state.enemies[i].get_is_active()) {
            glm::vec3 enemy_pos = m_game_state.enemies[i].get_position();
            if ( enemy_pos.y < -20.0f || enemy_pos.y > 20.0f ) {
                m_game_state.enemies[i].deactivate();
                m_game_state.enemy_health_bars[i].deactivate();
            }
        }
    }

    // Check the collision between projectiles and enemies =============================
    for (int i = 0; i < MAX_PROJECTILES_C; i++)
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
            
            // Check collision with enemies
            for (int j = 0; j < enemy_num_C; j++) {
                if (m_game_state.enemies[j].get_is_active() &&
                    projectile->check_collision(&m_game_state.enemies[j])) {
                    projectile->deactivate();
                    m_game_state.enemies[j].take_damage();
                    break;
                }
            }
            
            // check collision with walls
            if (projectile->get_collided_left() || projectile->get_collided_right() || 
                projectile->get_collided_top() || projectile->get_collided_bottom())
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

void LevelC::render(ShaderProgram *g_shader_program)
{
    m_game_state.background_map->render(g_shader_program);
    
//    for (int i = 0; i < enemy_num_C; i++) {
//        std::cout << "Enemy C" << i << " active: " << m_game_state.enemies[i].get_is_active() << std::endl;
//    }
    
    // Then render the rest of the elements
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    m_game_state.player2->render(g_shader_program);
    m_game_state.p1_health_bar->render(g_shader_program);
    m_game_state.p2_health_bar->render(g_shader_program);
    
    // Render enemies (only active ones)
    for (int i = 0; i < enemy_num_C; i++) {
        if (m_game_state.enemies[i].get_is_active()) {
            m_game_state.enemies[i].render(g_shader_program);
            // Render enemy health bar
            m_game_state.enemy_health_bars[i].render(g_shader_program);
        }
    }
    
    // Render projectiles directly in the render method
    for (int i = 0; i < MAX_PROJECTILES_C; i++) {
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

void LevelC::shoot_projectile(Entity* shooter)
{
    // Find an inactive projectile
    for (int i = 0; i < MAX_PROJECTILES_C; i++) {
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

void LevelC::update_health_bars(const float& delta_time)
{
    // Get player positions
    glm::vec3 p1_position = m_game_state.player->get_position();
    glm::vec3 p2_position = m_game_state.player2->get_position();
    
    // Position health bars above players
    m_game_state.p1_health_bar->set_position(glm::vec3(p1_position.x, p1_position.y + 0.8f, 0));
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

void LevelC::update_enemy_health_bars(const float& delta_time)
{
    // Update health bars for each active enemy
    for (int i = 0; i < enemy_num_C; i++) {
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

void LevelC::spawn_enemy_above_player(Entity* target_player)
{
    // Don't spawn enemies if player is above ground level
    if (target_player->get_position().y >= 0.0f) {
        return;
    }
    
    // Find an inactive enemy
    for (int i = 0; i < enemy_num_C; i++)
    {
        if (!m_game_state.enemies[i].get_is_active())
        {
            // Get a random texture from the available enemy textures
            uniform_int_distribution<int> dist(0, m_enemy_texture_ids.size() - 1);
            int random_index = dist(rng);
            GLuint random_texture_id = m_enemy_texture_ids[random_index];

            m_game_state.enemies[i].set_texture_id(random_texture_id);
            
            glm::vec3 position = target_player->get_position() + glm::vec3(0.0f, 6.0f, 0.0f);
            
            m_game_state.enemies[i].set_position(position);
            m_game_state.enemies[i].set_movement(glm::vec3(0.0f));
            m_game_state.enemies[i].set_speed(2.0f);
            
        
            m_game_state.enemies[i].set_acceleration(glm::vec3(0.0f, -1.75f, 0.0f));
            m_game_state.enemies[i].set_owner(target_player); 
            
            // Set the enemy to guard AI type
            m_game_state.enemies[i].set_ai_type(GUARD);
            m_game_state.enemies[i].set_ai_state(IDLE);
            
            m_game_state.enemies[i].activate();
            m_game_state.enemies[i].set_enemy_lives(1);
            
            m_game_state.enemy_health_bars[i].activate();
            glm::vec3 enemy_pos = m_game_state.enemies[i].get_position();
            m_game_state.enemy_health_bars[i].set_position(glm::vec3(enemy_pos.x, enemy_pos.y + 1.0f, 0));
            
            return;
        }
    }
}
