#ifdef SPRITE_HPP
#warning You should only include Nine.hpp to use this library
#else
#define SPRITE_HPP

class Sprite : public NPATHERNS::Entity {
public:
    Sprite();
    ~Sprite();
    Sprite(float size, glm::vec3 pos, std::string path);

    void Load(float size, glm::vec3 pos, std::string path);

    void Draw(float elapsedTime);

protected:
    float _size;
    Image _tex;
};

#endif // SPRITE_HPP

