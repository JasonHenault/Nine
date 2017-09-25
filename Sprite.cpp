#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NOBJECTS;
using namespace NRENDERING;

Sprite::Sprite() {
    _type = "Sprite";
    GetScene() += this;
}

Sprite::Sprite(float size, glm::vec3 pos, std::string path) {
    _type = "Sprite";
    Load(size, pos, path);
    GetScene() += this;
}

Sprite::~Sprite() {
    GetScene() -= this;
}

void Sprite::Load(float size, glm::vec3 pos, std::string path) {
    _size = size;
    _pos = pos;
    _tex.Load(NTOOLS::DecodePath(path));
}

void Sprite::Draw(float elapsedTime) {
    glPushMatrix();

    glDisable(GL_LIGHTING);
    glEnable (GL_POINT_SPRITE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Attenuations coefs
    float coeffs[] = {1.f, 0.f, 0.f};
    glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, coeffs);

    glPointSize(_size * 250.f);

    _tex.Bind();

    glTexEnvf (GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

    glBegin(GL_POINTS);
    glVertex3f(_pos.x, _pos.y, _pos.z);
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    glPopMatrix();
}
