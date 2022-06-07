#ifndef _PLAYER
#define _PLAYER

#include <boost/asio.hpp>
#include <deque>
#include "client_message.h"
#include <memory>

using boost::asio::ip::tcp;

void send_message_to_player_by_socket(tcp::socket socket, char data[], size_t length)
{
    boost::asio::write(socket, boost::asio::buffer(data, length));
}


class Player
{
    public:

    PlayerId id;
    std::shared_ptr<tcp::socket> socket;
    std::deque<char> buffer;
    Score score;
    std::string name;
    Position position;

    
    void send_message(char data[], size_t length);

    size_t serialize_player(char data[]);

    size_t serialize_accepted_player(char data[]);

    bool get_first_message_from_deque(ClientMessage& mess);

    Player() 
    {
        
    } // ten konstruktor nie powinien być nigdy użyty
};

#endif
