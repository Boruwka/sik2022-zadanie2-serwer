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
                   
        };
    }
};


/* Pojedyncze połączenie po TCP. */
void session(boost::asio::ip::tcp::tcp::socket sock)
{
    try
    {
        sock.set_option(boost::asio::ip::tcp::tcp::no_delay(true));
        serialize_hello(data);
        boost::asio::write(sock, boost::asio::buffer(data, length));
        if (server_info.game_state == GameStateType::Lobby)
        {
            for (auto player: server_info.current_lobby.accepted_players)
            {
                player.serialize_accepted_player(data);
                boost::asio::write(sock, boost::asio::buffer(data, length));
            }
        }
        else
        {
            server_info.current_game.serialize_game_started(data);
            boost::asio::write(sock, boost::asio::buffer(data, length));
            for (int i = 0; i < turn_number; i++)
            {
                server_info.serialize_turn(data, i);
                boost::asio::write(sock, boost::asio::buffer(data, length));
                
            }
        }
        server_info.add_player(sock);
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

            server_info.react_to_client_message(data, length, sock);
            // ta funkcja wyżej wysyła też coś jemu jeśli chce
            //boost::asio::write(sock, boost::asio::buffer(data, length));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}

/* Funkcja obsługująca nasłuchiwanie TCP. */
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
