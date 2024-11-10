#pragma once
#include "Game.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Sprites.h"
#include "Systems.h"
#include <print>
#include <entt/entt.hpp>
#include <vector>
#include <string>
#include "Background.h"
// #include "Tilemap.h"
#include <random>
#include "Collider.h"


class EntitiesSpawnSetupSystem : public SetupSystem {
    void run() {
        Entity* player = scene->createEntity("PLAYER", 150, 400);
        player->addComponent<PaddleComponent>(20, 200, SDL_Color{ 255, 0, 0 });
        player->addComponent<SpriteComponentSimple>(20, 200, SDL_Color{ 255, 0, 0 });
        player->addComponent<VelocityComponent>(0, 50);
        player->addComponent<PlayerComponent>();
        player->addComponent<BoxColliderComponent>(SDL_Rect{ 0, 0, 20, 200 }, SDL_Color{ 255, 255, 255 });

        Entity* ia = scene->createEntity("IA", 900, 400);
        ia->addComponent<PaddleComponent>(20, 200, SDL_Color{ 0, 255, 0 });
        ia->addComponent<SpriteComponentSimple>(20, 200, SDL_Color{ 0, 255, 0 });
        ia->addComponent<VelocityComponent>(0, 50);
        ia->addComponent<PC>();
        ia->addComponent<BoxColliderComponent>(SDL_Rect{ 0, 0, 20, 200 }, SDL_Color{ 255, 255, 255 });
        
        Entity* ball = scene->createEntity("BALL", 50, 50);
        ball->addComponent<BallComponent>(30, 30, SDL_Color{ 0, 0, 255 });
        ball->addComponent<SpriteComponentSimple>(30, 30, SDL_Color{ 0, 0, 255 });
        ball->addComponent<VelocityComponent>(150, 150);
        ball->addComponent<BoxColliderComponent>(SDL_Rect{ 0, 0, 30, 30 }, SDL_Color{ 255, 255, 255 });

    }
};

class PaddleMovementSystem : public UpdateSystem {
    void run(float dT) {

        auto view = scene->r.view<PositionComponent, SpriteComponent>();
        auto paddleView = scene->r.view<PaddleComponent, PositionComponent, VelocityComponent, SpriteComponentSimple, PlayerComponent>();

        const Uint8* ks = SDL_GetKeyboardState(NULL);

        for (auto paddle : paddleView) {
            auto& padleVelocity = paddleView.get<VelocityComponent>(paddle);

            padleVelocity.x = 0;

            if (ks[SDL_SCANCODE_A]) {
                padleVelocity.y = -500;
            }
            else if (ks[SDL_SCANCODE_D]) {
                padleVelocity.y = 500;
            }
            else {
                padleVelocity.y = 0;
            }
        }
    }
};

class MovementSystem : public UpdateSystem {
    void run(float dT) {
        auto view = scene->r.view<PositionComponent, VelocityComponent>();

        for (auto e : view) {
            auto& pos = view.get<PositionComponent>(e);
            auto vel = view.get<VelocityComponent>(e);

            pos.x += vel.x * dT;
            pos.y += vel.y * dT;

            if (pos.y >= 768) {
                auto gameStateView = scene->r.view<GameStateComponent>();

                for (auto entity : gameStateView) {
                    gameStateView.get<GameStateComponent>(entity).gameOver = true;

                }
            }
        }
    }
};


class WallHitSystem : public UpdateSystem {
    void run(float dT) {
        auto view = scene->r.view<PositionComponent, VelocityComponent, SpriteComponentSimple>();

        for (auto e : view) {
            auto pos = view.get<PositionComponent>(e);
            auto spr = view.get<SpriteComponentSimple>(e);
            auto& vel = view.get<VelocityComponent>(e);

            float newPosX = pos.x + vel.x * dT;
            float newPosY = pos.y + vel.y * dT;

            if (newPosX < 0 || newPosX + spr.width > 1024) {
                vel.x *= -1.05;


            }

            if (newPosY < 0 || newPosY + spr.height > 768) {
                vel.y *= -1.05;
            }

        }
    }
};


class GameStateSystem : public SetupSystem {
public:
    void run() override {
        Entity* ponts = scene->createEntity("POINTS");
        std::printf("Game State System\n");
    }
};

class SquareRenderSystem : public RenderSystem {
    void run(SDL_Renderer* renderer) {
        auto view = scene->r.view<PositionComponent, SpriteComponentSimple>();
        for (auto e : view) {
            auto pos = view.get<PositionComponent>(e);
            auto spr = view.get<SpriteComponentSimple>(e);

            SDL_SetRenderDrawColor(renderer, spr.color.r, spr.color.g, spr.color.b, spr.color.a);
            SDL_Rect r = { pos.x, pos.y, spr.width, spr.height };
            SDL_RenderFillRect(renderer, &r);
        }
    }
};



class DemoGame : public Game {
public:
    Scene* sampleScene;
    entt::registry r;

public:
    DemoGame()
        : Game("SAMPLE", 1024, 768)
    { }

    void setup() {
        std::printf("HELLO WORLD\n");
        sampleScene = new Scene("SAMPLE SCENE", r, renderer);

        ///* --- SETUP SYSTEMS --- */
        //// fondos y sprites
        //addSetupSystem<BackgroundSetupSystem>(sampleScene);
        addSetupSystem<EntitiesSpawnSetupSystem>(sampleScene);
        //addSetupSystem<TilemapSetupSystem>(sampleScene);
        addSetupSystem<TextureSetupSystem>(sampleScene);
        //addSetupSystem<TilemapColliderSetupSystem>(sampleScene);
        //addSetupSystem<GameStateSystem>(sampleScene);

        ///* --- UPDATE SYSTEMS --- */
        addUpdateSystem<MovementSystem>(sampleScene);
        addUpdateSystem<PaddleMovementSystem>(sampleScene);
        addUpdateSystem<WallHitSystem>(sampleScene);
        addUpdateSystem<CollisionDetectionSystem>(sampleScene);
        addUpdateSystem<CollisionSystem>(sampleScene);
        //// addUpdateSystem<CollisionSystem>(sampleScene);
        //addUpdateSystem<SpriteAnimationSystem>(sampleScene);
        //addUpdateSystem<SpriteMovementSystem>(sampleScene);
        //addUpdateSystem<PlayerTileCollisionDetectionSystem>(sampleScene);
        addUpdateSystem<SpriteAnimationSystem>(sampleScene);


        ///* --- RENDER SYSTEMS --- */
        addRenderSystem<SquareRenderSystem>(sampleScene);
        addRenderSystem<SpriteRenderSystem>(sampleScene);
        //// addRenderSystem<SquareRenderSystem>(sampleScene);
        //addRenderSystem<BackgroundRenderSystem>(sampleScene);
        //addRenderSystem<TilemapRenderSystem>(sampleScene);
        //addRenderSystem<SpriteRenderSystem>(sampleScene);
        //// addUpdateSystem<GameStateSystem>(sampleScene);
        addRenderSystem<ColliderRenderSystem>(sampleScene);

        setScene(sampleScene);
    }
};