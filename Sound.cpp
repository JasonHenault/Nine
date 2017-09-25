#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NAUDIO;

Sound::Sound (const std::string &path) : sf::Sound() {
    CreateResource<Sound>(NTOOLS::DecodePath(path));
}

int Sound::Load (const std::string &path) {
    sf::SoundBuffer buffer;

    if (!buffer.loadFromFile(NTOOLS::DecodePath(path))) {
        Error("Impossible to open : \"" + path + "\".")
        return error;
    }

    _sampleRate = buffer.getSampleRate();
    _duration = buffer.getDuration().asSeconds();

    if (_duration > 20.f)
        Warning(static_cast<std::string>("Sound to long (> 20 secs) :") + path)

    setBuffer(buffer);

    return success;
}
