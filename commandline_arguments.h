#ifndef _COMMANDLINE_ARGUMENTS
#define _COMMANDLINE_ARGUMENTS

#include <cstdint>
#include <string>
#include <boost/program_options.hpp>

class CommandlineArguments
{
    public:

    CommandlineArguments(int argc, char *argv[]);

    boost::program_options::variables_map parameter_map;

    uint16_t get_bomb_timer();
    uint16_t get_players_count();
    uint64_t get_turn_duration();
    uint16_t get_explosion_radius();
    uint16_t get_initial_blocks();
    uint16_t get_game_length();
    std::string get_server_name();
    uint16_t get_port();
    uint32_t get_seed();
    uint16_t get_size_x();
    uint16_t get_size_y();
};

#endif
