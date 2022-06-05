class GameServer // odpowiednik server_info
{
    public:

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
    RandomGenerator random_generator;
    std::map<PlayerId, Player> connected_players; // na razie olejmy możliwość odłączania się graczy, bo gdyby mogli to mogliby wciąż być w grze, a tu nie, ale niech to będzie główny zbiór graczy
    std::vector<std::vector<Event>> events;

    void start_game()
    {
        for (int i = 0; i < nr_players_for_game; i++)
        {
            // ponieważ gracze się nie odłączają to to są grający
            Position pos = random_generator.get_random_position(size_x, size_y);
            players[i].position = pos;
            PlayerMoved(i, pos) event;
            events[0].push_back(event);
        }

        for (int i = 0; i < initial_blocks; i++)
        {
            
        }
        /* nr_tury = 0
        zdarzenia = []

        dla każdego gracza w kolejności id:
            pozycja_x_robota = random() % szerokość_planszy
            pozycja_y_robota = random() % wysokość_planszy
            
            dodaj zdarzenie `PlayerMoved` do listy
            
        tyle razy ile wynosi parametr `initial_blocks`:
            pozycja_x_bloku = random() % szerokość_planszy
            pozycja_y_bloku = random() % wysokość_planszy
            
            dodaj zdarzenie `BlockPlaced` do listy
            
        wyślij komunikat `Turn` */
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
            for (int turn_nr = 0; turn_nr < game_length; turn_nr++)
            {
                sleep(turn_duration);
                execute_turn();
            }
        }
    }
};

int main(int argc, char *argv[])
{
    process_commandline_arguments();
    std::thread 
            tcp_server_thread([port_server, server_host_address]() 
            {
                boost::asio::io_context io_context;
                run_tcp_server(io_context, server_host_address, port_server); 
            });
    tcp_server_thread.detach();
    GameServer game_server;
    game_server.run_game_server();
    return 0;
}
