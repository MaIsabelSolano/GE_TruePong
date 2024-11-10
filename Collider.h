#pragma once
#pragma once

#include <SDL.h>
#include <iostream>
#include "Systems.h"
#include "Components.h"


enum class CollisionType {
	NONE,
	COLLISION
};


struct BoxColliderComponent {
	SDL_Rect rect;
	SDL_Color color;
	CollisionType collisionType = CollisionType::NONE;
	bool isTriggered = false;
};


class ColliderResetSystem : public UpdateSystem {
public:
	void run(float dT) override {
		auto view = scene->r.view<BoxColliderComponent>();

		for (auto entity : view) {
			auto& collider = view.get<BoxColliderComponent>(entity);

			collider.collisionType = CollisionType::NONE;
		}
	}
};

class ColliderRenderSystem : public RenderSystem {
public:
	void run(SDL_Renderer* renderer) override {
		auto view = scene->r.view<PositionComponent, BoxColliderComponent>();

		for (auto entity : view) {
			auto [position, collider] = view.get<PositionComponent, BoxColliderComponent>(entity);

			SDL_Rect renderRect = {
			  position.x + collider.rect.x ,
			  position.y + collider.rect.y,
			  collider.rect.w,
			  collider.rect.h,
			};

			SDL_SetRenderDrawColor(renderer, collider.color.r, collider.color.g, collider.color.b, collider.color.a);
			SDL_RenderDrawRect(renderer, &renderRect);
		}

	}
};

class CollisionDetectionSystem : public UpdateSystem {
public:
    void run(float dT) override {
        auto playerView = scene->r.view<PaddleComponent, BoxColliderComponent, PositionComponent>();
        auto ballView = scene->r.view<BallComponent, BoxColliderComponent, PositionComponent>();

        for (auto player : playerView) {
            auto position = playerView.get<PositionComponent>(player);
            auto& collider = playerView.get<BoxColliderComponent>(player);

            SDL_Rect playerRect = {
              position.x + collider.rect.x,
              position.y + collider.rect.y,
              collider.rect.w,
              collider.rect.h,
            };

            for (auto ball : ballView) {
                auto [pposition, pcollider] = ballView.get<PositionComponent, BoxColliderComponent>(ball);

                SDL_Rect ballRect = {
                  pposition.x + pcollider.rect.x,
                  pposition.y + pcollider.rect.y,
                  pcollider.rect.w,
                  pcollider.rect.h,
                };

                if (SDL_HasIntersection(&playerRect, &ballRect)) {
                    collider.collisionType = CollisionType::COLLISION;
                }
            }
        }
    }
};


class CollisionSystem : public UpdateSystem {
public:
    void run(float dT) override {
        auto playerView = scene->r.view<PaddleComponent, BoxColliderComponent, PositionComponent>();

        for (auto player : playerView) {
            auto& collider = playerView.get<BoxColliderComponent>(player);

            if (collider.collisionType == CollisionType::COLLISION && !collider.isTriggered) {
                std::cout << "paddle collider with ball" << std::endl;
                collider.isTriggered = true;
            }
        }
    }
};




