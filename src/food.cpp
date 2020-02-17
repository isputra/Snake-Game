#include "food.h"
#include <future>

Food::Food(int grid_width, int grid_height) :
    engine(dev()),
    random_w(0, static_cast<int>(grid_width-1)),
    random_h(0, static_cast<int>(grid_height-1)),
    _position{-1,-1}
{
    std::cout << "Food::Food..." << std::endl;
    _id = _idCnt++;
}

Food::~Food(){
    is_active = false; // we need to signal the thread that the class has deen destroyed
    std::cout << "Food::~Food() called..." << std::endl;
    std::for_each(threads.begin(), threads.end(), [](std::thread &t) {
        std::cout << "Ending thread id=" << t.get_id() << std::endl;
        t.join();
    });
}

void Food::RunThread(std::unique_ptr<Snake> &snake){
    std::cout << "Food::RunThread..." << std::endl;
    threads.emplace_back(std::thread(&Food::RunFoodCycle, this, std::ref(snake)));
}

void Food::RunFoodCycle(std::unique_ptr<Snake> &snake){
    std::cout << "Food::RunFoodCycle thread id=" << std::this_thread::get_id() << std::endl;
    while (is_active)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if(CheckIfFoodIsEaten(snake) == true)
        {
            // Because of data race, we could call methods whose child class has been destructed.
            // Therefore here we need to check with is_active. 
            if(is_active) RewardSnake(snake);
        }
        if(is_active && EvaluateIfFoodShouldBeGenerated(snake) == true)
        {
            GenerateFood(snake);
        }
    }
}

bool Food::CheckIfFoodIsEaten(std::unique_ptr<Snake> &snake){
    int new_x = static_cast<int>(snake->head_x);
    int new_y = static_cast<int>(snake->head_y);
    // std::cout << "_position.x=" << _position.x << " new_x=" << new_x << std::endl;
    // std::cout << "_position.y=" << _position.y << " new_y=" << new_y << std::endl;
    if (_position.x == new_x && _position.y == new_y) {
        _position.x=-1; // remove food from screen
        _position.y=-1;
        is_eaten = true;
        return true;
    }
    return false;
}

void Food::GenerateFood(std::unique_ptr<Snake> &snake){
    std::cout << "Generate Food #"<< getID() <<" thread id=" << std::this_thread::get_id() << std::endl;
    int x, y;
    while (true) {
        x = random_w(engine);
        y = random_h(engine);
        if (!snake->SnakeCell(x, y)) {
            std::cout << "Food #"<< getID() <<" Generated at x=" << x << " y=" << y << " thread id=" << std::this_thread::get_id() << std::endl;
            _position.x = x;
            _position.y = y;
            is_eaten = false;
            if(_type == FoodType::food_normal) {
                std::lock_guard<std::mutex> lock(_mutex);
                next_cycle = _idCnt-1;
            } else {
                // set timer to remove food
                std::async(&Food::RemoveUntil, this);
            }
            return;
        }
    }
}

template <typename T>
bool Food::CheckSnakeCondition(T const &value_attribute, T const &value_min, T const &random_value) {
    if(value_attribute > value_min && random_value < value_attribute){
        if(first_food) {
            std::cout << "Food::CheckSnakeCondition first_food=" << first_food << std::endl;
            first_food = false;
            return true;
        }
        std::cout << "Food::CheckSnakeCondition is_eaten=" << is_eaten << std::endl;
        return is_eaten;
    }
    return false;
}

void Food::RemoveUntil(){
    std::cout << "Food::RemoveUntil #" << getID() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    if(is_eaten) return;
    
    _position.x=-1; // remove food from screen
    _position.y=-1;
    is_eaten = true;
    std::cout << "Food #" << getID() << " is removed.." << std::endl;
}

int Food::_idCnt = 0;
int Food::next_cycle = 0;
std::mutex Food::_mutex;
std::condition_variable Food::_condition;