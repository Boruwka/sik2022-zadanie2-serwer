/* Deserializuje liczbę o rozmiarze size. */
size_t deserialize_number(char data[], size_t size);

/* Serializuje numer o rozmiarze size do tablicy data. */
size_t serialize_number(char data[], size_t number, size_t size);

/* Serializuje string s do tablicy data.
Zwracana wartość to długość całego zapisu stringa. */
size_t serialize_string(char data[], std::string s);

/* Deserializuje string do tablicy data,
w zmiennej pos zapisuje pozycję po nim. */
std::string deserialize_string(char data[], size_t& pos);


/* Serializuje kierunek. Zwraca długość zapisu. */
size_t serialize_direction(Direction direction, char data[]);


/* Deserializuje kierunek. */
Direction deserialize_direction(char data[]);

size_t serialize_event(char data[], Event e);

size_t serialize_game_ended(char data[], std::map<PlayerId, Score> scores);
