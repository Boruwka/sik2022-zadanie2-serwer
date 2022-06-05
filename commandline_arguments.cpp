class CommandlineArguments
{
    CommandlineArguments(int argc, char *argv[])
    {
        boost::program_options::options_description description("App usage");
        boost::program_options::variables_map parameter_map;
     
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
        dalej_je_sprocesuj_i_do_jakichs_zmiennych();
    }
};
