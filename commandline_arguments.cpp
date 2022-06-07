#include "commandline_arguments.h"

CommandlineArguments::CommandlineArguments(int argc, char *argv[])
    {
        boost::program_options::options_description description("App usage");
        description.add_options()
            ("help,h", "Get help")
            ("bomb-timer,b", 
                boost::program_options::value<uint16_t>(), "Bomb timer")
            ("players-count,c", 
                boost::program_options::value<uint16_t>(), "Players count")
            ("turn-duration,d", 
                boost::program_options::value<uint64_t>(), "Turn duration in miliseconds")
            ("explosion-radius,e", 
                boost::program_options::value<uint16_t>(), "Explosion radius")
            ("initial-blocks,k", 
                boost::program_options::value<uint16_t>(), "Initial blocks")
            ("game-length,k", 
                boost::program_options::value<uint16_t>(), "Game length")
            ("server-name,n", 
                boost::program_options::value<std::string>(), "Server name")
            ("port,p", 
                boost::program_options::value<uint16_t>(), "Port")
            ("seed,s", 
                boost::program_options::value<uint32_t>(), "Seed")
            ("size-x,x", 
                boost::program_options::value<uint16_t>(), "Size x")
            ("size-y,y", 
                boost::program_options::value<uint16_t>(), "Size y");

        boost::program_options::store(
            boost::program_options::command_line_parser(argc, argv).options(description).run(), 
            parameter_map);
        
        
    }

    uint16_t CommandlineArguments::get_bomb_timer()
    {
        if (parameter_map.count("bomb-timer"))
        {
            return parameter_map["bomb-timer"].as<uint16_t>();
        }
        else
        {
            fprintf(stderr, "Stop. No bomb timer given.");
            exit(1);
        }
    }

    uint16_t CommandlineArguments::get_players_count()
    {
        if (parameter_map.count("players-count"))
        {
            return parameter_map["players-count"].as<uint16_t>();
        }
        else
        {
            fprintf(stderr, "Stop. No players count given.");
            exit(1);
        }
    }

    uint64_t CommandlineArguments::get_turn_duration()
    {
        if (parameter_map.count("turn-duration"))
        {
            return parameter_map["turn-duration"].as<uint64_t>();
        }
        else
        {
            fprintf(stderr, "Stop. No turn duration given.");
            exit(1);
        }
    }

    uint16_t CommandlineArguments::get_explosion_radius()
    {
        if (parameter_map.count("explosion-radius"))
        {
            return parameter_map["explosion-radius"].as<uint16_t>();
        }
        else
        {
            fprintf(stderr, "Stop. No explosion radius given.");
            exit(1);
        }
    }

    uint16_t CommandlineArguments::get_initial_blocks()
    {
        if (parameter_map.count("initial-blocks"))
        {
            return parameter_map["initial-blocks"].as<uint16_t>();
        }
        else
        {
            fprintf(stderr, "Stop. No initial blocks given.");
            exit(1);
        }
    }

    uint16_t CommandlineArguments::get_game_length()
    {
        if (parameter_map.count("game-length"))
        {
            return parameter_map["game-length"].as<uint16_t>();
        }
        else
        {
            fprintf(stderr, "Stop. No game length given.");
            exit(1);
        }
    }

    std::string CommandlineArguments::get_server_name()
    {
        if (parameter_map.count("server-name"))
        {
            return parameter_map["server-name"].as<std::string>();
        }
        else
        {
            fprintf(stderr, "Stop. No server name given.");
            exit(1);
        }
    }

    uint16_t CommandlineArguments::get_port()
    {
        if (parameter_map.count("port"))
        {
            return parameter_map["port"].as<uint16_t>();
        }
        else
        {
            fprintf(stderr, "Stop. No port given.");
            exit(1);
        }
    }

    uint32_t CommandlineArguments::get_seed()
    {
        if (parameter_map.count("seed"))
        {
            return parameter_map["seed"].as<uint32_t>();
        }
        else
        {
            fprintf(stderr, "Stop. No seed given.");
            exit(1);
        }
    }

    uint16_t CommandlineArguments::get_size_x()
    {
        if (parameter_map.count("size-x"))
        {
            return parameter_map["size-x"].as<uint16_t>();
        }
        else
        {
            return time(NULL); // randomowe seed
        }
    }

    uint16_t CommandlineArguments::get_size_y()
    {
        if (parameter_map.count("size-y"))
        {
            return parameter_map["size-y"].as<uint16_t>();
        }
        else
        {
            fprintf(stderr, "Stop. No size-y given.");
            exit(1);
        }
    }
