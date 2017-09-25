#ifdef MUSIC_HPP
#warning You should only include Nine.hpp to use this library
#else
#define MUSIC_HPP

class Music : public NRESOURCES::Resource, NPATHERNS::Entity, public sf::Music {
public:
    Music(const std::string &path);
    ~Music () {};

    int Load (const std::string &path);
};

#endif // MUSIC_HPP
