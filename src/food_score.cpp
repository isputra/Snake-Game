#include "food_score.h"

FoodScore::FoodScore(int grid_width, int grid_height, std::unique_ptr<Snake> &snake) :
    Food(grid_width, grid_height)
{
    std::cout << "FoodScore::FoodScore..." << std::endl;
    _type = FoodType::food_score;
}

bool FoodScore::EvaluateIfFoodShouldBeGenerated(std::unique_ptr<Snake> &snake) {
    std::unique_lock<std::mutex> lck(_mutex);
    if(next_cycle <= 0) return false;
    // std::cout << "FoodScore::EvaluateIfFoodShouldBeGenerated next_cycle=" << next_cycle << std::endl;
    next_cycle = next_cycle - 1;
    lck.unlock();

    std::uniform_int_distribution<> distr(0, RANDOM_MAX);
    int score = snake->GetScore();
    int random = distr(engine);
    int score_min = SCORE_MIN;
    // std::cout << "FoodShrink::EvaluateIfFoodShouldBeGenerated random="<< std::to_string(random) << " score="<< std::to_string(score) << std::endl;
    return CheckSnakeCondition(score, score_min, random);
}

void FoodScore::RewardSnake(std::unique_ptr<Snake> &snake) {
    std::cout << "FoodScore::Rewardsnake->.." << std::endl;
    int score = snake->GetScore() / 10;
    snake->SetScore(snake->GetScore()+score);
    snake->GrowBody();
    std::cout << "snake->GetScore=" << snake->GetScore() << std::endl;
}

