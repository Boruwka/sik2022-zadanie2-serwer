#include "commandline_arguments.h"
#include "event.h"
#include "object_definitions.h"
#include "player.h"
#include "random_generator.h"
#include "serialization_deserialization.h"
#include "sending_messages.h"

class GameInfo
{
    public:

    uint16_t turn_number;
    std::vector<Position> blocks;
    std::vector<Bomb> bombs;
    std::vector<std::vector<Event>> events;
    
    GameInfo()
    {
        // default konstruktor żeby się kompilator nie czepiał
    }

    GameInfo(uint16_t game_length)
    {        
        events = std::vector<std::vector<Event>>(game_length);
        turn_number = 0;
    }
};



class GameServer // odpowiednik server_info
{
    public:

    GameServer(CommandlineArguments args)
    {
        mutex.lock();
        bomb_timer = args.get_bomb_timer();
        players_count_for_game = args.get_players_count();
        turn_duration = args.get_turn_duration();
        explosion_radius = args.get_explosion_radius();
        initial_blocks = args.get_initial_blocks();
        game_length = args.get_game_length();
        server_name = args.get_server_name();
        port = args.get_port();
        seed = args.get_seed();
        size_x = args.get_size_x();
        size_y = args.get_size_y();
        random_generator = RandomGenerator(seed);
        players_count = 0;
    }

    std::mutex mutex; // biorą go pozostałe wątki, gdy chcą tu coś zmienić, ta go oddaje tylko jak zasypia
    uint16_t bomb_timer;
    uint8_t players_count_for_game;
    uint8_t players_count;
    uint64_t turn_duration;
    uint16_t explosion_radius;
    uint16_t initial_blocks;
    uint16_t game_length;
    std::string server_name;
    uint16_t port;
    uint32_t seed;
    uint16_t size_x;
    uint16_t size_y;
    GameInfo current_game_info;
    RandomGenerator random_generator;
    std::map<PlayerId, Player> players; // na razie olejmy możliwość odłączania się graczy, bo gdyby mogli to mogliby wciąż być w grze, a tu nie, ale niech to będzie główny zbiór graczy


    size_t serialize_turns(char data[], uint16_t turn_nr, uint16_t begin, uint16_t end)
    {
        // serializuje tury od tury nr begin do tury nr end (wyłącznie po prawej)
        // bo czasami możemy chcieć zserializować ich więcej niż jedną
        // a turn_nr to jako jaki nr mamy wysłać (bo możemy chcieć np jako 0)
        size_t pos = serialize_number(data, turn_nr, sizeof(turn_nr));

        uint16_t number_of_events = 0; // sum of numbers of events in serialized turns
        for (int i = begin; i < end; i++)
        {
            number_of_events += current_game_info.events[i].size();
        }

        pos += serialize_number(&(data[pos]), number_of_events, len_of_sizetype);

        for (int i = begin; i < end; i++)
        {
            for (auto event: current_game_info.events[i])
            {
                pos += serialize_event(&(data[pos]), event);
            }
        }
        
    }

    

    size_t serialize_turn(char data[], uint16_t turn_nr)
    {
        // musi przyjmować turn nr bo możemy chcieć przesłać inną niż aktualna turę
        return serialize_turns(data, turn_nr, turn_nr + 1);
    }

    
    void send_turn_to_players(uint16_t turn_nr)
    {
        // wysyła do wszystkich, obserwatorów też
        char data[BUFFER_LEN];
        serialize_turn(turn_nr, data);
        TODO
    }

    void start_game()
    {
        current_game_info = GameInfo(this->game_length);

        current_game_info.turn_number = 0;
    
        for (int i = 0; i < players_count_for_game; i++)
        {
            // ponieważ gracze się nie odłączają to to są grający
            Position pos = random_generator.get_random_position(size_x, size_y);
            players[i].position = pos;
            PlayerMoved event(i, pos);
            events[0].push_back(event);
        }

        for (int i = 0; i < initial_blocks; i++)
        {
            Position pos = random_generator.get_random_position(size_x, size_y);
            current_game_info.blocks.push_back(pos);
            BlockPlaced event(pos);
            events[0].push_back(event);
        }

        send_turn_to_players(0);
    }
    
    void execute_turn()
    {
        /* zdarzenia = []

dla każdej bomby:
    zmniejsz jej licznik czasu o 1
    jeśli licznik wynosi 0:
        zaznacz, które bloki znikną w wyniku eksplozji
        zaznacz, które roboty zostaną zniszczone w wyniku eksplozji
        dodaj zdarzenie `BombExploded` do listy
        usuń bombę    
    
dla każdego gracza w kolejności id:
    jeśli robot nie został zniszczony:
        jeśli gracz wykonał ruch:
            obsłuż ruch gracza i dodaj odpowiednie zdarzenie do listy
    jeśli robot został zniszczony:
        pozycja_x_robota = random() % szerokość_planszy
        pozycja_y_robota = random() % wysokość_planszy
    
        dodaj zdarzenie `PlayerMoved` do listy
        
zwiększ nr_tury o 1 */
    }

    void run_game_server()
    {
        mutex.lock();
        while(true)
        {
            // lobby
            mutex.unlock();
            while (true) // czekamy aż się uzbierają gracze
            {
                // w tym czasie wątki od obsługi tcp same wysyłają im hello i info o dotychczasowych graczach
                // i odpowiadają na join i dodają do listy graczy tu
                mutex.lock();
                if (players_count == players_count_for_game)
                {
                    // oleję też na razie że może ich się od razu zrobić więcej
                    break; // doczekaliśmy się
                }
                mutex.unlock();
            }
            // game
            // mutex jest locked
            // gracze są w mapie
            start_game();
            for (int i = 0; i < game_length; i++)
            {
                mutex.unlock();
                sleep(turn_duration);
                mutex.lock();
                execute_turn();
            }
            end_game();
        }
    }
};

int main(int argc, char *argv[])
{
    CommandlineArguments arguments(argc, argv);
    arguments.process_commandline_arguments();
    std::thread 
            tcp_server_thread([port_server, server_host_address]() 
            {
                boost::asio::io_context io_context;
                run_tcp_server(io_context, server_host_address, port_server); 
            });
    tcp_server_thread.detach();
    GameServer game_server(arguments);
    game_server.run_game_server();
    return 0;
}
