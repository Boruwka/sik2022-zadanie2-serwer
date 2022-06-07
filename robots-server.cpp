#include "commandline_arguments.h"
#include "event.h"
#include "object_definitions.h"
#include "player.h"
#include "random_generator.h"
#include "serialization_deserialization.h"
#include "sending_messages.h"
#include <set>

class GameInfo
{
    public:

    uint16_t turn_number;
    std::set<Position> blocks;
    std::vector<std::vector<Event>> events;
    std::map<PlayerId, Score> scores;
    std::map<BombId, Bomb> bombs;
    std::map<PlayerId, bool> was_killed; // tylko przechowuje info tymczasowo, między śmiercią a odrodzeniem
    std::vector<PlayerId> players_playing;
    std::vector<std::vector<Square>> board;
    BombId first_free_bomb_id;
    
    
    GameInfo()
    {
        // default konstruktor żeby się kompilator nie czepiał
    }

    GameInfo(uint16_t game_length, uint8_t players_count, uint16_t size_x, uint16_t size_y)
    {        
        events = std::vector<std::vector<Event>>(game_length);
        turn_number = 0;
        for (size_t i = 0; i < players_count; i++)
        {
            // zakładamy, że grają po prostu pierwsze id na razie
            players_playing.push_back(i);
        }
        for (auto id: players_playing)
        {
            scores[id] = 0;
            was_killed[id] = false;
        }

        board = std::vector<std::vector<Square>>(size_x);
        for (size_t i = 0; i < size_x; i++)
        {
            board[i] = std::vector<Square>(size_y);
        }

        first_free_bomb_id = 0;

        
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
            for (auto& event: current_game_info.events[i])
            {
                pos += serialize_event(&(data[pos]), event);
            }
        }
        return pos;
        
    }

    

    size_t serialize_turn(char data[], uint16_t turn_nr)
    {
        // musi przyjmować turn nr bo możemy chcieć przesłać inną niż aktualna turę
        return serialize_turns(data, turn_nr, turn_nr, turn_nr + 1);
    }


    
    void send_turn_to_players(uint16_t turn_nr)
    {
        // wysyła do wszystkich, obserwatorów też
        char data[BUFFER_LEN];
        size_t length = serialize_turn(data, turn_nr);
        
        for (auto& [id, player]: players) 
        {
            player.send_message(data, length);
        }
    }

    void end_game()
    {
        char data[BUFFER_LEN];
        size_t length = serialize_game_ended(data, current_game_info.scores);

        for (auto& [id, player]: players) 
        {
            player.send_message(data, length);
        }
    }

    void start_game()
    {
        current_game_info = GameInfo(this->game_length, this->players_count_for_game, this->size_x, this->size_y);

        current_game_info.turn_number = 0;
    
        for (int i = 0; i < players_count_for_game; i++)
        {
            // ponieważ gracze się nie odłączają to to są grający
            Position pos = random_generator.get_random_position(size_x, size_y);
            players[i].position = pos;
            current_game_info.board[pos.x][pos.y].players.insert(i);
            PlayerMoved event(i, pos);
            current_game_info.events[0].push_back(event);
        }

        for (int i = 0; i < initial_blocks; i++)
        {
            Position pos = random_generator.get_random_position(size_x, size_y);
            current_game_info.blocks.insert(pos);
            current_game_info.board[pos.x][pos.y].block = true;
            BlockPlaced event(pos);
            current_game_info.events[0].push_back(event);
        }

        send_turn_to_players(0);
    }

    BombExploded execute_bomb_explosion(Bomb bomb)
    {
        //BombExploded(BombId id, std::vector<Position> blocks_destroyed, std::vector<PlayerId> robots_destroyed)
        // to trzeba policzyć
        // ta funkcja aktualizuje też stan odpowiednich pól
        // no i generuje parametry do BombExploded
        std::vector<Position> blocks_destroyed;
        std::vector<PlayerId> robots_destroyed;

        for (uint16_t i = 0; i < explosion_radius; i++)
        {
            uint16_t pos_x = bomb.position.x - i;
            uint16_t pos_y = bomb.position.y;

            if (pos_x < 0)
            {
                break;
            }

            for (auto& player_id: current_game_info.board[pos_x][pos_y].players)
            {
                robots_destroyed.push_back(player_id);
                current_game_info.was_killed[player_id] = true;
            }
            
            if (current_game_info.board[pos_x][pos_y].block)
            {
                current_game_info.board[pos_x][pos_y].block = false;
                blocks_destroyed.push_back(Position(pos_x, pos_y));
                break; // bo blok kończy wybuch
            }
            
        }
        
        for (uint16_t i = 0; i < explosion_radius; i++)
        {
            uint16_t pos_x = bomb.position.x + i;
            uint16_t pos_y = bomb.position.y;

            if (pos_x >= size_x)
            {
                break;
            }

            for (auto& player_id: current_game_info.board[pos_x][pos_y].players)
            {
                robots_destroyed.push_back(player_id);
                current_game_info.was_killed[player_id] = true;
            }
            
            if (current_game_info.board[pos_x][pos_y].block)
            {
                current_game_info.board[pos_x][pos_y].block = false;
                blocks_destroyed.push_back(Position(pos_x, pos_y));
                break; // bo blok kończy wybuch
            }
            
        }

        for (uint16_t i = 0; i < explosion_radius; i++)
        {
            uint16_t pos_x = bomb.position.x;
            uint16_t pos_y = bomb.position.y - i;

            if (pos_y < 0)
            {
                break;
            }

            for (auto& player_id: current_game_info.board[pos_x][pos_y].players)
            {
                robots_destroyed.push_back(player_id);
                current_game_info.was_killed[player_id] = true;
            }
            
            if (current_game_info.board[pos_x][pos_y].block)
            {
                current_game_info.board[pos_x][pos_y].block = false;
                blocks_destroyed.push_back(Position(pos_x, pos_y));
                break; // bo blok kończy wybuch
            }
            
        }

        for (uint16_t i = 0; i < explosion_radius; i++)
        {
            uint16_t pos_x = bomb.position.x;
            uint16_t pos_y = bomb.position.y + i;

            if (pos_y >= size_y)
            {
                break;
            }

            for (auto& player_id: current_game_info.board[pos_x][pos_y].players)
            {
                robots_destroyed.push_back(player_id);
                current_game_info.was_killed[player_id] = true;
            }
            
            if (current_game_info.board[pos_x][pos_y].block)
            {
                current_game_info.board[pos_x][pos_y].block = false;
                blocks_destroyed.push_back(Position(pos_x, pos_y));
                break; // bo blok kończy wybuch
            }
            
        }

        return BombExploded(bomb.id, blocks_destroyed, robots_destroyed);
    }

    PlayerMoved move_player_to_random(PlayerId id)
    {
        Position position = random_generator.get_random_position(size_x, size_y);
        current_game_info.board[position.x][position.y].players.insert(id);
        players[id].position = position;
        return PlayerMoved(id, position);
    }

    bool execute_players_action(PlayerId id, Event& e)
    {
        ClientMessage mess;
        bool any_message_was_read = false;
        while(players[id].get_first_message_from_deque(mess))
        {
            any_message_was_read = true;
            // jak zwróci false, to znaczy, że następnej nie ma, lub jest niekompletna           
        }

        if (!any_message_was_read)
        {
            // gracz nic nie zrobił
            return false;
        }
        
        // zakładamy, że nie ma tu join ani błędów
        switch(mess.type)
        {
            case (ClientMessageType::Move):
                switch(mess.direction)
                {
                    case (Direction::Up):
                        current_game_info.board[players[id].position.x][players[id].position.y].players.erase(id);
                        players[id].position.y++;
current_game_info.board[players[id].position.x][players[id].position.y].players.insert(id);
                        break;
                    case (Direction::Down):
                        current_game_info.board[players[id].position.x][players[id].position.y].players.erase(id);
                        players[id].position.y--;
current_game_info.board[players[id].position.x][players[id].position.y].players.insert(id);
                        break;
                    case (Direction::Left):
                        current_game_info.board[players[id].position.x][players[id].position.y].players.erase(id);
                        players[id].position.x--;
current_game_info.board[players[id].position.x][players[id].position.y].players.insert(id);
                        break;
                    case (Direction::Right):
                        current_game_info.board[players[id].position.x][players[id].position.y].players.erase(id);
                        players[id].position.x++;
current_game_info.board[players[id].position.x][players[id].position.y].players.insert(id);
                        break;
                }
                e = PlayerMoved(id, players[id].position);
                break;
            case (ClientMessageType::PlaceBlock):
                current_game_info.blocks.insert(players[id].position);
                current_game_info.board[players[id].position.x][players[id].position.y].block = true;
                e = BlockPlaced(players[id].position);
                break;
            case (ClientMessageType::PlaceBomb):    
                Bomb bomb(current_game_info.first_free_bomb_id, bomb_timer, players[id].position);
                current_game_info.first_free_bomb_id++;
                current_game_info.bombs[bomb.id] = bomb;
                current_game_info.board[players[id].position.x][players[id].position.y].bombs.insert(bomb.id);
                e = BombPlaced(bomb.id, players[id].position);
                break;
                
        } 
        // aktualizuje planszę, playersów i całe current_game_info
        // przypisujemy to zmiennej e
        return true;
    }
    
    void execute_turn()
    {
        for (auto& [id, bomb] : current_game_info.bombs)
        {
            bomb.timer--;
            if (bomb.timer == 0)
            {
                BombExploded explosion = execute_bomb_explosion(bomb);
                current_game_info.events[current_game_info.turn_number].push_back(explosion);  
                current_game_info.bombs.erase(id); 
            }
        }

        
        for (auto& [id, player] : players)
        {
            if (current_game_info.was_killed[id])
            {
                current_game_info.was_killed[id] = false;
                PlayerMoved move = move_player_to_random(id);
                current_game_info.scores[id]++;
                current_game_info.events[current_game_info.turn_number].push_back(move);  
            }
            else
            {
                Event move;
                if(execute_players_action(id, move))
                {
                current_game_info.events[current_game_info.turn_number].push_back(move);  
                }
               
            }
        }

        current_game_info.turn_number++;
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
