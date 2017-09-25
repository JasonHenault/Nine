#ifdef TRIGGER_HPP
#warning You should only include Nine.hpp to use this library
#else
#define TRIGGER_HPP

class Trigger : public NPATHERNS::Entity {
public:
    Trigger(glm::vec3 downCorner, glm::vec3 upCorner, std::string funcName, NTOOLS::Script *script, std::vector<NPATHERNS::Entity*> *entities, std::string type = "user", bool autoLock = true);
    ~Trigger() {};

    void Refresh(float elapsedTime);
    void Check(float elapsedTime);
    void Draw(float elapsedTime);

protected:
    bool _autoLock, _triggered;
    glm::vec3 _downCorner, _upCorner, _point;
    std::string _funcName, _type;
    NTOOLS::Script *_script;
    NRENDERING::Image *_tex;
    std::vector<NPATHERNS::Entity*> *_ents;
};

#endif // TRIGGER_HPP

