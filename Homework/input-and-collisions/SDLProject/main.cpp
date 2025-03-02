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

constexpr char RACKET_LEFT_FILEPATH[] = "racketLeft.png",
               RACKET_RIGHT_FILEPATH[]  = "racketRight.png",
               BALL_FILEPATH[] = "tenisBall.png";

constexpr float MINIMUM_COLLISION_DISTANCE = 1.0f;

constexpr glm::vec3 INIT_SCALE_RACKET_LEFT = glm::vec3(1.0f, 1.0f, 0.0f),
                    INIT_SCALE_RACKET_RGIHT = glm::vec3(1.0f, 1.0f, 0.0f),
                    INIT_SCALE_BALL = glm::vec3(1.0f, 1.0f, 0.0f),
                    // initial position
                    INIT_POS_RACKET_LEFT  = glm::vec3(2.0f, 0.0f, 0.0f),
                    INIT_POS_RACKET_RIGHT  = glm::vec3(-2.0f, 0.0f, 0.0f),
                    INIT_POS_BALL = glm::vec3(0.0f, 0.0f, 0.0f);

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();
glm::mat4 g_view_matrix, g_projection_matrix, g_racket_left_matrix, g_racket_right_matrix, g_ball_matrix;

float g_previous_ticks = 0.0f;

GLuint g_racket_left_texture_id;
GLuint g_racket_right_texture_id;
GLuint g_ball_texture_id;

constexpr float DROPS_SPEED = 3.0f;  // speed of drop fall

glm::vec3 g_racket_left_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_racket_left_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_racket_right_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_racket_right_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_ball_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball_movement = glm::vec3(0.0f, 0.0f, 0.0f);

float g_ball_speed = 1.0f;// move 1 unit per second
float g_racket_left_speed = 2.0f;
float g_racket_right_speed = 2.0f;

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

    g_racket_left_matrix = glm::mat4(1.0f);
    g_racket_right_matrix = glm::mat4(1.0f);
    g_ball_matrix = glm::mat4(1.0f);
    g_racket_left_matrix = glm::translate(g_racket_left_matrix, glm::vec3(1.0f, 1.0f, 0.0f));
    g_racket_left_matrix += g_racket_left_movement;
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_racket_left_texture_id = load_texture(RACKET_LEFT_FILEPATH);
    g_racket_right_texture_id = load_texture(RACKET_RIGHT_FILEPATH);
    g_ball_texture_id = load_texture(BALL_FILEPATH);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    // Every frame, turn off the x-component of the movement trigger vector only
    // since that is the only component that relies on user-input
    g_racket_left_movement = glm::vec3(0.0f);

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
                    case SDLK_q: g_app_status = TERMINATED; break;

                    // If the player presses the lowercase L key, then we're going to
                    // activate the drops' movement trigger vector's y-component in the
                    // downwards (negative) direction
                    case SDLK_l:  g_racket_left_movement.x = 1.0f;

                    default: break;
                }

            default:
                break;
        }
    }


    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    // Left and right movement for the drops is user-input dependent
    if      (key_state[SDL_SCANCODE_A])
    {
        g_racket_left_movement.x = -1.0f;
    }
    else if (key_state[SDL_SCANCODE_D])
    {
        g_racket_left_movement.x = 1.0f;}

    if (glm::length(g_racket_left_movement) > 1.0f)
    {
        // In this particular case, I'm not going to normalise because I am simulating
        // gravity
        // g_drops_movement = glm::normalize(g_drops_movement);
    }
}

void update()
{
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
    float delta_time = ticks - g_previous_ticks; // the delta time is the difference from the last frame
    g_previous_ticks = ticks;

    // Add direction * units per second * elapsed time
    g_racket_left_position += g_racket_left_movement * g_racket_left_speed * delta_time;
    
    g_racket_left_matrix = glm::mat4(1.0f);
    g_racket_left_matrix = glm::translate(g_racket_left_matrix, INIT_POS_RACKET_LEFT);
    g_racket_left_matrix = glm::translate(g_racket_left_matrix, g_racket_left_position);
    
    g_racket_right_matrix = glm::mat4(1.0f);
    g_racket_right_matrix = glm::translate(g_racket_right_matrix, INIT_POS_RACKET_RIGHT);
    
    g_racket_left_matrix = glm::scale(g_racket_left_matrix, INIT_POS_RACKET_LEFT);
    g_racket_right_matrix  = glm::scale(g_racket_right_matrix, INIT_POS_RACKET_RIGHT);
    
    float x_distance = fabs(g_racket_left_position.x + INIT_POS_RACKET_LEFT.x - INIT_POS_RACKET_RIGHT.x) -
        ((INIT_POS_RACKET_RIGHT.x + INIT_POS_RACKET_LEFT.x) / 2.0f);

    float y_distance = fabs(g_racket_left_position.y + INIT_POS_RACKET_LEFT.y - INIT_POS_RACKET_RIGHT.y) -
        ((INIT_POS_RACKET_RIGHT.y + INIT_POS_RACKET_LEFT.y) / 2.0f);
    
    if (x_distance < 0.0f && y_distance < 0.0f)
    {
        std::cout << std::time(nullptr) << ": Collision.\n";
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
    draw_object(g_racket_left_matrix, g_racket_left_texture_id);
    draw_object(g_racket_right_matrix, g_racket_right_texture_id);
    draw_object(g_ball_matrix, g_ball_texture_id);

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

