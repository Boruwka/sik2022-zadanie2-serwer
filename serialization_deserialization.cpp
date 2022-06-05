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
void serialize_number(char data[], size_t number, size_t size)
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
