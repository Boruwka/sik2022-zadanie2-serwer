#include "object_definitions.h"



class Bomb
{
    
    Bomb(BombId id, uint16_t bomb_timer, Position position)
    {
        this->id = id;
        this->time_to_explode = bomb_timer;
        this->position = position;
    }
};
