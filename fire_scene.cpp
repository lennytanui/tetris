#include "fire_scene.h"


FireScene::FireScene(){
    particlesManager.count = 500;
    particlesManager.life_time = 10.0f;
    printf("Hello WOlrd\n\n\n");
    for(int i = 0; i < particlesManager.count; i++){
        Particle *particle = &particlesManager.particles[i];
        particle->original.position = v2{400.0f, 200.0f};
        particle->original.velocity = {0.0f, 0.0f};
        
        float randx = RandomFloat(-1.0, 1.0) * 4.0f;
        float randy = RandomFloat(0.1, 1) * 10.0f;
        particle->original.acceleration.x = randx; // rand
        particle->original.acceleration.y = randy;

        particle->original.size = v2{10.0f, 10.0f};
        particle->original.color = RGBA{255.0f, 0.0f, 255.0f, 255.0f};
    }
}

void FireScene::Start() { 
    for(int i = 0; i < particlesManager.count; i++){
        Particle *particle = &particlesManager.particles[i];
        particle->current = particle->original;
    }
}

void FireScene::Update(AppState *app_state, float dt) {
    
    for(int i = 0; i < particlesManager.count; i++){
        Particle *particle = &particlesManager.particles[i];
        particle->current.velocity += particle->current.acceleration * dt;
        particle->current.position += particle->current.velocity * dt;
    }

    particlesManager.current_time -= dt;
    if(particlesManager.current_time <= 0){
        Start();
        particlesManager.current_time = particlesManager.life_time;
    } 
}

void FireScene::Draw(AppState *app_state){
    // render background
    create_render_square(app_state,
    {0.0f, 0.0f, 0.0f, 0.0f}, {1200.0f, 1200.0f}, 
    RGBA{0.0f, 0.0f, 0.0f, 255.0f}, RGBA{255.0f, 0.0f, 0.0f, 255.0f});

    // render particles
    for(int i = 0; i < particlesManager.count; i++){
        Particle *particle = &particlesManager.particles[i];
        
        v4 particlePos = {particle->current.position.x, particle->current.position.y, 0.0f, 1.0f};
        create_render_square(app_state, particlePos, 
            particle->current.size, particle->current.color, 
            particle->current.color);
    }
}