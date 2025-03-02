/**
* Author: Eric Zhao
* Assignment: Pong Clone
* Date due: 2025-3-01, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>

enum AppStatus { RUNNING, TERMINATED };

constexpr float WINDOW_SIZE_MULT = 2.0f;

constexpr int WINDOW_WIDTH  = 640 * WINDOW_SIZE_MULT,
              WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;

constexpr float BG_RED     = 0.9765625f,
                BG_GREEN   = 0.97265625f,
                BG_BLUE    = 0.9609375f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL    = 0;
constexpr GLint TEXTURE_BORDER     = 0;

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr char  BEAKER_SPRITE_FILEPATH[] = "racketLeft.png",
                DROPS_SPRITE_FILEPATH[]  = "racketRight.png",
                BALL_FILEPATH[]          = "tenisBall.png",
                COURT_FILEPATH[]         = "tenniscourt.jpeg",
                LEFT_WINS_FILEPATH[]     = "leftPlayerWins.png",
                RIGHT_WINS_FILEPATH[]    = "rightPlayerWins.png";

constexpr float MINIMUM_COLLISION_DISTANCE = 1.0f;
constexpr glm::vec3 INIT_SCALE_DROPS  = glm::vec3(1.5f, 1.5f, 0.0f),
                    INIT_POS_DROPS    = glm::vec3(-4.5f, 0.0f, 0.0f),
                    INIT_SCALE_BEAKER = glm::vec3(1.5f, 1.5f, 0.0f),
                    INIT_POS_BEAKER   = glm::vec3(4.5f, 0.0f, 0.0f),
                    INIT_POS_BALL   = glm::vec3(0.0f, 0.0f, 0.0f),
                    INIT_SCALE_BALL = glm::vec3(0.6f, 0.6f, 0.0f),
                    INIT_SCALE_COURT  = glm::vec3(10.0f, 7.5f, 1.0f),
                    INIT_POS_COURT    = glm::vec3(0.0f, 0.0f, 0.0f),
                    INIT_SCALE_WINNER_LEFT = glm::vec3(5.0f, 2.0f, 1.0f),
                    INIT_POS_WINNER_LEFT   = glm::vec3(0.0f, 0.0f, 0.0f),
                    INIT_SCALE_WINNER_RIGHT = glm::vec3(5.0f, 2.0f, 1.0f),
                    INIT_POS_WINNER_RIGHT = glm::vec3(0.0f, 0.0f, 0.0f);
                    

SDL_Window* g_display_window;

const float TOP_BOUND = 3.5f;
const float BOTTOM_BOUND = -3.5f;
const float LEFT_BOUND = -5.0f;
const float RIGHT_BOUND = 5.0f;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();
glm::mat4 g_view_matrix, g_beaker_matrix, g_projection_matrix, g_drops_matrix,  g_court_matrix, g_left_winner_matrix, g_right_winner_matrix;

glm::mat4 g_ball1_matrix = glm::mat4(1.0f);
glm::mat4 g_ball2_matrix = glm::mat4(1.0f);
glm::mat4 g_ball3_matrix = glm::mat4(1.0f);

float g_previous_ticks = 0.0f;

GLuint g_beaker_texture_id;
GLuint g_drops_texture_id;
GLuint g_ball_texture_id;
GLuint g_court_texture_id;
GLuint g_left_wins_texture_id;
GLuint g_right_wins_texture_id;

constexpr float DROP_SPEED = 2.0f;
constexpr float BEAKER_SPEED = 2.0f;
constexpr float BALL_SPEED = 1.0f;  // speed of drop fall
bool GAMESTATE = true;
bool singlePlayerMode = false;
float enemySpeed = BEAKER_SPEED * 1.5;
std::string WINNER;
int num_balls = 1;
bool ball_array[3] = {true, false, false};


glm::vec3 g_beaker_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_beaker_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_drops_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_drops_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_drops_scale = glm::vec3(0.0f, 0.0f, 0.0f);  // scale trigger vector
glm::vec3 g_drops_size  = glm::vec3(1.0f, 1.0f, 0.0f);  // scale accumulator vector

glm::vec3 g_ball1_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball2_position = glm::vec3(0.0f, 0.5f, 0.0f);
glm::vec3 g_ball3_position = glm::vec3(0.0f, -0.5f, 0.0f);

glm::vec3 g_ball1_movement = glm::vec3(1.0f, 1.0f, 0.0f);
glm::vec3 g_ball2_movement = glm::vec3(-1.0f, 1.0f, 0.0f); // diff directions
glm::vec3 g_ball3_movement = glm::vec3(1.0f, -1.0f, 0.0f);

void initialise();
void process_input();
void update();
void render();
void shutdown();

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("User-Input and Collisions Exercise",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);


    if (g_display_window == nullptr) shutdown();

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_beaker_matrix = glm::mat4(1.0f);
    
//    g_ball_matrix = glm::mat4(1.0f);
    g_ball1_matrix = glm::mat4(1.0f);
    g_ball2_matrix = glm::mat4(1.0f);
    g_ball3_matrix = glm::mat4(1.0f);
    
    g_drops_matrix = glm::mat4(1.0f);
    g_court_matrix = glm::mat4(1.0f);
    g_left_winner_matrix = glm::mat4(1.0f);
    g_right_winner_matrix = glm::mat4(1.0f);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_beaker_texture_id = load_texture(BEAKER_SPRITE_FILEPATH);
    g_drops_texture_id = load_texture(DROPS_SPRITE_FILEPATH);
    g_ball_texture_id = load_texture(BALL_FILEPATH);
    g_court_texture_id = load_texture(COURT_FILEPATH);
    g_left_wins_texture_id = load_texture(LEFT_WINS_FILEPATH);
    g_right_wins_texture_id = load_texture(RIGHT_WINS_FILEPATH);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_drops_movement.y = 0.0f;
    g_beaker_movement.y = 0.0f;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // End game
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_q:
                        g_app_status = TERMINATED;
                        break;
                    case SDLK_t:
                        singlePlayerMode = !singlePlayerMode;
                        break;
                    case SDLK_1:
                        num_balls = 1;
                        ball_array[0] = true;
                        ball_array[1] = false;
                        ball_array[2] = false;
                        break;
                        
                    case SDLK_2:
                        num_balls = 2;
                        ball_array[0] = true;
                        ball_array[1] = true;
                        ball_array[2] = false;
                        break;
                        
                    case SDLK_3:
                        num_balls = 3;
                        ball_array[0] = true;
                        ball_array[1] = true;
                        ball_array[2] = true;
                        break;
                    default: break;
                }

            default:
                break;
        }
    }


    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    if (singlePlayerMode == true) // During single player only allow the left side to play
    {
        if (key_state[SDL_SCANCODE_W] && g_drops_position.y < TOP_BOUND)
        {
            g_drops_movement.y = 1.0f;
        }
        else if (key_state[SDL_SCANCODE_S] && g_drops_position.y > BOTTOM_BOUND)
        {
            g_drops_movement.y = -1.0f;
        }
    
    } else { // During double player mode two players play
        if (key_state[SDL_SCANCODE_W] && g_drops_position.y < TOP_BOUND)
        {
            g_drops_movement.y = 1.0f;
        }
        else if (key_state[SDL_SCANCODE_S] && g_drops_position.y > BOTTOM_BOUND)
        {
            g_drops_movement.y = -1.0f;
        }
        
        if (key_state[SDL_SCANCODE_UP] && g_beaker_position.y < TOP_BOUND)
        {
            g_beaker_movement.y = 1.0f;
        }
        else if (key_state[SDL_SCANCODE_DOWN] && g_beaker_position.y > BOTTOM_BOUND)
        {
            g_beaker_movement.y = -1.0f;
        }
    }
    
    if (glm::length(g_drops_movement) > 1.0f)
    {
        // In this particular case, I'm not going to normalise because I am simulating
        // gravity
        // g_drops_movement = glm::normalize(g_drops_movement);
    }
}

void update()
{
    // --- DELTA TIME CALCULATIONS --- //
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    if (WINNER.empty() == true){
        // single player mode
        if (singlePlayerMode == true)
        {
            if (g_beaker_position.y >= TOP_BOUND)
            {
                enemySpeed = -BEAKER_SPEED;
            }
            else if (g_beaker_position.y <= BOTTOM_BOUND)
            {
                enemySpeed = BEAKER_SPEED;
            }
            
            g_beaker_position.y += enemySpeed * delta_time;
        }
        
        
        // --- ACCUMULATOR LOGIC --- //
        g_drops_position += g_drops_movement * DROP_SPEED * delta_time;
        g_beaker_position += g_beaker_movement * BEAKER_SPEED * delta_time;
        
        if (ball_array[0]) {
            g_ball1_position += g_ball1_movement * BALL_SPEED * delta_time;
        }
        
        if (ball_array[1]) {
            g_ball2_position += g_ball2_movement * BALL_SPEED * delta_time;
        }
        
        if (ball_array[2]) {
            g_ball3_position += g_ball3_movement * BALL_SPEED * delta_time;
        }
        
        
        // --- TRANSLATION --- //
        g_beaker_matrix = glm::mat4(1.0f);
        g_beaker_matrix = glm::translate(g_beaker_matrix, INIT_POS_BEAKER);
        g_beaker_matrix = glm::translate(g_beaker_matrix, g_beaker_position);
        g_beaker_matrix = glm::scale(g_beaker_matrix, INIT_SCALE_BEAKER);
        
        g_drops_matrix = glm::mat4(1.0f);
        g_drops_matrix = glm::translate(g_drops_matrix, INIT_POS_DROPS);
        g_drops_matrix = glm::translate(g_drops_matrix, g_drops_position);
        g_drops_matrix = glm::scale(g_drops_matrix, INIT_SCALE_DROPS);
        
        g_ball1_matrix = glm::mat4(1.0f);
        g_ball1_matrix = glm::translate(g_ball1_matrix, INIT_POS_BALL);
        g_ball1_matrix = glm::translate(g_ball1_matrix, g_ball1_position);
        g_ball1_matrix = glm::scale(g_ball1_matrix, INIT_SCALE_BALL);
        
        g_ball2_matrix = glm::mat4(1.0f);
        g_ball2_matrix = glm::translate(g_ball2_matrix, INIT_POS_BALL);
        g_ball2_matrix = glm::translate(g_ball2_matrix, g_ball2_position);
        g_ball2_matrix = glm::scale(g_ball2_matrix, INIT_SCALE_BALL);
        
        g_ball3_matrix = glm::mat4(1.0f);
        g_ball3_matrix = glm::translate(g_ball3_matrix, INIT_POS_BALL);
        g_ball3_matrix = glm::translate(g_ball3_matrix, g_ball3_position);
        g_ball3_matrix = glm::scale(g_ball3_matrix, INIT_SCALE_BALL);
        
        g_court_matrix = glm::mat4(1.0f);
        g_court_matrix = glm::translate(g_court_matrix, INIT_POS_COURT);
        g_court_matrix = glm::scale(g_court_matrix, INIT_SCALE_COURT);
        
        g_left_winner_matrix = glm::mat4(1.0f);
        g_left_winner_matrix = glm::translate(g_left_winner_matrix, INIT_POS_WINNER_LEFT);
        g_left_winner_matrix = glm::scale(g_left_winner_matrix, INIT_SCALE_WINNER_LEFT);
        
        g_right_winner_matrix = glm::mat4(1.0f);
        g_right_winner_matrix = glm::translate(g_right_winner_matrix, INIT_POS_WINNER_RIGHT);
        g_right_winner_matrix = glm::scale(g_right_winner_matrix, INIT_SCALE_WINNER_RIGHT);
        
        if (ball_array[0]) {
            if (g_ball1_position.y >= TOP_BOUND && g_ball1_movement.y > 0)
            {
                g_ball1_movement.y = -g_ball1_movement.y;
            }
            
            if (g_ball1_position.y <= BOTTOM_BOUND && g_ball1_movement.y < 0)
            {
                g_ball1_movement.y = -g_ball1_movement.y;
            }
            
            // collision on the left
            float x_distance_left = fabs(g_ball1_position.x - INIT_POS_DROPS.x) - ((INIT_SCALE_BALL.x + INIT_SCALE_DROPS.x) / 2.0f);
            float y_distance_left = fabs(g_ball1_position.y - g_drops_position.y) - ((INIT_SCALE_BALL.y + INIT_SCALE_DROPS.y) / 2.0f);
            
            if (x_distance_left < 0.0f && y_distance_left < 0.0f)
            {
                if (g_ball1_movement.x < 0)
                {
                    g_ball1_movement.x = -g_ball1_movement.x;
                }
            }
            
            // collision on the right
            float x_distance_right = fabs(g_ball1_position.x - INIT_POS_BEAKER.x) - ((INIT_SCALE_BALL.x + INIT_SCALE_BEAKER.x) / 2.0f);
            float y_distance_right = fabs(g_ball1_position.y - g_beaker_position.y) - ((INIT_SCALE_BALL.y + INIT_SCALE_BEAKER.y) / 2.0f);
            
            if (x_distance_right < 0.0f && y_distance_right < 0.0f)
            {
                if (g_ball1_movement.x > 0)
                {
                    g_ball1_movement.x = -g_ball1_movement.x;
                }
            }
            
            if (g_ball1_position.x < LEFT_BOUND) {
                WINNER = "right";
            } else if (g_ball1_position.x > RIGHT_BOUND) {
                WINNER = "left";
            }
        }
        

        if (ball_array[1]) {
            if (g_ball2_position.y >= TOP_BOUND && g_ball2_movement.y > 0)
            {
                g_ball2_movement.y = -g_ball2_movement.y;
            }
            
            if (g_ball2_position.y <= BOTTOM_BOUND && g_ball2_movement.y < 0)
            {
                g_ball2_movement.y = -g_ball2_movement.y;
            }
            
            // collision on the left
            float x_distance_left = fabs(g_ball2_position.x - INIT_POS_DROPS.x) - ((INIT_SCALE_BALL.x + INIT_SCALE_DROPS.x) / 2.0f);
            float y_distance_left = fabs(g_ball2_position.y - g_drops_position.y) - ((INIT_SCALE_BALL.y + INIT_SCALE_DROPS.y) / 2.0f);
            
            if (x_distance_left < 0.0f && y_distance_left < 0.0f)
            {
                if (g_ball2_movement.x < 0)
                {
                    g_ball2_movement.x = -g_ball2_movement.x;
                }
            }
            
            // collision on the right
            float x_distance_right = fabs(g_ball2_position.x - INIT_POS_BEAKER.x) - ((INIT_SCALE_BALL.x + INIT_SCALE_BEAKER.x) / 2.0f);
            float y_distance_right = fabs(g_ball2_position.y - g_beaker_position.y) - ((INIT_SCALE_BALL.y + INIT_SCALE_BEAKER.y) / 2.0f);
            
            if (x_distance_right < 0.0f && y_distance_right < 0.0f)
            {
                if (g_ball2_movement.x > 0)
                {
                    g_ball2_movement.x = -g_ball2_movement.x;
                }
            }
            
            if (g_ball2_position.x < LEFT_BOUND) {
                WINNER = "right";
            } else if (g_ball2_position.x > RIGHT_BOUND) {
                WINNER = "left";
            }
        }
        
        if (ball_array[2]) {
            if (g_ball3_position.y >= TOP_BOUND && g_ball3_movement.y > 0)
            {
                g_ball3_movement.y = -g_ball3_movement.y;
            }
            
            if (g_ball3_position.y <= BOTTOM_BOUND && g_ball3_movement.y < 0)
            {
                g_ball3_movement.y = -g_ball3_movement.y;
            }
            
            // collision on the left
            float x_distance_left = fabs(g_ball3_position.x - INIT_POS_DROPS.x) - ((INIT_SCALE_BALL.x + INIT_SCALE_DROPS.x) / 2.0f);
            float y_distance_left = fabs(g_ball3_position.y - g_drops_position.y) - ((INIT_SCALE_BALL.y + INIT_SCALE_DROPS.y) / 2.0f);
            
            if (x_distance_left < 0.0f && y_distance_left < 0.0f)
            {
                if (g_ball3_movement.x < 0)
                {
                    g_ball3_movement.x = -g_ball3_movement.x;
                }
            }
            
            // collision on the right
            float x_distance_right = fabs(g_ball3_position.x - INIT_POS_BEAKER.x) - ((INIT_SCALE_BALL.x + INIT_SCALE_BEAKER.x) / 2.0f);
            float y_distance_right = fabs(g_ball3_position.y - g_beaker_position.y) - ((INIT_SCALE_BALL.y + INIT_SCALE_BEAKER.y) / 2.0f);
            
            if (x_distance_right < 0.0f && y_distance_right < 0.0f)
            {
                if (g_ball3_movement.x > 0)
                {
                    g_ball3_movement.x = -g_ball3_movement.x;
                }
            }

            if (g_ball3_position.x < LEFT_BOUND) {
                WINNER = "right";
            } else if (g_ball3_position.x > RIGHT_BOUND) {
                WINNER = "left";
            }
        }
        
    }
}


void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    // Bind texture
    draw_object(g_court_matrix, g_court_texture_id);
    draw_object(g_drops_matrix, g_drops_texture_id);
    draw_object(g_beaker_matrix, g_beaker_texture_id);
//    draw_object(g_ball_matrix, g_ball_texture_id);
    
    if (ball_array[0])
    {
        draw_object(g_ball1_matrix, g_ball_texture_id);
    }
    
    if (ball_array[1])
    {
        draw_object(g_ball2_matrix, g_ball_texture_id);
    }
    
    if (ball_array[2])
    {
        draw_object(g_ball3_matrix, g_ball_texture_id);
    }

    
    if (WINNER == "left")
    {
            draw_object(g_left_winner_matrix, g_left_wins_texture_id);
    }
    else if (WINNER == "right")
    {
            draw_object(g_right_winner_matrix, g_right_wins_texture_id);
        }

    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }


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

