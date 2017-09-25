#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NSCENE;
using namespace NRENDERING;
using namespace NOBJECTS;
using namespace NLOGIC;
using namespace NSYSTEM;

Scene::Scene() : _map(NULL), _skybox(NULL) {
}

Scene::~Scene() {
    if (_map != NULL)
        delete _map;
    if (_skybox != NULL)
        delete _skybox;
}

void Scene::Refresh(float elapsedTime) {
    Renderer::Start();

    User::Instance().Refresh();
    Physics::Refresh();

    if (_map == NULL)
        Renderer::ShowGrid(5);

    if (!_entities.empty()) {
    for (std::vector<NPATHERNS::Entity*>::iterator it = _entities.begin(); it != _entities.end(); it++)
        if ((*it)->GetType() == "Light")
            (*it)->Refresh(elapsedTime);

    for (std::vector<NPATHERNS::Entity*>::iterator it = _entities.begin(); it != _entities.end(); it++)
        if ((*it)->GetType() != "Sprite" && (*it)->GetType() != "Light")
            (*it)->Refresh(elapsedTime);

    SortSprites();

    for (std::vector<NRENDERING::Sprite*>::iterator it = _sprites.begin(); it != _sprites.end(); it++)
            (*it)->Refresh(elapsedTime);
    }

    if (_skybox != NULL)
        _skybox->Draw();

    NOBJECTS::User::Instance().DrawHUD();

    Renderer::End();
}

Sprite* Scene::CreateSprite(float size, glm::vec3 pos, std::string path) {
    Sprite *spr = new Sprite(size, pos, path);
    return spr;
}

Trigger* Scene::CreateTrigger(glm::vec3 downCorner, glm::vec3 upCorner, std::string funcName, std::string type, bool autoLock) {
    Trigger *trig = new Trigger(downCorner, upCorner, funcName, &_script, &_entities, type, autoLock);
    return trig;
}

Object* Scene::CreateObject(const std::string &path, glm::mat4 matrix, float weight) {
    Object *obj = new Object(path, matrix, weight);
    return obj;
}

Object* Scene::CreateObject(const std::string &path, float size, float collideSize, glm::vec3 pos, float mass) {
    Object *obj = new Object(path, size, collideSize, pos, mass);
    return obj;
}

NOBJECTS::Weapon* Scene::CreateWeapon(const std::string &path) {
    Weapon *weapon = new Weapon(path);

    return weapon;
}

NOBJECTS::Weapon* Scene::CreateWeapon(const std::string &path, glm::mat4 matrix) {
    Weapon *weapon = new Weapon(path, matrix);

    return weapon;
}

Npc* Scene::CreateNpc(const std::string &path, glm::mat4 matrix, float weight) {
    Npc *cnp = new Npc(path, matrix, weight, false);

    return cnp;
}

Light* Scene::CreateLight(int x, int y, int z) {
    Light *light = new Light(glm::vec3(x,y,z));

    return light;
}

Light* Scene::CreateLight(int x, int y, int z, float r, float v, float b) {
    Light *light = new Light(glm::vec3(x,y,z), glm::vec3(r,v,b));

    return light;
}

void Scene::Attach(NPATHERNS::Entity* entReceiving, NPATHERNS::Entity* entToAttach) {
    entReceiving->Attach(entToAttach);
}

void Scene::Attach(int idReceiving, int idToAttach) {
    NPATHERNS::Entity* entReceiving = GetEntity(idReceiving);
    NPATHERNS::Entity* entToAttach = GetEntity(idToAttach);

    if(entReceiving != NULL && entToAttach != NULL)
        Attach(entReceiving, entToAttach);
}

void Scene::Unattach(NPATHERNS::Entity* entReceiving, NPATHERNS::Entity* entToUnattach) {
    entReceiving->Unattach(entToUnattach);
}

void Scene::Unattach(int idReceiving, int idToUnattach) {
    NPATHERNS::Entity* entReceiving = GetEntity(idReceiving);
    NPATHERNS::Entity* entToUnattach = GetEntity(idToUnattach);

    if(entReceiving != NULL && entToUnattach != NULL)
        Unattach(entReceiving, entToUnattach);
}


