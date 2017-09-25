#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NSYSTEM;

namespace pPhysics {
int _state, _gravity;
float _accelerate, _sGravity;
sf::Clock _clock;
NewtonWorld *_nWorld;
NewtonBody *_raycasted;
glm::vec3 _start, _dir;
std::vector<int*> _matPair;
int _debugCount;
std::vector<std::vector<glm::vec3> > _debugFace;
}
using namespace pPhysics;

dFloat RaycastCallback(const NewtonBody* body, const dFloat* hitNormal, int collisionID, void* userData, dFloat intersectParam) {
    if (body == NOBJECTS::User::Instance().GetBody() || body == Nine::GetScene().GetMap()->GetBody())
        return 1;   // ignore, continue

    _raycasted = (NewtonBody*)body;
    return intersectParam;      //stop
}

void BodyLeaveWorld(const NewtonBody* body, int thread) {
    NewtonDestroyBody(_nWorld, body);
}

Physics::Physics() {
}

Physics::~Physics() {
}

int Physics::Init() {
    _state = run;
    _gravity = stop;
    _sGravity = 9.81f;
    _accelerate = 1;
    _nWorld = NewtonCreate();
    _raycasted = NULL;

    glm::vec3 sizeMin(-500, -500, -500);
    glm::vec3 sizeMax(500, 500, 500);
    NewtonSetWorldSize(_nWorld, &sizeMin.x, &sizeMax.x);
    NewtonSetBodyLeaveWorldEvent(_nWorld, BodyLeaveWorld);

    if (_nWorld != NULL)
        return success;
    else
        return error;
}

void Physics::Refresh() {
    /*if(GetDebug())
        DebugShowCollision();*/

    NewtonUpdate(_nWorld, _clock.getElapsedTime().asMicroseconds());
    _clock.restart();

    glDisable(GL_TEXTURE);
    glColor3ub(255, 0, 0);

    glBegin(GL_LINES);
    glVertex3d(_start.x, _start.y, _start.z);
    glVertex3d(_dir.x, _dir.y, _dir.z);
    glEnd();

    glColor3ub(255, 255, 255);
    glEnable(GL_TEXTURE);
}

int Physics::SetGravity(int state) {
    if (_gravity == run || _gravity == stop) {
        _gravity = state;
        return success;
    } else
        return error;
}

int Physics::GetGravity() {
    return _gravity;
}

int Physics::SetState(int state) {
    if (_state == run || _state == stop) {
        _state = state;
        return success;
    } else
        return error;
}

int Physics::GetState() {
    return _state;
}

void Physics::SetAccelerate(int accelerate) {
    _accelerate = accelerate;
}

NewtonWorld* Physics::GetWorld() {
    return _nWorld;
}

float Physics::GetStandardGravity() {
    return _sGravity;
}

int Physics::CreateMaterial() {
    return success;
}

void Physics::CreateMaterialPair(int* mat1, int* mat2) {
}

NewtonBody* Physics::Raycast(glm::vec3 start, glm::vec3 direction) {
    direction = 100.f * (direction - start) + direction;

    _start = start; // debug
    _dir = direction;

    NewtonWorldRayCast(_nWorld, &(start.x), &(direction.x), RaycastCallback, NULL, NULL);

    return _raycasted;
}
