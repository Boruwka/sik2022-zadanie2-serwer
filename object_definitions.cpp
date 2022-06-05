enum class Direction
{
    Up,
    Right,
    Down,
    Left
};

class Position
{
    public:

    uint16_t x;
    uint16_t y;
};

class Bomb
{
    public:

    BombId id;
    uint16_t time_to_explode;
    Position position;
    
    Bomb(BombId id, uint16_t bomb_timer, Position position)
    {
        this->id = id;
        this->time_to_explode = bomb_timer;
        this->position = position;
    }
};