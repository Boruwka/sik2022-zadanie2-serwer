#include "player.h"

class Player
{
    public: 

    void send_message(char data[])
    {
        send_message_to_player_by_socket(sock, data);
    }

    size_t serialize_player(char data[])
    {
        TODO
    }

    size_t serialize_accepted_player(char data[])
    {
        TODO
    }
}
