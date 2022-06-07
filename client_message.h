#ifndef _CLIENT_MESSAGE
#define _CLIENT_MESSAGE

#include <boost/asio.hpp>
#include "serialization_deserialization.h"

using boost::asio::ip::tcp;

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

ClientMessage::ClientMessage(char data[])
{
    switch(data[0])
    {
        case 0:
            type = ClientMessageType::Join;
            size_t pos;
            name = deserialize_string(&(data[1]), pos);
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

ClientMessage::ClientMessage() {} // żeby się kompilator nie czepiał

#endif

