#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 25
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "Menu.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "endGameP1.h"
#include "endGameP2.h"

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH  = 640 * 2.2,
          WINDOW_HEIGHT = 480 * 2.2;

constexpr float MENU_BG_RED     = 0.839f;
constexpr float MENU_BG_GREEN   = 0.541f;
constexpr float MENU_BG_BLUE    = 0.306f;

constexpr float LEVEL_A_BG_RED   = 0.839f;
constexpr float LEVEL_A_BG_GREEN = 0.541f;
constexpr float LEVEL_A_BG_BLUE  = 0.306f;

constexpr float LEVEL_B_BG_RED   = 0.18f;
constexpr float LEVEL_B_BG_GREEN = 0.42f;
constexpr float LEVEL_B_BG_BLUE  = 0.21f;

constexpr float LEVEL_C_BG_RED   = 0.1f;
constexpr float LEVEL_C_BG_GREEN = 0.1f;
constexpr float LEVEL_C_BG_BLUE  = 0.4f;

constexpr float BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;
float g_invert_effect_timer = 0.0f;
bool g_invert_effect_active = false;

enum AppStatus { RUNNING, TERMINATED };

// ————— GLOBAL VARIABLES ————— //
Scene *g_current_scene;
Menu *g_menu;
LevelA *g_level_a;
LevelB *g_level_b;
LevelC *g_level_c;
endGameP1 *g_endGamePlayer1;
endGameP2 *g_endGamePlayer2;

// Point system
int player1_points = 0;
int player2_points = 0;

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
    
    // Change background color based on which scene we're switching to
    if (scene == g_menu) {
        glClearColor(MENU_BG_RED, MENU_BG_GREEN, MENU_BG_BLUE, BG_OPACITY);
    } 
    else if (scene == g_level_a) {
        glClearColor(LEVEL_A_BG_RED, LEVEL_A_BG_GREEN, LEVEL_A_BG_BLUE, BG_OPACITY);
    }
    else if (scene == g_level_b) {
        glClearColor(LEVEL_B_BG_RED, LEVEL_B_BG_GREEN, LEVEL_B_BG_BLUE, BG_OPACITY);
    }
    else if (scene == g_level_c) {
        glClearColor(LEVEL_C_BG_RED, LEVEL_C_BG_GREEN, LEVEL_C_BG_BLUE, BG_OPACITY);
    } else if (scene == g_endGamePlayer1 || scene == g_endGamePlayer2) {
        glClearColor(MENU_BG_RED, MENU_BG_GREEN, MENU_BG_BLUE, BG_OPACITY);
    }
}

void initialise();
void process_input();
void update();
void render();
void shutdown();


void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Battle Arena - Player vs Player",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    g_shader_program.set_invert_colors(0);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-15.0f, 15.0f, -11.25f, 11.25f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    
    g_level_a = nullptr;
    g_level_b = nullptr;
    g_level_c = nullptr;
    g_menu = nullptr;
    g_endGamePlayer1 = nullptr;
    g_endGamePlayer2 = nullptr;
    
    // ————— LEVEL A SETUP ————— //
    if (g_menu != nullptr){
        delete g_menu;
    }
    
    g_menu = new Menu();
    switch_to_scene(g_menu);
    
    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{

    if (g_current_scene->get_state().player != nullptr && g_current_scene->get_state().player->get_is_active()) {
        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    }
    
    if (g_current_scene->get_state().player2 != nullptr && g_current_scene->get_state().player2->get_is_active()) {
        g_current_scene->get_state().player2->set_movement(glm::vec3(0.0f));
    }
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        // Quit the game with a keystroke
                        g_app_status = TERMINATED;
                        break;
                        
                    case SDLK_RETURN:
                        if (g_level_a != nullptr) {
                            delete g_level_a;
                        }
                        
                        g_level_a = new LevelA();
                        switch_to_scene(g_level_a);
                        
                        break;

                    case SDLK_1:
                        if (g_level_a != nullptr) {
                            delete g_level_a;
                        }
                        g_level_a = new LevelA();
                        switch_to_scene(g_level_a);
                        break;
                    
                    case SDLK_2:
                        if (g_level_b != nullptr) {
                            delete g_level_b;
                        }
                        g_level_b = new LevelB();
                        switch_to_scene(g_level_b);
                        break;
                        
                    case SDLK_3:
                        if (g_level_c != nullptr) {
                            delete g_level_c;
                        }
                        g_level_c = new LevelC();
                        switch_to_scene(g_level_c);
                        break;  
                        
                    case SDLK_w:
                        if (g_current_scene->get_state().player != nullptr) {
                            if (g_current_scene->get_state().player->get_collided_bottom())
                            {
                                g_current_scene->get_state().player->jump();
                                Mix_PlayChannel(-1,  g_current_scene->get_state().jump_sfx, 0);
                            }
                        }
                         break;
                    
                    case SDLK_UP:
                        if (g_current_scene->get_state().player2 != nullptr) {
                            if (g_current_scene->get_state().player2->get_collided_bottom())
                            {
                                g_current_scene->get_state().player2->jump();
                                Mix_PlayChannel(-1,  g_current_scene->get_state().jump_sfx, 0);
                            }
                        }
                         break;
                    
                    case SDLK_e:
                        if (g_current_scene->get_state().player != nullptr) {
                            g_current_scene->get_state().player->shoot_projectile();
                        }
                        break;
                        
                    case SDLK_SLASH:
                        if (g_current_scene->get_state().player2 != nullptr) {
                            g_current_scene->get_state().player2->shoot_projectile();
                        }
                        break;
                    
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    // ————— KEY HOLD ————— //
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (g_current_scene->get_state().player != nullptr) {
    // Player 1 controls (WASD)
        if (key_state[SDL_SCANCODE_A])
        {
            g_current_scene->get_state().player->move_left();
        }
        else if (key_state[SDL_SCANCODE_D])
        {
            g_current_scene->get_state().player->move_right();
        }
     
        if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
        {
            g_current_scene->get_state().player->normalise_movement();
        }
    }

    if (g_current_scene->get_state().player2 != nullptr) {
    // Player 2 controls (arrow keys)
        if (key_state[SDL_SCANCODE_LEFT])
        {
            g_current_scene->get_state().player2->move_left();
        }
        else if (key_state[SDL_SCANCODE_RIGHT])
        {
            g_current_scene->get_state().player2->move_right();
        }
     
        if (glm::length(g_current_scene->get_state().player2->get_movement()) > 1.0f)
        {
            g_current_scene->get_state().player2->normalise_movement();
        }
    }
}

