/**
* Author: Eric Zhao
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 20
#define LEVEL_HEIGHT 8


constexpr char SPRITESHEET_FILEPATH_B[] = "assets/char.png",
           PLATFORM_FILEPATH_B[]    = "assets/platformPack_tile027.png",
           ENEMY_FILEPATH_B[]       = "assets/soph.png",
           FONT_FILEPATH_B[]        = "assets/font1.png";

GLuint g_font_texture_id_B;

const int ENEMY_NUM_B = 4;

static unsigned int LEVEL_DATA_B[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0,
    3, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1,
    3, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2,
    3, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2, 2, 1, 2, 2, 1, 1, 2, 2, 2
};

LevelB::~LevelB()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
    Mix_FreeChunk(m_game_state.die_effects);
}

void LevelB::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/tilesetREALWORLD.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA_B, map_texture_id, 1.0f, 4, 1);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH_B);
    
    g_font_texture_id_B = Utility::load_texture(FONT_FILEPATH_B);

    int player_walking_animation[4][4] =
    {
        { 4, 5, 6, 7 },  // for George to move to the left,
        { 8, 9, 10, 11 }, // for George to move to the right,
        {0, 1, 2, 3}, // for George to move upwards,
        { 0, 4, 8, 12 }   // for George to move downwards
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);

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
        1.0f,                      // width
        1.0f,                       // height
        PLAYER
    );
    
    m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(3.0f);
    
    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH_B);

    m_game_state.enemies = new Entity[ENEMY_NUM_B];

    for (int i = 0; i < ENEMY_NUM_B; i++)
    {
        m_game_state.enemies[i] =  Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
    }

    m_game_state.enemies[0].set_position(glm::vec3(7.0f, -3.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(1.0f, 0.0f, 0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    m_game_state.enemies[1].set_position(glm::vec3(12.0f, -5.0f, 0.0f));
    m_game_state.enemies[1].set_movement(glm::vec3(1.5f, 0.0f, 0.0f));
    m_game_state.enemies[1].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    m_game_state.enemies[2].set_position(glm::vec3(17.0f, -3.0f, 0.0f));
    m_game_state.enemies[2].set_movement(glm::vec3(2.0f, 0.0f, 0.0f));
//    m_game_state.enemies[2].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_game_state.enemies[2].set_ai_type(FLYER);
    m_game_state.enemies[2].set_ai_state(FLYING);
    
    m_game_state.enemies[3].set_position(glm::vec3(15.0f, -5.0f, 0.0f));
    m_game_state.enemies[3].set_movement(glm::vec3(1.5f, 0.0f, 0.0f));
    m_game_state.enemies[3].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    
    
    
//    for (int i = 0; i < WALKER; i++)
//    {
//        m_game_state.enemies[i].activate();
//    }


    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("assets/Horror.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(30.0f);
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/cartoon-jump-6462.WAV");
    m_game_state.die_effects = Mix_LoadWAV("assets/death.WAV");
    m_game_state.next_level = Mix_LoadWAV("assets/win.WAV");
}

void LevelB::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_NUM_B, m_game_state.map);
    
    for (int i = 0; i < ENEMY_NUM_B; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }
    
    // Check if player has fallen below -10.0f
    if (m_game_state.player->get_position().y < -10.0f)
    {
        return;
    }
    
    for (int i = 0; i < ENEMY_NUM_B; i++)
    {
        if (m_game_state.enemies[i].check_activate() == true)
        {
            if (m_game_state.player->check_collision(&m_game_state.enemies[i]) == true)
            {
                m_game_state.player->dec_live();
                Mix_PlayChannel(-1, m_game_state.die_effects, 0);
                m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
            }
        }
    }
}


void LevelB::render(ShaderProgram *g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < ENEMY_NUM_B; i++)
            m_game_state.enemies[i].render(g_shader_program);
    
    std::string message = "LIVES:" + std::to_string(m_game_state.player->get_lives());
    Utility::draw_text(g_shader_program, g_font_texture_id_B, message, 0.4f, 0.05f, glm::vec3(1.0f, -1.0f, 0.0f));
    
    std::string level_message = "LEVEL 2";
    Utility::draw_text(g_shader_program, g_font_texture_id_B, level_message, 0.4f, 0.05f, glm::vec3(1.0f, -1.5f, 0.0f));
    
    std::string new_level = "Jump down to";
    Utility::draw_text(g_shader_program, g_font_texture_id_B, new_level, 0.4f, 0.0005f, glm::vec3(15.0f, -1.0f, 0.0f));
    
    std::string new_level2 = "go next level";
    Utility::draw_text(g_shader_program, g_font_texture_id_B, new_level2, 0.4f, 0.0005f, glm::vec3(15.0f, -1.5f, 0.0f));
    
    
}
