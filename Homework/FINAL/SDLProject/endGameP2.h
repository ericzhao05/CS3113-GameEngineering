#include "Scene.h"

class endGameP2 : public Scene {
    
public:
    ~endGameP2();
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    void shoot_projectile(Entity* shooter);
};
