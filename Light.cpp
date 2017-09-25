#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NSCENE;

int numLight = GL_LIGHT0;

Light::Light() : Entity(), _attachedLight(NULL), _attachedRay(NULL) {
    _type = "Light";

    _numLight = numLight;

    glEnable(numLight);
    numLight++;

    _sprite = new NRENDERING::Sprite(4, glm::vec3(0.f), "System::Textures/omni.dds");
}

Light::Light(glm::vec3 pos) : Entity(), _attachedLight(NULL), _attachedRay(NULL), _color(1.f) {
    _type = "Light";

    _numLight = numLight;

    glEnable(numLight);
    numLight++;

    _sprite = new NRENDERING::Sprite(4, pos, "System::Textures/omni.dds");
    SetPosition(pos);
}

Light::Light(glm::vec3 pos, glm::vec3 color) : Entity(), _attachedLight(NULL), _attachedRay(NULL) {
    _type = "Light";

    _numLight = numLight;

    glEnable(numLight);
    numLight++;

    _sprite = new NRENDERING::Sprite(4, pos, "System::Textures/omni.dds");
    SetPosition(pos);
    SetColor(color);
}

Light::~Light() {
}

void Light::SetPosition(glm::vec3 pos) {
    _pos = pos;
    _sprite->SetPosition(pos);
}

void Light::SetRotation(glm::vec3 angles) {
}

void Light::SetMatrix(glm::mat4 matrix) {
}

glm::vec3 Light::GetRotation() {
}

glm::mat4 Light::GetMatrix() {
}

void Light::operator*=(glm::mat4 m) {
}

void Light::RotateInDirection(glm::vec3 pos) {
}

void Light::SetColor(glm::vec3 color) {
    _color = glm::vec4(color, 1.f);
}

void Light::AttachLight(NPATHERNS::Entity *ent) {
}

void Light::AttachDirection(NPATHERNS::Entity *ent) {
}

void Light::Check(float elapsedTime) {
    glEnable(GL_LIGHTING);
    glEnable(_numLight);
    glLightfv(_numLight, GL_DIFFUSE, &(_color[0]));
}

