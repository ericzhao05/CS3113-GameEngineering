#include "Scene.h"

class endGameP1 : public Scene {
private:
    
public:
    ~endGameP1();
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    void shoot_projectile(Entity* shooter);
};
