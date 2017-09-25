#ifdef MAP_HPP
#warning You should only include Nine.hpp to use this library
#else
#define MAP_HPP

class Map : public NPATHERNS::Entity {
    friend void ApplyForceAndTorqueCallback (const NewtonBody * nBody);

public:
    Map(const std::string &path);
    virtual ~Map();

    void Draw(float elapsedTime);

    glm::vec3 Min();
    glm::vec3 Max();
    float Size() { return _size; };

protected:
    float _size;
    NMODELS::Nwm *_nwm;
};

#endif // MAP_HPP
