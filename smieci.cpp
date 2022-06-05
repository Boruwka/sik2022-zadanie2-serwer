class Player
{
    PlayerId id;
    Score score;
    std::string address;
    std::string name;
    uint16_t port;
    std::shared_ptr<boost::asio::ip::tcp::tcp::socket> socket;
    std::deque<char> message;
    
    size_t serialize_accepted_player(char data[])
    {
        data[0] = 1;
        size_t pos = 1;
        pos += serialize_number(&(data[pos]), this->id, sizeof(this->id));
        pos += serialize_string(&(data[pos]), this->name);
        pos += serialize_string(&(data[pos]), this->address);
        return pos;
    }
};



class ServerInfo
{
    // stałe dla danego serwera, nie zmieniają się gdy się rozgrywka skończy 
    std::map<PlayerId, Player> connected_players;
    Game current_game;
    Lobby current_lobby; // jedno z nich jest aktualne

    
    uint16_t bomb_timer;
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

    size_t serialize_hello(char data[])
    {
        size_t pos;
        data[0] = 0;
        pos = 1;
        pos += serialize_string(&(data[pos]), this->server_name);
        pos += serialize_number(&(data[pos]), this->players_count, sizeof(this->players_count));    
        pos += serialize_number(&(data[pos]), this->size_x, sizeof(this->size_x));  
        pos += serialize_number(&(data[pos]), this->size_y, sizeof(this->size_y)); 
        pos += serialize_number(&(data[pos]), this->game_length, sizeof(this->game_length)); 
        pos += serialize_number(&(data[pos]), this->explosion_radius, sizeof(this->explosion_radius)); 
        pos += serialize_number(&(data[pos]), this->bomb_timer, sizeof(this->bomb_timer));  
        return pos;  
    }

};

ServerInfo server_info; // globalna zmienna z całym info

class Game
{
    Game()
    {
        turn_number = 0;
        for (auto player: accepted_players)
        {
            ustaw_pozycje_na_random(player);
            dodaj_player_move_do_eventow();
        }
        ustaw_initial_blocks();
        wyslij_komunikaty_turn();
    }

    std::vector<PlayerId> accepted_players;
    turn_number;
    std::vector<std::vector<events>> events;
    std::vector<Bomb> bombs;
    std::vector<Block> blocks;

    size_t serialize_game_started(char data[])
    {
        data[0] = 0;
        size_t pos = 1;
        pos += serialize_number(data, nr_accepted_player, sizeof(nr_accepted_players));
        for (auto player: accepted_players)
        {
            pos += serialize_player(player);
        }
        return pos;
    }
    
    size_t serialize_turn(char data[])
    {
        data[0] = 2;
        size_t pos = 1;
        pos += serialize_number(data, tour_nr, sizeof(tour_nr));
        for (auto e: events[tour_nr])
        {
            pos += serialize_event(&(data[pos]), e);
        }
        return pos;
    }

    size_t serialize_game_ended(char data[])
    {
        data[0] = 3;
        size_t pos = 1;
        pos += serialize_number(data, nr_accepted_player, sizeof(nr_accepted_players));
        for (auto player: accepted_players)
        {
            pos += serialize_number(&(data[pos]), player.id, sizeof(player.id));
            pos += serialize_number(&(data[pos]), player.score, sizeof(player.score));
        }
        return pos;
         
    }

    void explode_bomb(BombId id)
    {
        
        /*zaznacz, które bloki znikną w wyniku eksplozji
        zaznacz, które roboty zostaną zniszczone w wyniku eksplozji
        dodaj zdarzenie `BombExploded` do listy
        usuń bombę    */
    }

    void do_turn()
    {
        for (auto bomb: bombs)
        {
            bomb.timer--;
            if (bomb.timer == 0)
            {
                explode_bomb(bomb.id);
            }
        }
        for (auto player: players)
        {
            if (!destroyed_players[player.id].count())
            {
                // ten gracz nie został zniszczony
                process_client_message(player.last_message);
                
            }
            else
            {
                // ten gracz został zniszczony
                pos_x = server_info.random_generator.generate_number_from_range(0, server_info.size_x);                
                pos_y = server_info.random_generator.generate_number_from_range(0, server_info.size_y);
                process_player_moved(player.id, pos_x, pos_y);
            }

            server_info.game.tour_number++;
        }
    /*
dla każdego gracza w kolejności id:
    jeśli robot nie został zniszczony:
        jeśli gracz wykonał ruch:
            obsłuż ruch gracza i dodaj odpowiednie zdarzenie do listy
    jeśli robot został zniszczony:
        pozycja_x_robota = random() % szerokość_planszy
        pozycja_y_robota = random() % wysokość_planszy
    
        dodaj zdarzenie `PlayerMoved` do listy
        
zwiększ nr_tury o 1*/
    }

    void manage_game()
    {
        for (int i = 0; i < nr_turns; i++)
        {
            sleep(turn_duration);
            for (auto player: accepted_players)
            {
                if (player.has_pending_message())
                {
                    deserialize_message();
                }
            }
        }
    }

    
};

class Lobby
{
    // stan lobby
};





void start_game()
{
    server_info.game = new Game();
    server_info.game.serialize_game_started();
    /*dla każdego gracza w kolejności id:
    pozycja_x_robota = random() % szerokość_planszy
    pozycja_y_robota = random() % wysokość_planszy
    
    dodaj zdarzenie `PlayerMoved` do listy
    
tyle razy ile wynosi parametr `initial_blocks`:
    pozycja_x_bloku = random() % szerokość_planszy
    pozycja_y_bloku = random() % wysokość_planszy
    
    dodaj zdarzenie `BlockPlaced` do listy
    
wyślij komunikat `Turn` */
    
    server_info.game.accepted_players = server_info.players();
    server_info.current_game.move_players_to_random();

    for (int i = 0; i < server_info.initial_blocks; i++)
    {
        int pos_x = server_info.random_generator.get_number_from_range(0, server_info.size_x);
        int pos_y = server_info.random_generator.get_number_from_range(0, server_info.size_y);
        server_info.current_game.add_block(size_x, size_y); 
        // ta funkcja wyżej dodaje też block added do listy
    }

    server_info.game.send_turn(0);
}



int main(int argc, char *argv[])
{
    

    return 0;
}
