#include "LevelB.h"
#include "Utility.h"
#include <iostream>
#include <cmath>

#define LEVEL_WIDTH 33
#define LEVEL_HEIGHT 16

constexpr char SPRITESHEET_FILEPATH[] = "assets/player1Sprite.png",
           PLAYER2_SPRITESHEET_FILEPATH[] = "assets/char2Sprite.png",
           PLATFORM_FILEPATH[]    = "assets/platformPack_tile027.png",
           ENEMY_FILEPATH[]       = "assets/soph.png",
           VULTURE_FILEPATH[]     = "assets/Vulture.png",
           BULLET_FILEPATH[]      = "assets/arrow2.png",
           FONT_FILEPATH[]        = "assets/font1.png",
           BLUE_HP_BAR_FILEPATH[]  = "assets/blueHP2.png",
           GREEN_HP_BAR_FILEPATH[] = "assets/greenHP2.png",
           RED_HP_BAR_FILEPATH[] = "assets/redHP2.png",
           BACKGROUND_FILEPATH[]  = "assets/forestbg2.png";

const int MAX_PROJECTILES_B = 6;
const int enemy_num_B = 3;
GLuint g_font_texture_id_B;
GLuint background_texture_id_B;

unsigned int LEVEL_DATA_B[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
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
    GLuint map_texture_id = Utility::load_texture("assets/forest5.png");
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    GLuint player2_texture_id = Utility::load_texture(PLAYER2_SPRITESHEET_FILEPATH);
    g_font_texture_id_B = Utility::load_texture(FONT_FILEPATH);
    background_texture_id_B = Utility::load_texture(BACKGROUND_FILEPATH);
    GLuint green_health_bar_texture_id = Utility::load_texture(GREEN_HP_BAR_FILEPATH);
    GLuint blue_health_bar_texture_id = Utility::load_texture(BLUE_HP_BAR_FILEPATH);
    GLuint red_health_bar_texture_id = Utility::load_texture(RED_HP_BAR_FILEPATH);
    GLuint projectile_texture_id = Utility::load_texture(BULLET_FILEPATH);
//    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    GLuint vulture_texture_id = Utility::load_texture(VULTURE_FILEPATH);
    
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA_B, map_texture_id, 1.0f, 4, 1);
//    m_game_state.background_map = new Map(1, 1, BACKGROUND_DATA_B, background_texture_id_B, 16.0f, 1, 1);
    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);
    
//    
//    int player_walking_animation[4][4] =
//    {
//        { 1, 5, 9, 13 },  // for George to move to the left,
//        { 3, 7, 11, 15 }, // for George to move to the right,
//        { 2, 6, 10, 14 }, // for George to move upwards,
//        { 0, 4, 8, 12 }   // for George to move downwards
//    };

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
    
    // Background initialize ===========================================================
    m_game_state.background_map = new Entity(background_texture_id_B, 0.0f, 33.0f, 25.0f, BACKGROUND);
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
    
    m_game_state.player->set_position(glm::vec3(4.0f, 5.0f, 0.0f));
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
    m_game_state.p1_health_bar->four_health();
    
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
        0.725f,                      // width
        0.999f,                      // height
        PLAYER2
    );
    
    m_game_state.player2->set_position(glm::vec3(26.0f, 5.0f, 0.0f));
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
    m_game_state.p2_health_bar->four_health();
    // =================================================================================
    
    int vulture_walking_animation[4][4] = {
        { 0, 1, 2, 3 },
        { 4, 5, 6, 7 },
        { 0, 1, 2, 3 },
        { 0, 1, 2, 3 }
    };
    
    // Enemies initialization ==========================================================
    m_game_state.enemies = new Entity[enemy_num_B];
    m_game_state.enemy_health_bars = new Entity[enemy_num_B];

    for (int i = 0; i < enemy_num_B; i++)
    {
        m_game_state.enemies[i] = Entity(
            vulture_texture_id,
            1.0f,
            glm::vec3(0.0f, 0.0f, 0.0f),
            0.0f,
            vulture_walking_animation,
            0.25f,
            4,
            0,
            4,
            2,
            1.5f,
            1.0f,
            ENEMY
        );
        m_game_state.enemies[i].set_scene(this);
        m_game_state.enemies[i].activate();
        m_game_state.enemies[i].set_enemy_lives(10);
        m_game_state.enemies[i].set_ai_type(FLYER);
        
        if (i == 0) {
            m_game_state.enemies[i].set_position(glm::vec3(6.0f, -14.0f, 0.0f));
            m_game_state.enemies[i].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
            m_game_state.enemies[i].set_ai_state(DIAGONAL_FLY_LEFT);
            m_game_state.enemies[i].face_right();
        } else if (i == 1) {
            m_game_state.enemies[i].set_position(glm::vec3(24.5f, -14.0f, 0.0f));
            m_game_state.enemies[i].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
            m_game_state.enemies[i].set_ai_state(DIAGONAL_FLY_RIGHT);
        } else if (i == 2) {
            m_game_state.enemies[i].set_position(glm::vec3(15.0f, -8.0f, 0.0f));
            m_game_state.enemies[i].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
            m_game_state.enemies[i].set_ai_state(OVAL_FLY);
        }
        
        m_game_state.enemies[i].set_movement(glm::vec3(0.0f));
        
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
    // =================================================================================

    // BGM and SFX initialization ======================================================

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("assets/juglemusic.wav");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(30.0f);
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/jumppp11.ogg");
    m_game_state.shoot_sfx = Mix_LoadWAV("assets/arrowSound2.wav");
    // =================================================================================

    // Projectiles initialization ======================================================
    m_game_state.projectiles = new Entity[MAX_PROJECTILES_B];
    
    for (int i = 0; i < MAX_PROJECTILES_B; i++) {
        m_game_state.projectiles[i] = Entity(projectile_texture_id, 10.0f, 1.0f, 0.5f, PROJECTILE);
        m_game_state.projectiles[i].deactivate();
        m_game_state.projectiles[i].set_scene(this);
    }
    // =================================================================================
    


}

