#include "object_definitions.h"

class Position
{
    public:
    
    Position() {}

    Position(uint16_t x, uint16_t y)
    {
        this->x = x;
        this->y = y;
    }
};

class Bomb
{
    
    Bomb(BombId id, uint16_t bomb_timer, Position position)
    {
        this->id = id;
        this->time_to_explode = bomb_timer;
        this->position = position;
    }
};

class Square
{
    public:
    
    Square()
    {
        this->block = false;
    }
};
