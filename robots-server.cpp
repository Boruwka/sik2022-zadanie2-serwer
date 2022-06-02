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


enum class Direction
{
    Up,
    Right,
    Down,
    Left
};

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

class Position
{
    public:

    uint16_t x;
    uint16_t y;
};


class Player
{
    PlayerId id;
    Score score;
    std::string address;
    std::string name
    boost::asio::ip::tcp::tcp::socket* socket;
    
    size_t serialize_accepted_player(char data[])
    {
        data[0] = 1;
        size_t pos = 1;
        pos += serialize_number(&(data[pos]), this->id, sizeof(this->id));
        pos += serialize_string(&(data[pos]), this->name);
        pos += serialize_string(&(data[pos]), this->address);
        return pos;

        /* [1] AcceptedPlayer {
        id: PlayerId,
        player: Player,
    }, */
        
        /* PlayerId: u8
Position: { x: u16, y: u16 }
Player: { name: String, address: String } */
    }
};

class ServerInfo
{
    // stałe dla danego serwera, nie zmieniają się gdy się rozgrywka skończy 
    std::map<PlayerId, Player> players;
    Game current_game;
    Lobby current_lobby; // jedno z nich jest aktualne

    /*     -b, --bomb-timer <u16>
    -c, --players-count <u8>
    -d, --turn-duration <u64, milisekundy>
    -e, --explosion-radius <u16>
    -h, --help                                   Wypisuje jak używać programu
    -k, --initial-blocks <u16>
    -l, --game-length <u16>
    -n, --server-name <String>
    -p, --port <u16>
    -s, --seed <u32, parametr opcjonalny>
    -x, --size-x <u16>
    -y, --size-y <u16> */
    
    uint16_t bomb_timer;
    uint8_t players_count;
    uint64_t turn_duration;
    uint16_t explosion_radius;
    uint16_t initial_blocks;
    uint16_t game_length;
    std::string server_name;
    uint16_t port;
    uint32_t seed;
    uint16_t size_x;
    uint16_t size_y;

    size_t serialize_hello(char data[])
    {
        size_t pos;
        data[0] = 0;
        pos = 1;
        pos += serialize_string(&(data[pos]), this->server_name);
        pos += serialize_number(&(data[pos]), this->players_count, sizeof(this->players_count));    
        pos += serialize_number(&(data[pos]), this->size_x, sizeof(this->size_x));  
        pos += serialize_number(&(data[pos]), this->size_y, sizeof(this->size_y)); 
        pos += serialize_number(&(data[pos]), this->game_length, sizeof(this->game_length)); 
        pos += serialize_number(&(data[pos]), this->explosion_radius, sizeof(this->explosion_radius)); 
        pos += serialize_number(&(data[pos]), this->bomb_timer, sizeof(this->bomb_timer));  
        return pos;  
    }
};

ServerInfo server_info; // globalna zmienna z całym info

class Game
{
    Game()
    {
        turn_number = 0;
        for (auto player: accepted_players)
        {
            ustaw_pozycje_na_random(player);
            dodaj_player_move_do_eventow();
        }
        ustaw_initial_blocks();
        wyslij_komunikaty_turn();
    }

    std::vector<PlayerId> accepted_players;
    turn_number;
    std::vector<Event> events;
    // stan gry
    
};

class Lobby
{
    // stan lobby
};

enum class ClientMessageType
{
    Join,
    PlaceBomb,
    PlaceBlock,
    Move
    WrongMessage
};

class ClientMessage
{
    public:

    ClientMessageType type;
    std::string name; // tylko dla join
    Direction direction; // tylko dla move

    // konstruktor deserializujący
    ClientMessage(char data[], endpoint client_endpoint)
    {
        switch(data[0])
        {
            case 0:
                type = ClientMessageType::Join;
                name = deserialize_string(&(data[1]));
                break;
            case 1:
                type = ClientMessageType::PlaceBomb;
                break;
            case 2:
                type = ClientMessageType::PlaceBlock;
                break;
            case 3:
                type = ClientMessageType::Move;
                direction = deserialize_direction(&(data[1]));
                break;
            default:
                type = ClientMessageType::WrongMessage;
                disconnect_client(client_endpoint);
                
                
        };
    }
    
    void process_message()
    {
        // dodanie ruchów do zdarzeń, dodanie join, itp
    }
};

/* Pojedyncze połączenie po TCP. */
void session(boost::asio::ip::tcp::tcp::socket sock)
{
    try
    {
        sock.set_option(boost::asio::ip::tcp::tcp::no_delay(true));
        for (;;)
        {
            char data[MAX_LENGTH];

            boost::system::error_code error;
            size_t length = 
                sock.read_some(boost::asio::buffer(data, MAX_LENGTH), error);
            if (error == boost::asio::error::eof)
                break; // połączenie zamknięte
            else if (error)
                throw boost::system::system_error(error);

            handle_message_from_server(data, length);
            boost::asio::write(sock, boost::asio::buffer(data, length));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}

void start_game()
{
    server_info.game = new Game();
}

/* Funkcja obsługująca połączenie z serwerem. */
void tcp_server(boost::asio::io_context& io_context, std::string address,
    unsigned short port)
{
    boost::asio::ip::tcp::tcp::resolver resolver(io_context);
    boost::asio::ip::tcp::tcp::endpoint endpoint;
    try 
    {
        endpoint = *(resolver.resolve(address, std::to_string(port)));
    }
    catch (std::exception& e)
    {
        
        std::cerr << "Resolving exception: wrong server address: " << e.what() << "\n";
        exit(1);
    }

    boost::asio::ip::tcp::tcp::acceptor 
        a(io_context, boost::asio::ip::tcp::tcp::endpoint(endpoint));

    for (;;)
    {
        std::thread(session, a.accept()).detach();
    }
}

int main(int argc, char *argv[])
{
    boost::program_options::options_description description("App usage");
    boost::program_options::variables_map parameter_map;
     
    description.add_options()
        ("help,h", "Get help")
        ("bomb-timer,b", 
            boost::program_options::value<uint16_t>(), "Bomb timer")
        ("players-count,c", 
            boost::program_options::value<uint16_t>(), "Players count")
        ("turn-duration,d", 
            boost::program_options::value<uint64_t>(), "Turn duration in miliseconds")
        ("explosion-radius,e", 
            boost::program_options::value<uint16_t>(), "Explosion radius")
        ("initial-blocks,k", 
            boost::program_options::value<uint16_t>(), "Initial blocks")
        ("game-length,k", 
            boost::program_options::value<uint16_t>(), "Game length")
        ("server-name,n", 
            boost::program_options::value<std::string>(), "Server name")
        ("port,p", 
            boost::program_options::value<uint16_t>(), "Port")
        ("seed,s", 
            boost::program_options::value<uint32_t>(), "Seed")
        ("size-x,x", 
            boost::program_options::value<uint16_t>(), "Size x")
        ("size-y,y", 
            boost::program_options::value<uint16_t>(), "Size y");

        boost::program_options::store(
            boost::program_options::command_line_parser(argc, argv).options(description).run(), 
            parameter_map);

    return 0;
}
