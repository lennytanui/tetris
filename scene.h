#include "app.h"

class Scene{

public:
    Scene(){
        // Init();
    }

public:
    virtual void Start() = 0;
    virtual void Update(AppState *app_state, float dt) = 0;
    virtual void Draw(AppState *app_state) = 0;
};