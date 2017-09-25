#ifdef NPC_HPP
#warning You should only include Nine.hpp to use this library
#else
#define NPC_HPP

class Npc : public Object {
public:
    Npc(std::string path, glm::mat4 matrix, GLfloat mass, bool gravity);
    virtual ~Npc();

    virtual void Check(float elapsedTime);

protected:
    int _forceIndex;
    NTOOLS::Script _script;
    sf::Sound _sound;
};

#endif // NPC_HPP
