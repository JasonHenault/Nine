#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NLOGIC;

Trigger::Trigger(glm::vec3 downCorner, glm::vec3 upCorner, std::string funcName, NTOOLS::Script *script, std::vector<NPATHERNS::Entity*> *entities, std::string type, bool autoLock) : Entity(), _funcName(funcName), _type(type), _script(script), _ents(entities), _autoLock(autoLock), _triggered(false) {
    for (int i = 0; i < 3; i++) // Secure components
        if (downCorner[i] > upCorner[i]) {
            int down = downCorner[i];
            downCorner[i] = upCorner[i];
            upCorner[i] = down;
        }

    _downCorner = downCorner;
    _upCorner = upCorner;
    _point.x = (upCorner.x + downCorner.x) / 2;
    _point.y = (upCorner.y + downCorner.y) / 2;
    _point.z = (upCorner.z + downCorner.z) / 2;
    _tex = new NRENDERING::Image(NTOOLS::DecodePath("System::Textures/Patterns/trigger.dds"));
}

void Trigger::Refresh(float elapsedTime) {
    Check(elapsedTime);
    if (GetDebug())
        Draw(elapsedTime);
}

void Trigger::Check(float elapsedTime) {
    glm::vec3 entPos;
    int id = 0;

    if (_type == "user")
        entPos = NOBJECTS::User::Instance().Instance().GetPosition();
    else if (_type == "all") {
        std::vector<NPATHERNS::Entity*>::iterator winEnt = _ents->begin();
        float winDist = glm::distance((*winEnt)->GetPosition(), _point);
        float curDist;

        for (std::vector<NPATHERNS::Entity*>::iterator it = _ents->begin() + 1; it != _ents->end(); it++) {
            curDist = glm::distance((*it)->GetPosition(), _point);
            if (curDist < winDist) {
                winEnt = it;
                winDist = curDist;
            }
        }

        curDist = glm::distance(NOBJECTS::User::Instance().Instance().GetPosition(), _point);
        if (curDist < winDist)
            entPos = NOBJECTS::User::Instance().Instance().GetPosition();
        else {
            entPos = (*winEnt)->GetPosition();
            id = (*winEnt)->GetId();
        }
    }

    bool down = _downCorner.x <= entPos.x && _downCorner.y <= entPos.y && _downCorner.z <= entPos.z;
    bool up = _upCorner.x >= entPos.x && _upCorner.y >= entPos.y && _upCorner.z >= entPos.z;

    if (down && up && !_triggered) {
        _script->ActiveFunction(_funcName);
        _script->PushNumber(entPos.x);
        _script->PushNumber(entPos.y);
        _script->PushNumber(entPos.z);
        _script->PushNumber(id);

        _script->CallFunction(4,0);

        _triggered = _autoLock;
    } else if (!(down && up))
        _triggered = false;
}

void Trigger::Draw(float elapsedTime) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    _tex->Bind();
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;

    glBegin(GL_QUADS);

    // Front Face
    glTexCoord2f(0.f, 0.f);
    glVertex3f(_upCorner.x, _upCorner.y,  _downCorner.z);
    glTexCoord2f(1.f, 0.f);
    glVertex3f( _downCorner.x, _upCorner.y,  _downCorner.z);
    glTexCoord2f(1.f, 1.f);
    glVertex3f( _downCorner.x, _downCorner.y,  _downCorner.z);
    glTexCoord2f(0.f, 1.f);
    glVertex3f(_upCorner.x,  _downCorner.y,  _downCorner.z);

    // Back Face
    glTexCoord2f(1.f, 0.f);
    glVertex3f(_upCorner.x, _upCorner.y, _upCorner.z);
    glTexCoord2f(1.f, 1.f);
    glVertex3f(_upCorner.x,  _downCorner.y, _upCorner.z);
    glTexCoord2f(0.f, 1.f);
    glVertex3f( _downCorner.x,  _downCorner.y, _upCorner.z);
    glTexCoord2f(0.f, 0.f);
    glVertex3f( _downCorner.x, _upCorner.y, _upCorner.z);

    // Top Face
    glTexCoord2f(0.f, 1.f);
    glVertex3f(_upCorner.x,  _downCorner.y, _upCorner.z);
    glTexCoord2f(0.f, 0.f);
    glVertex3f(_upCorner.x,  _downCorner.y,  _downCorner.z);
    glTexCoord2f(1.f, 0.f);
    glVertex3f(_downCorner.x,  _downCorner.y,  _downCorner.z);
    glTexCoord2f(1.f, 1.f);
    glVertex3f( _downCorner.x,  _downCorner.y, _upCorner.z);

    // Bottom Face
    glTexCoord2f(1.f, 1.f);
    glVertex3f(_upCorner.x, _upCorner.y, _upCorner.z);
    glTexCoord2f(0.f, 1.f);
    glVertex3f( _downCorner.x, _upCorner.y, _upCorner.z);
    glTexCoord2f(0.f, 0.f);
    glVertex3f( _downCorner.x, _upCorner.y,  _downCorner.z);
    glTexCoord2f(1.f, 0.f);
    glVertex3f(_upCorner.x, _upCorner.y,  _downCorner.z);

    // Right face
    glTexCoord2f(1.f, 0.f);
    glVertex3f( _downCorner.x, _upCorner.y, _upCorner.z);
    glTexCoord2f(1.f, 1.f);
    glVertex3f( _downCorner.x,  _downCorner.y, _upCorner.z);
    glTexCoord2f(0.f, 1.f);
    glVertex3f( _downCorner.x,  _downCorner.y,  _downCorner.z);
    glTexCoord2f(0.f, 0.f);
    glVertex3f( _downCorner.x, _upCorner.y,  _downCorner.z);

    // Left Face
    glTexCoord2f(0.f, 0.f);
    glVertex3f(_upCorner.x, _upCorner.y, _upCorner.z);
    glTexCoord2f(1.f, 0.f);
    glVertex3f(_upCorner.x, _upCorner.y,  _downCorner.z);
    glTexCoord2f(1.f, 1.f);
    glVertex3f(_upCorner.x,  _downCorner.y,  _downCorner.z);
    glTexCoord2f(0.f, 1.f);
    glVertex3f(_upCorner.x,  _downCorner.y, _upCorner.z);

    glEnd();

    glDisable(GL_BLEND) ;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
}

