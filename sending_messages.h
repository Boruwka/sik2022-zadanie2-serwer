#ifndef _SENDING_MESSAGES
#define _SENDING_MESSAGES

#include <boost/asio.hpp>

#include "object_definitions.h"

using boost::asio::ip::tcp;

class GameServer;

enum class ClientMessageType
{
    Join,
    PlaceBomb,
    PlaceBlock,
    Move,
    WrongMessage
};

class ClientMessage
{
    public:

    ClientMessageType type;
    std::string name; // tylko dla join
    Direction direction; // tylko dla move

    // konstruktor deserializujący
    ClientMessage(char data[]);
    ClientMessage();
};

void send_message_to_player_by_socket(tcp::socket socket, char data[], size_t length);

void push_array_to_players_deque();


/* Pojedyncze połączenie po TCP. */
void session(tcp::socket sock, std::shared_ptr<GameServer> game_server);

/* Funkcja obsługująca nasłuchiwanie TCP. */
void run_tcp_server(boost::asio::io_context& io_context, unsigned short port, std::shared_ptr<GameServer> game_server);

#endif
