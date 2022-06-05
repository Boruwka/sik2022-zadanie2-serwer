#include <event.h>

enum class EventType
{
    BombPlaced,
    BombExploded,
    PlayerMoved, 
    BlockPlaced
};

class Event
{
    public:

    Event()
    {
    }

    virtual size_t serialize(char data[]);

    EventType type;
    /* Event:
[0] BombPlaced { id: BombId, position: Position },
[1] BombExploded { id: BombId, robots_destroyed: List<PlayerId>, blocks_destroyed: List<Position> },
[2] PlayerMoved { id: PlayerId, position: Position },
[3] BlockPlaced { position: Position }, */
};

class BombPlaced : public Event
{
    public BombPlaced(BombId id, Position position)
    {
        this->type = EventType::BombPlaced;
        this->id = id;
        this->position = position;
    }

    BombId id;
    Position position;
    
    size_t serialize(char data[])
    {
    }
};

class BombExploded : public Event
{
    public BombExploded(BombId id, std::vector<Position> blocks_destroyed, std::vector<PlayerId> robots_destroyed)
    {
        this->type = EventType::BombPlaced;
        this->id = id;
        this->block_destroyed = blocks_destroyed;
        this->robots_destroyed = robots_destroyed;
    }

    BombId id;
    std::vector<Position> blocks_destroyed; 
    std::vector<PlayerId> robots_destroyed;
    
    size_t serialize(char data[])
    {
    }
};

class PlayerMoved : public Event
{
    public PlayerMoved(PlayerId id, Position position)
    {
        this->type = EventType::PlayerMoved;
        this->id = id;
        this->position = position;
    }

    PlayerId id;
    Position position;
    
    size_t serialize(char data[])
    {
    }
};

class BlockPlaced : public Event
{
    public BlockPlaced(Position position)
    {
        this->type = EventType::BlockPlaced;
        this->position = position;
    }

    Position position;
    
    size_t serialize(char data[])
    {
    }
};
