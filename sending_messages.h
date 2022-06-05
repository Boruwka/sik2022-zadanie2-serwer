using namespace boost::asio::ip::tcp;

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
    ClientMessage(char data[], endpoint client_endpoint);
};

void send_message_to_player_by_socket(tcp::socket socket, char data[], size_t length);

void push_array_to_players_deque();


/* Pojedyncze połączenie po TCP. */
void session(tcp::socket sock);

/* Funkcja obsługująca nasłuchiwanie TCP. */
void run_tcp_server(boost::asio::io_context& io_context,        
    std::string address,
    unsigned short port);
