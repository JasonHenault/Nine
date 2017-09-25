#ifdef WEAPON_HPP
#warning You should only include Nine.hpp to use this library
#else
#define WEAPON_HPP

class Weapon : public Object {
public:
    Weapon(const std::string &path);
    Weapon(const std::string &path, glm::mat4 matrix);

    void Shoot();
    void Second();

private:
    NTOOLS::Script _script;
};

#endif // WEAPON_HPP

