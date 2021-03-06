#include "game.h"
#include <iostream>
#include <future>
#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height) :
      grid_width(grid_width),
      grid_height(grid_height)
{}

void Game::Init(){
  std::cout << "Game::Init..." << std::endl;
  snake = std::make_unique<Snake>(grid_width, grid_width);
  food_normal = std::make_unique<FoodNormal>(grid_width, grid_width, snake);
  food_normal->RunThread(snake);
  food_score = std::make_unique<FoodScore>(grid_width, grid_width, snake);
  food_score->RunThread(snake);
  food_slow = std::make_unique<FoodSlow>(grid_width, grid_width, snake);
  food_slow->RunThread(snake);
  food_shrink = std::make_unique<FoodShrink>(grid_width, grid_width, snake);
  food_shrink->RunThread(snake);
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    Update();
    renderer.Render(snake, food_normal, food_score, food_slow, food_shrink);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(snake->GetScore(), frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::Update() {
  if (!snake->alive) return;

  snake->Update();
}

int Game::GetScore() const { return snake->GetScore(); }
int Game::GetSize() const { return snake->size; }