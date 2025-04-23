#include "Menu.h"
#include "Utility.h"
#include <string>
#include <vector>

//#define LEVEL_WIDTH 14
//#define LEVEL_HEIGHT 8

constexpr char  FONT_FILEPATH[] = "assets/font1.png",
BACKGROUND_FILEPATH[] = "";

GLuint g_font_texture_id_M;

//unsigned int MENU_LEVEL_DATA[] =
//{
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
//};

Menu::~Menu()
{
//    delete [] m_game_state.enemies;
//    delete    m_game_state.player;
//    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void Menu::initialise()
{
    g_font_texture_id_M = Utility::load_texture(FONT_FILEPATH);
    
    m_game_state.player = nullptr;
    m_game_state.player2 = nullptr;
    m_game_state.map = nullptr;
    m_game_state.enemies = nullptr;
    m_game_state.projectiles = nullptr;
   
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("assets/dooblydoo.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(0.0f);
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/bounce.wav");
    m_game_state.next_level = Mix_LoadWAV("assets/win.WAV");
}

void Menu::update(float delta_time)
{
    
}


void Menu::render(ShaderProgram *g_shader_program)
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    float text_size = 1.0f;
    
    std::string message = "Welcome to my game";
    Utility::draw_text(g_shader_program, g_font_texture_id_M, message, text_size, 0.00005f, glm::vec3(-8.0f, 2.0f, 0.0f));
    
    std::string message2 = "Press enter to start";
    Utility::draw_text(g_shader_program, g_font_texture_id_M, message2, text_size, 0.000005f, glm::vec3(-8.7f, 1.0f, 0.0f));
}

void Menu::shoot_projectile(Entity* shooter)
{

}
