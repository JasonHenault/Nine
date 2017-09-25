#ifdef SKYBOX_HPP
#warning You should only include Nine.hpp to use this library
#else
#define SKYBOX_HPP

class Skybox {
    public:
    Skybox();
    Skybox(std::string bottom, std::string top, std::string north, std::string east, std::string south, std::string west);

    void Load(std::string bottom, std::string top, std::string north, std::string east, std::string south, std::string west);
    void Draw();

    protected:
    bool _loaded;
    NRENDERING::Image *_bottom, *_top, *_north, *_east, *_south, *_west;
};

#endif // SKYBOX_HPP
