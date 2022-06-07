#include "sending_messages.h"


void push_array_to_players_deque(char data[], size_t length, PlayerId id, std::shared_ptr<GameServer> game_server)
{
    for (size_t i = 0; i < length; i++)
    {
        game_server->players[id].buffer.push(data[i]);
    }
}

/* Pojedyncze połączenie po TCP. */
void session(tcp::socket sock, std::shared_ptr<GameServer> game_server)
{
    try
    {
        sock.set_option(tcp::no_delay(true));
        game_server->mutex.lock();
        size_t length = game_server.serialize_hello(data);
        game_server->mutex.unlock();
        send_message_to_player_by_socket(socket, data, length);        
        ClientMessage join_message = wait_for_join(socket, data, length);
        game_server->mutex.lock();

        if (game_server->game_state == GameStateType::Lobby)
        {
            PlayerId id = game_server->add_player(join_message, socket); 
                    game_server->players[id].serialize_accepted_player(data);
            for (auto player: game_server->players)
            {
                // wysyłamy innym info o tym
                player.send_message(data);
            }
            for (auto player: game_server->players)
            {
                // wysyłamy temu info o innych
                player.serialize_accepted_player(data);
                game_server->players[id].send_message(data);
            }
        }
        else
        {
            /* [2] GameStarted {
            players: Map<PlayerId, Player>, */
            PlayerId id = game_server.add_player(join_message, socket); // dodajemy go jako obserwatora, na razie to oleję
            size_t length = game_server.serialize_game_started(data);
            send_message_to_player_by_socket(socket, data, length); // wysyłamy mu game started
            
            length = game_server.serialize_turns(data, 0, game_server.turn_number);
            send_message_to_player_by_socket(socket, data, length); // wysyłamy mu get started
            
        }
        game_server.mutex.unlock();


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

            push_array_to_players_deque(data, length);
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
void run_tcp_server(boost::asio::io_context& io_context, unsigned short port)
{
    tcp::resolver resolver(io_context);
    tcp::endpoint endpoint;
    try 
    {
        endpoint = *(resolver.resolve(address, std::to_string(port)));
    }
    catch (std::exception& e)
    {
        
        std::cerr << "Resolving exception: wrong server address: " << e.what() << "\n";
        exit(1);
    }

    tcp::acceptor a(io_context, tcp::endpoint(endpoint));

    for (;;)
    {
        std::thread(session, a.accept()).detach();
    }
}
