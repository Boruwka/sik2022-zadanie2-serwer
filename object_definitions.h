#ifndef _OBJECT_DEFINITIONS
#define _OBJECT_DEFINITIONS

#include <cstdint>
#include <set>

const uint16_t BUFFER_LEN = 65535;
const size_t len_of_sizetype = 4; // długość rozmiaru listy/mapy podczas serializacji

typedef uint32_t BombId;
typedef uint8_t PlayerId;
typedef uint32_t Score;

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

    Position(uint16_t x, uint16_t);
    Position() {}

    Position(const Position& other)
    {
        x = other.x;
        y = other.y;
        // konstruktor kopiujący
    }

    auto operator<=>(const Position& other) const 
    {
        return this->x <=> other.x;
        // żeby się kompilator odczepił jak robię seta
    }
};

class Bomb
{
    public:

    BombId id;
    uint16_t timer;
    Position position;
    
    Bomb(BombId id, uint16_t bomb_timer, Position position);

    Bomb() {}
};

class Square
{
    public:
    
    std::set<BombId> bombs;
    std::set<PlayerId> players;
    bool block;
};


#endif