void Scene::operator+=(NPATHERNS::Entity *entity) {
    _entities.push_back(entity);
}

void Scene::operator-=(NPATHERNS::Entity *entity) {
    for (std::vector<NPATHERNS::Entity*>::iterator it = _entities.begin(); it != _entities.end(); it++)
            if(*it == entity) {
                _entities.erase(it);
                break;
            }
}

void Scene::operator+=(NRENDERING::Sprite *sprite) {
    _sprites.push_back(sprite);
}

void Scene::operator-=(NRENDERING::Sprite *sprite) {
    for (std::vector<NRENDERING::Sprite*>::iterator it = _sprites.begin(); it != _sprites.end(); it++)
            if(*it == sprite) {
                _sprites.erase(it);
                break;
            }
}

NPATHERNS::Entity* Scene::GetEntity(int id) {
    for (std::vector<NPATHERNS::Entity*>::iterator it = _entities.begin(); it != _entities.end(); it++)
        if ((*it)->GetId() == id)
            return *it;

    return NULL;
}

NPATHERNS::Entity* Scene::Raycast(glm::vec3 start, glm::vec3 direction) {
    NewtonBody *body = NSYSTEM::Physics::Raycast(start, direction);

    for (std::vector<NPATHERNS::Entity*>::iterator it = _entities.begin(); it != _entities.end(); it++)
        if ((*it)->GetBody() == body)
            return *it;

    return NULL;
}

bool Scene::Load(const std::string &path) {
    Physics::SetGravity(Physics::run);

    if (_script.Execute(NTOOLS::DecodePath(path)) == success) {
        SetMap(_script.GetString("map"));
        SetSkybox(_script.GetString("skybox"));
        User::Instance().Init(_script.GetString("user"));
        return success;
    } else
        return error;
}

bool Scene::SetSkybox(const std::string &path) {
    if (path == "error")
        return false;

    std::string pathMod = path;
    if (pathMod[pathMod.size()-1] != '/')
        pathMod.push_back('/');

    std::string bottom, top, north, east, south, west;
    bottom = top = north = east = south = west = pathMod;

    bottom += "bottom.dds";
    top += "top.dds";
    north += "north.dds";
    east += "east.dds";
    south += "south.dds";
    west += "west.dds";

    _skybox = new Skybox(bottom, top, north, east, south, west);

    Package(path, "Skybox")

    return true;
}

bool Scene::SetMap(const std::string &path) {
    _map = new Map(path);

    glm::vec3 min = _map->Min();
    glm::vec3 max = _map->Max();
    glm:: vec3 minWorld = min - glm::vec3(1,1,1);
    glm:: vec3 maxWorld = max + glm::vec3(1,1,1);

    float mapSize = MapSize();
        if (mapSize != error)
            Renderer::ChangePerspective(70.f, mapSize + 1.f);

    NewtonSetWorldSize(Physics::GetWorld(), &minWorld[0], &maxWorld[0]); // Resize the world in function of scene
    CreateWorldBox(min, max); // protect the exit

    return true;
}

float Scene::MapSize() {
    if(_map != NULL)
        return _map->Size();
    return error;
}

Map* Scene::GetMap() {
    return _map;
}

void Scene::CreateWorldBox(glm::vec3 &min, glm::vec3 &max) {
    glm::vec3 boxSize = max - min;
    glm::mat4 offset = glm::translate(min.x - boxSize.x / 2, min.y - boxSize.y / 2, min.z - boxSize.z / 2);

    NewtonCollision *colBox = NewtonCreateBox(Physics::GetWorld(), boxSize.x, boxSize.y, boxSize.z, 0, &offset[0][0]);
    NewtonBody *bodyBox = NewtonCreateBody(Physics::GetWorld(), colBox, &(glm::mat4(0.f))[0][0]);
    NewtonBodySetMassMatrix(bodyBox, 0.0f, 0.0f, 0.0f, 0.0f);

    NewtonBodySetContinuousCollisionMode(bodyBox, 1);
}
