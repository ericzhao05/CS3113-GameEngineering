#include "Scene.h"
#include <vector>

class LevelC : public Scene {
private:
   
    float m_spawn_timer = 0.0f;
    float m_spawn_interval = 1.0f;
    float m_total_game_time = 0.0f;
    int m_max_enemies = 20; 
    bool m_danger_music_playing = false;
    std::vector<GLuint> m_enemy_texture_ids; 

public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 1;
    
    // ————— DESTRUCTOR ————— //
    ~LevelC();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    

    void shoot_projectile(Entity* shooter);
    void update_health_bars(const float& delta_time);
    void update_enemy_health_bars(const float& delta_time);
    void spawn_enemy_above_player(Entity* target_player);
};
