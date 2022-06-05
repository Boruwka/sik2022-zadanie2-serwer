#include <boost/asio.hpp>
#include <deque>

using boost::asio::ip::tcp;

class Player
{
    public:

    PlayerId id;
    tcp::socket socket;
    std::deque<char> buffer;
    Score score;
    std::string name;
    Position position;
    
    void send_message(char data[]);

    size_t serialize_player(char data[]);

    size_t serialize_accepted_player(char data[]);
};
