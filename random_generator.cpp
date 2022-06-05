class RandomGenerator
{
    public:

    RandomGenerator()
    {
        this->seed = time(NULL);
        this->state = 0;
        this->prev = this->seed;
    }

    RandomGenerator(uint32_t seed)
    {
        this->seed = (uint64_t)seed;
        this->state = 0;
        this->prev = this->seed;
    }

    
    uint64_t prev;
    uint64_t state;
    uint64_t seed;

    uint32_t get_random()
    {
        prev = (prev * 48271) % 2147483647;
        state++;
        return (uint32_t)prev;
    }

    uint32_t get_random_from_range(uint32_t left, uint32_t right)
    {
        uint32_t res = get_random();
        res %= (right - left);
        ref += left;
        return res;
        
    }

    Position get_random_position(uint16_t size_x, uint16_t size_y)
    {
        uint16_t pos_x = (uint16_t)get_random_from_range(0, size_x);
        uint16_t pos_y = (uint16_t)get_random_from_range(0, size_y);
        return Position(pos_x, pos_y);
    }
};
