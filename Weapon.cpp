#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NOBJECTS;

Weapon::Weapon(const std::string &path) {
    _script.Execute(path);
    Init(_script.GetString("model"), User::Instance().GetWeaponMatrix(), 0.f, "weapon");
    _type = "Weapon";
}

Weapon::Weapon(const std::string &path, glm::mat4 matrix) {
    _script.Execute(path);
    Init(_script.GetString("model"), matrix, 10.f, "weapon");
    _type = "Weapon";
}

void Weapon::Shoot() {
    if (_script.ActiveFunction("shoot") != error)
        _script.CallFunction(0,0);
    else
        Error("No shoot function in weapon")
}

void Weapon::Second() {
    if (_script.ActiveFunction("second") != error)
        _script.CallFunction(0,0);
    else
        Info("No second (second shoot mode) function in weapon")
}
