#ifndef FOOD_SLOW_H
#define FOOD_SLOW_H

#include "food.h"
#include "snake.h"

class FoodSlow : public Food 
{
public:
    FoodSlow(int grid_width, int grid_height, std::unique_ptr<Snake> &snake);
    ~FoodSlow(){std::cout << "FoodSlow::~FoodSlow() called..." << std::endl;}
    bool EvaluateIfFoodShouldBeGenerated(std::unique_ptr<Snake> &snake) override;
    void RewardSnake(std::unique_ptr<Snake> &snake) override;
};

#endif