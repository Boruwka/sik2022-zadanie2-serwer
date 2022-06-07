#include "game_server.h"
#include "sending_messages.h"

int main(int argc, char *argv[])
{
    CommandlineArguments arguments(argc, argv);
    uint16_t port = arguments.get_port();
    std::shared_ptr<GameServer> game_server = std::make_shared<GameServer>(arguments);
    std::thread 
            tcp_server_thread([port, game_server]() 
            {
                boost::asio::io_context io_context;
                run_tcp_server(io_context, port, game_server); 
            });
    tcp_server_thread.detach();
    game_server->run_game_server();
    return 0;
}