void LevelB::update(float delta_time)
{

    m_game_state.background_map->update(delta_time, nullptr, nullptr, 0, m_game_state.map);
    
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, enemy_num_B, m_game_state.map);
    m_game_state.player2->update(delta_time, m_game_state.player2, m_game_state.enemies, enemy_num_B, m_game_state.map);
    
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
        m_game_state.player->set_position(glm::vec3(4.0f, 5.0f, 0.0f));
    }
   
    if (m_game_state.player2->get_position().y < -18.0f) {
        m_game_state.player2->lose_life();
        m_game_state.player2->set_position(glm::vec3(26.0f, 5.0f, 0.0f));
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
            
         
            if (distance_to_player1 <= distance_to_player2)
            {
                // Player 1 is closer
                m_game_state.enemies[i].update(delta_time, m_game_state.player, nullptr, 0, m_game_state.map);
            } else
            {
                // Player 2 is closer
                m_game_state.enemies[i].update(delta_time, m_game_state.player2, nullptr, 0, m_game_state.map);
            }

            if (m_game_state.enemies[i].get_movement().x < 0) {
                m_game_state.enemies[i].face_left();
            }
            else if (m_game_state.enemies[i].get_movement().x > 0) {
                m_game_state.enemies[i].face_right();
            }
            
            if (m_game_state.player->check_collision(&m_game_state.enemies[i]))
            {
                m_game_state.player->lose_life();
                m_game_state.player->set_position(glm::vec3(4.0f, 5.0f, 0.0f)); 
            }
            
            if (m_game_state.player2->check_collision(&m_game_state.enemies[i]))
            {
                m_game_state.player2->lose_life();
                m_game_state.player2->set_position(glm::vec3(26.0f, 5.0f, 0.0f));
            }
        }
    }
    
    for (int i = 0; i < enemy_num_B; i++)
    {
        
        glm::vec3 enemy_pos = m_game_state.enemies[i].get_position();
        //        std::cout << "after check curr enemy pos y is " << enemy_pos.y << std::endl;
        //        std::cout << "after delta time is " << delta_time << std::endl;
        if (enemy_pos.y < -20.0f || std::isnan(enemy_pos.y) || enemy_pos.y > 20.0f)
        {
            if (i == 0){m_game_state.enemies[i].set_position(glm::vec3(16.0f, 2.0f, 0.0f));}
            else if (i == 1) { m_game_state.enemies[i].set_position(glm::vec3(8.0f, 0.0f, 0.0f));}
            else if (i == 2) { m_game_state.enemies[i].set_position(glm::vec3(23.0f, 0.0f, 0.0f));}
            m_game_state.enemies[i].set_movement(glm::vec3(0.0, 0.0, 0.0));
            m_game_state.enemies[i].set_velocity(glm::vec3(0.0, 0.0, 0.0));
            m_game_state.enemies[i].set_speed(1.0f);
        }
    }
    // =================================================================================

    // Check the collision between projectiles and players =============================
    for (int i = 0; i < MAX_PROJECTILES_B; i++)
    {
        Entity* projectile = &m_game_state.projectiles[i];
        
        if (projectile->get_is_active()) {
            projectile->update(delta_time, nullptr, nullptr, 0, m_game_state.map);
            
            // Check for collisions with P1
            if (projectile->get_owner() != m_game_state.player &&
                projectile->check_collision(m_game_state.player)) {
                projectile->deactivate();
                m_game_state.player->lose_life();
                m_game_state.player->set_position(glm::vec3(4.0f, 5.0f, 0.0f));
            }
            
            // Check for collisions with P2
            if (projectile->get_owner() != m_game_state.player2 &&
                projectile->check_collision(m_game_state.player2)) {
                projectile->deactivate();
                m_game_state.player2->lose_life();
                m_game_state.player2->set_position(glm::vec3(26.0f, 5.0f, 0.0f));
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
    
//    for (int i = 0; i < enemy_num_B; i++) {
//        std::cout << "Enemy B" << i << " active: " << m_game_state.enemies[i].get_is_active() << std::endl;
//    }
    
    // Then render the rest of the elements
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    m_game_state.player2->render(g_shader_program);
    m_game_state.p1_health_bar->render(g_shader_program);
    m_game_state.p2_health_bar->render(g_shader_program);
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
    
//     Utility::draw_text(g_shader_program, g_font_texture_id_B,
//                         "HP:" + std::to_string(current_p1_lives),
//                         text_size,
//                         spacing,
//                         glm::vec3(player1_position.x - 2.0f, player1_position.y + 1.0f, 0));
//     Utility::draw_text(g_shader_program, g_font_texture_id_B,
//                         "HP:" + std::to_string(current_p2_lives),
//                         text_size,
//                         spacing,
//                         glm::vec3(player2_position.x - 3.0f, player2_position.y + 1.0f, 0));
//
//    // Debug player position ================================================
//    // Utility::draw_text(g_shader_program, g_font_texture_id_A,
//    //                 "X:" + std::to_string(int(player1_position.x)) + "\nY:" + std::to_string(int(player1_position.y)),
//    //                 text_size,
//    //                 spacing,
//    //                 glm::vec3(player1_position.x + 2.0f, player1_position.y + 2.0f, 0));
//     Utility::draw_text(g_shader_program, g_font_texture_id_B,
//                         "X:" + std::to_string(int(player2_position.x)) + "\nY:" + std::to_string(int(player2_position.y)),
//                         text_size,
//                         spacing,
//                         glm::vec3(player2_position.x - 3.0f, player2_position.y + 2.0f, 0));
//    Utility::draw_text(g_shader_program, g_font_texture_id_B,
//                        "X",
//                        text_size,
//                        spacing,
//                        glm::vec3(20.3f, -8.5f, 0));
//    Utility::draw_text(g_shader_program, g_font_texture_id_B,
//                        "X",
//                        text_size,
//                        spacing,
//                        glm::vec3(25.5f, -14.0f, 0));
//    
//    // Bottom Point
//    Utility::draw_text(g_shader_program, g_font_texture_id_B,
//                       "Bottom",
//                       0.6f, 0.0001f,
//                       glm::vec3(15.0f - 1.0f, -7.0f + 1.0f, 0.0f));
//
//    // Left Point
//    Utility::draw_text(g_shader_program, g_font_texture_id_B,
//                       "Left",
//                       0.6f, 0.0001f,
//                       glm::vec3(7.0f - 1.0f, -4.0f + 1.0f, 0.0f));
//
//    // Right Point
//    Utility::draw_text(g_shader_program, g_font_texture_id_B,
//                       "Right",
//                       0.6f, 0.0001f,
//                       glm::vec3(22.0f - 1.0f, -4.0f + 1.0f, 0.0f));
//
//    // Top Point
//    Utility::draw_text(g_shader_program, g_font_texture_id_B,
//                       "Top",
//                       0.6f, 0.0001f,
//                       glm::vec3(15.0f - 1.0f, -1.0f + 1.0f, 0.0f));
    // ======================================================================

     // Display enemy health if active
//     for (int i = 0; i < enemy_num_B; i++) {
//         if (m_game_state.enemies[i].get_is_active()) {
//             glm::vec3 enemy_position = m_game_state.enemies[i].get_position();
//             Utility::draw_text(g_shader_program, g_font_texture_id_B,
//                               "HP:" + std::to_string(m_game_state.enemies[i].get_enemy_lives()),
//                               text_size,
//                               spacing,
//                               glm::vec3(enemy_position.x - 2.0f, enemy_position.y + 1.0f, 0));
//         }
//     }
}

void LevelB::shoot_projectile(Entity* shooter)
{
    
    for (int i = 0; i < MAX_PROJECTILES_B; i++) {
        Entity* projectile = &m_game_state.projectiles[i];
        
        if (!projectile->get_is_active()) {
            glm::vec3 position = shooter->get_position();
            projectile->set_position(position);
            
            projectile->set_owner(shooter);
            
            glm::vec3 direction(0);
            
            const int* current_anim = shooter->get_animation_indices();
            const int* left_anim = shooter->get_walking_indices(LEFT);
            const int* right_anim = shooter->get_walking_indices(RIGHT);
            const int* up_anim = shooter->get_walking_indices(UP);
            const int* down_anim = shooter->get_walking_indices(DOWN);
            
            bool facing_left = false;
            bool facing_right = false;
            
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

void LevelB::update_enemy_health_bars(const float& delta_time)
{
    // Update health bars for each active enemy
    for (int i = 0; i < enemy_num_B; i++) {
        if (m_game_state.enemies[i].get_is_active()) {
            glm::vec3 enemy_pos = m_game_state.enemies[i].get_position();
            m_game_state.enemy_health_bars[i].set_position(glm::vec3(enemy_pos.x, enemy_pos.y + 1.0f, 0));
            int enemy_lives = m_game_state.enemies[i].get_enemy_lives();
            
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

