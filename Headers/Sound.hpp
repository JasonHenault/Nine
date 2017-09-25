#ifdef SOUND_HPP
#warning You should only include Nine.hpp to use this library
#else
#define SOUND_HPP

class Sound : public NRESOURCES::Resource, NPATHERNS::Entity, public sf::Sound {
public:
    Sound();
    Sound (const std::string &path);

    int Load (const std::string &path);

protected:
    float _sampleRate, _duration;
};

#endif // SOUND_HPP
