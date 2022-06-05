#include <player.h>

class Player
{
    PlayerId id;
    tcp::socket socket;
    std::deque<char> buffer;
    Score score;
    std::string name;
    
    void send_message(char data[])
    {
        send_message_to_player_by_socket(sock, data);
    }

    size_t serialize_player(char data[])
    {
    }

    size_t serialize_accepted_player(char data[])
    {
    }
}
