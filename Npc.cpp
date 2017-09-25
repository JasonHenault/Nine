#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NOBJECTS;

namespace cnp {
    int nowIndex;
    std::vector<glm::vec3> forces;
}


Npc* currentNpc = NULL;     // for SerAnim function (see bottom)
int SetAnim(lua_State* L) {  // Info()
    if (currentNpc != NULL)
        if (lua_isstring(L, 1))
            currentNpc->SetAnim(lua_tostring(L, 1));
    return 0;
}

int SetPos(lua_State* L) {
    if (currentNpc == NULL)
        return 0;

    glm::vec3 pos;

    if (lua_isnumber(L, 1))
        pos.x = lua_tonumber(L, 1);
    else return 0;

    if (lua_isnumber(L, 2))
        pos.y = lua_tonumber(L, 2);
        else return 0;

    if (lua_isnumber(L, 3))
        pos.z = lua_tonumber(L, 3);
    else return 0;


    currentNpc->SetPosition(pos);
    return 0;
}

int IsLastFrame(lua_State* L) {
    if (currentNpc == NULL)
        return 0;

    lua_pushnumber(L, currentNpc->GetMd5Object()->isLastFrame());

    return 1;
}

void ApplyNpcForceAndTorque (const NewtonBody* body, dFloat timestep, int threadIndex) {
    if (NSYSTEM::Physics::GetState() != NSYSTEM::Physics::stop) {
        GLfloat mass;
        glm::vec3 slowness;

        NewtonBodyGetMassMatrix(body, &mass, &slowness.x, &slowness.y, &slowness.z);

        cnp::forces[cnp::nowIndex] *= mass * 9.81 * 0.1;
        cnp::forces[cnp::nowIndex].y = -mass * 9.81;

        NewtonBodyAddForce(body, &cnp::forces[cnp::nowIndex].x);

        cnp::nowIndex++;

        if (cnp::nowIndex == cnp::forces.size())
            cnp::nowIndex = 0;
    }
}

Npc::Npc(std::string path, glm::mat4 matrix, GLfloat mass, bool gravity) {
    _type = "Npc";
    _script.Execute(path);

    _life = _script.GetNumber("life");
    Init(_script.GetString("model"), matrix, mass);
    SetAnim(_script.GetString("anim"));

    _script.AddFunction("SetAnim", ::SetAnim);
    _script.AddFunction("SetPos", ::SetPos);
    _script.AddFunction("IsLastFrame", ::IsLastFrame);
}

Npc::~Npc() {
}

void Npc::Check(float elapsedTime) {
    currentNpc = this;

    _script.ActiveFunction("Walk");
    glm::vec3 pos = GetPosition();
    _script.PushNumber(pos.x);
    _script.PushNumber(pos.y);
    _script.PushNumber(pos.z);
    _script.CallFunction(3, 0);

    currentNpc = NULL;
}

