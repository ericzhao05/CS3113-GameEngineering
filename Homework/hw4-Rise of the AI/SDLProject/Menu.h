#include "Scene.h"

class Menu : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    static const int ENEMY_COUNT = 1;
    
    // ————— DESTRUCTOR ————— //
    ~Menu();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
