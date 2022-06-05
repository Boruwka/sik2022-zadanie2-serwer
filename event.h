#include <object_definitions.h>

enum class EventType;

class Event
{
    public:

    Event();

    virtual size_t serialize(char data[]);

    EventType type;
};

class BombPlaced : public Event
{
    public:
    
    BombPlaced(BombId id, Position position);

    BombId id;
    Position position;
    
    size_t serialize(char data[]);
};

class BombExploded : public Event
{
    public BombExploded(BombId id, std::vector<Position> blocks_destroyed, std::vector<PlayerId> robots_destroyed);

    BombId id;
    std::vector<Position> blocks_destroyed; 
    std::vector<PlayerId> robots_destroyed;
    
    size_t serialize(char data[]);
};

class PlayerMoved : public Event
{
    public:
    
    PlayerMoved(PlayerId id, Position position);

    PlayerId id;
    Position position;
    
    size_t serialize(char data[]);
};

class BlockPlaced : public Event
{
    public:
    
    BlockPlaced(Position position);

    Position position;
    
    size_t serialize(char data[]);
};
