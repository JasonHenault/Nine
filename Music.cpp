#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NAUDIO;

Music::Music(const std::string &path) : sf::Music() {
    //CreateResource<Music>(NTOOLS::DecodePath(path)); Non copyable -> constructeur de copies obligatoire
    Load(path);
}

int Music::Load (const std::string &path) {
    if (!openFromFile(NTOOLS::DecodePath(path))) {
        Error("Impossible to open : \"" + path + "\".")
        return error;
    }
}