void update()
{
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP);
        
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
   
    if ((g_current_scene->get_state().player != nullptr && g_current_scene->get_state().player->get_lives() <= 0) || 
        (g_current_scene->get_state().player2 != nullptr && g_current_scene->get_state().player2->get_lives() <= 0))
    {
        if (g_current_scene == g_level_a)
        {
            // Award points based on who survived
            if (g_current_scene->get_state().player != nullptr && g_current_scene->get_state().player->get_lives() <= 0) {
                // Player 1 died, award point to player 2
                player2_points++;
            } else if (g_current_scene->get_state().player2 != nullptr && g_current_scene->get_state().player2->get_lives() <= 0) {
                // Player 2 died, award point to player 1
                player1_points++;
            }
            
            if (g_level_b != nullptr) {
                delete g_level_b;
            }
            
            g_level_b = new LevelB();
            
            Mix_PlayChannel(-1, g_current_scene->get_state().next_level, 0);
            
            switch_to_scene(g_level_b);
            
        }
        else if (g_current_scene == g_level_b) {
            
 
            if (g_current_scene->get_state().player != nullptr && g_current_scene->get_state().player->get_lives() <= 0) {
                player2_points++;
            } else if (g_current_scene->get_state().player2 != nullptr && g_current_scene->get_state().player2->get_lives() <= 0) {
                player1_points++;
            }
            
            if (g_level_c != nullptr) {
                delete g_level_c;
            }
            
            g_level_c = new LevelC();
            
            Mix_PlayChannel(-1, g_current_scene->get_state().next_level, 0);
            
            switch_to_scene(g_level_c);
            
        }
        else if (g_current_scene == g_level_c){
                
            if (g_current_scene->get_state().player != nullptr && g_current_scene->get_state().player->get_lives() <= 0) {
                player2_points++;
            } else if (g_current_scene->get_state().player2 != nullptr && g_current_scene->get_state().player2->get_lives() <= 0) {
                player1_points++;
            }
            
            // PLayer 1 wins
            if (player1_points > player2_points) 
            {
                if (g_endGamePlayer1 != nullptr) { delete g_endGamePlayer1; }
                g_endGamePlayer1 = new endGameP1();
                switch_to_scene(g_endGamePlayer1);
            } else if (player2_points > player1_points) 
            {
                if (g_endGamePlayer2 != nullptr) { delete g_endGamePlayer2; }
                g_endGamePlayer2 = new endGameP2();
                switch_to_scene(g_endGamePlayer2);
            }
        }
    }
    
    // Check if any player has 1 health
    bool player_has_critical_health = (g_current_scene->get_state().player != nullptr && 
                         g_current_scene->get_state().player->get_lives() == 1) ||
                        (g_current_scene->get_state().player2 != nullptr && 
                         g_current_scene->get_state().player2->get_lives() == 1);
    

    // shader effect for critical health
    static bool previous_health_status = false;
    if (player_has_critical_health && !previous_health_status) {
        g_invert_effect_active = true;
        g_invert_effect_timer = 0.3f;
        g_shader_program.set_invert_colors(1);
    }
    if (g_invert_effect_active) {
        g_invert_effect_timer -= FIXED_TIMESTEP;
        
        if (g_invert_effect_timer <= 0.0f) {
            g_invert_effect_active = false;
            g_shader_program.set_invert_colors(0);
        }
    }
    previous_health_status = player_has_critical_health;
    
    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);
    
    // if (g_current_scene->get_state().player->get_position().x > LEVEL1_LEFT_EDGE) {
    //     g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, 3.75, 0));
    if (g_current_scene->get_state().player != nullptr && g_current_scene->get_state().player2 != nullptr) 
    {
        float player_mid_x = (g_current_scene->get_state().player->get_position().x + 
                            g_current_scene->get_state().player2->get_position().x) / 2.0f;
        
        if (player_mid_x > LEVEL1_LEFT_EDGE) {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-player_mid_x, 3.75, 0));
        } else {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
        }
    }
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program);

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{    
    SDL_Quit();
    
    // Only delete the scenes we explicitly created
    if (g_level_a != nullptr) delete g_level_a;
    if (g_level_b != nullptr) delete g_level_b;
    if (g_level_c != nullptr) delete g_level_c;
    if (g_menu != nullptr) delete g_menu;
    if (g_endGamePlayer1 != nullptr) delete g_endGamePlayer1;
    if (g_endGamePlayer2 != nullptr) delete g_endGamePlayer2;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
