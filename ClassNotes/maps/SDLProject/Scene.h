#define GL_SILENCE_DEPRECATION


#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include <vector>
#include "Entity.h"
#include "Map.h"

struct GameState
{
    Entity *player;
    Entity *enemies;
    
    Map *map;
    
    Mix_Music *bgm;
    Mix_Chunk *jump_sfx;
    
    int next_scene_id;
};

class Scene {
public:
    static int const DEFAULT_ENEMY_COUNT = 3;
    
    GameState m_state;
    
    virtual void initialize() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *shader_program) = 0;
    
    GamaState const get_state() const {return m_state}
};

