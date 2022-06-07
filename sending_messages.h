#ifndef _SENDING_MESSAGES
#define _SENDING_MESSAGES

#include <boost/asio.hpp>

#include "object_definitions.h"
#include "serialization_deserialization.h"
#include "game_server.h"

using boost::asio::ip::tcp;



void push_array_to_players_deque();


/* Pojedyncze połączenie po TCP. */
void session(tcp::socket sock, std::shared_ptr<GameServer> game_server);

/* Funkcja obsługująca nasłuchiwanie TCP. */
void run_tcp_server(boost::asio::io_context& io_context, unsigned short port, std::shared_ptr<GameServer> game_server);

#endif
