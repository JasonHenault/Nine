#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NPATHERNS;

int idCount = 1;

Entity::Entity() : _id(idCount), _type("Entity") {
    GetScene() += this;
    idCount++;
}

Entity::~Entity() {
    GetScene() -= this;
}

void Entity::Refresh(float elapsedTime) {
    Check(elapsedTime);
    Draw(elapsedTime);
}

void Entity::Attach(NPATHERNS::Entity *ent) {
    _attached.push_back(ent);
}

void Entity::Unattach(NPATHERNS::Entity *ent) {
    if (!_attached.empty()) {
        for(std::vector<NPATHERNS::Entity*>::iterator it = _attached.begin(); it != _attached.end(); it++)
            if((*it) == ent)
                _attached.erase(it);
    }
}
