#include "player.h"

class Player
{
    public: 

    void send_message(char data[], size_t length)
    {
        send_message_to_player_by_socket(sock, data, length);
    }

    size_t serialize_player(char data[])
    {
        TODO
    }

    size_t serialize_accepted_player(char data[])
    {
        TODO
    }

    bool get_first_message_from_deque(ClientMessage& mess);
    {
        TODO  
    }
};
