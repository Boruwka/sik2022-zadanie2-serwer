class RandomGenerator
{
    public:

    RandomGenerator();

    RandomGenerator(uint32_t seed);

    
    uint64_t prev;
    uint64_t state;
    uint64_t seed;

    uint32_t get_random();

    uint32_t get_random_from_range(uint32_t left, uint32_t right);

    Position get_random_position(uint16_t size_x, uint16_t size_y);
};
