#include "scene.h"

class FireScene: public Scene{

private:
    ParticleManager particlesManager = {};

public:
    FireScene();

public:
    void Start() override;
    void Reset();
    void Update(AppState *app_state, float dt) override;
    void Draw(AppState *app_state) override;
};
