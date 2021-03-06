#include "serialization_deserialization.h"
#include "event.h"

/* Deserializuje liczbę o rozmiarze size. */
size_t deserialize_number(char data[], size_t size)
{
    size_t res = 0;
    for (size_t i = 0; i < size; i++)
    {
        res *= BYTE_SIZE;
        res += data[size - i - 1];       
    }
    return res;
}

/* Serializuje numer o rozmiarze size do tablicy data. */
size_t serialize_number(char data[], size_t number, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        data[i] = 0;
    }

    for (size_t i = 0; i < size; i++)
    {
        data[size - i - 1] = number % BYTE_SIZE;
        number /= BYTE_SIZE;       
    }

    return size;
}

/* Serializuje string s do tablicy data.
Zwracana wartość to długość całego zapisu stringa. */
size_t serialize_string(char data[], std::string s)
{
    size_t len = s.length();
    serialize_number(data, len, LEN_OF_SIZETYPE);
    size_t pos = LEN_OF_SIZETYPE;
    for (int i = 0; i < len; i++)
    {
        data[pos + i] = s[i];
        
    }
    return LEN_OF_SIZETYPE + len;
}

/* Deserializuje string do tablicy data,
w zmiennej pos zapisuje pozycję po nim. */
std::string deserialize_string(char data[], size_t& pos)
{
    size_t len = deserialize_number(data, LEN_OF_SIZETYPE);
    std::string s;
    for (int i = 0; i < len; i++)
    {
        s = s + char(data[i + LEN_OF_SIZETYPE]);
    }
    pos = LEN_OF_SIZETYPE + len;
    return s;
}


/* Serializuje kierunek. Zwraca długość zapisu. */
size_t serialize_direction(Direction direction, char data[])
{
    switch(direction)
    {
        case Direction::Up:
        {
            data[0] = 0;
            break;
        }
        case Direction::Right:
        {
            data[0] = 1;
            break;
        }
        case Direction::Down:
        {
            data[0] = 2;
            break;
        }
        case Direction::Left:
        {
            data[0] = 3;
            break;
        }
        default:
            exit(1);
    }
    return 1;
}


/* Deserializuje kierunek. */
Direction deserialize_direction(char data[])
{
    Direction d;
    switch(data[0])
    {
        case 0:
        {
            d = Direction::Up;
            break;
        }
        case 1:
        {
            d = Direction::Right;
            break;
        }
        case 2:
        {
            d = Direction::Down;
            break;
        }
        case 3:
        {
            d = Direction::Left;
            break;
        }
        default:
            exit(1);
    }
    return d;
    
}

size_t serialize_bomb_placed(BombPlaced e, char data[])
{
    /* [0] BombPlaced { id: BombId, position: Position }, */
    size_t pos = 1;
    data[0] = 0;
    pos += serialize_number(&(data[pos]), e.id, sizeof(e.id));
    pos += serialize_position(&(data[pos]), e.position);
    return pos;
}

size_t serialize_bomb_exploded(BombExploded e, char data[])
{
    size_t pos = 1;
    data[0] = 1;
    pos += serialize_number(&(data[pos]), e.id, sizeof(e.id));
    pos += serialize_number(&(data[pos]), e.robots_destroyed.size(), len_of_sizetype);
    for (auto& player_id: e.robots_destroyed)
    {
        pos += serialize_number(&(data[pos]), player_id, sizeof(player_id));
    }

    pos += serialize_number(&(data[pos]), e.blocks_destroyed.size(), len_of_sizetype);
    for (auto& block_position: e.blocks_destroyed)
    {
        pos += serialize_position(&(data[pos]), player_id);
    }
    return pos;
    /* [1] BombExploded { id: BombId, robots_destroyed: List<PlayerId>, blocks_destroyed: List<Position> }, */
     
}


size_t serialize_event(char data[], Event e)
{
    size_t pos = 1;

    switch(e.type)
    {
        case (EventType::BombPlaced):
            pos += serialize_bomb_placed(e, data);   
            break;
        case (EventType::BombExploded):
            pos += serialize_bomb_exploded(e, data);
            break;
        case (EventType::PlayerMoved):
            pos += serialize_player_moved(e, data);
            break;
        case (EventType::BlockPlaced):
            pos += serialize_block_placed(e, data);
            break;
        default:
            break;
    };

    return pos;
    /* Event:
[0] BombPlaced { id: BombId, position: Position },
[1] BombExploded { id: BombId, robots_destroyed: List<PlayerId>, blocks_destroyed: List<Position> },
[2] PlayerMoved { id: PlayerId, position: Position },
[3] BlockPlaced { position: Position }, */
}


size_t serialize_game_ended(char data[], std::map<PlayerId, Score> scores)
{
    TODO
}
