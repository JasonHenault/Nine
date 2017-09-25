#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NSYSTEM;

namespace RndVar {
    int _width, _height;
    float _fovy, _depth;
}

Renderer::Renderer() {
}

void Renderer::SetResolution(int width, int height) {
    RndVar::_width = width;
    RndVar::_height = height;
}

int Renderer::GetWidth() {
    return RndVar::_width;
}

int Renderer::GetHeight() {
    return RndVar::_height;
}

void Renderer::Init() {
    glClearDepth(1.f);
    glClearColor(0.f, 0.f, 0.f, 0.f);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glLineWidth(1.3);
    glBlendColor(0, 0, 0, 1);   // black is black

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    ChangePerspective();
}

void Renderer::Start() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    ChangePerspective(RndVar::_fovy, RndVar::_depth);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    NSCENE::Camera::LookAt();
}

void Renderer::End() {
    UnuseProgram();
    SwapBuffers(NULL);
}

void Renderer::Begin2D () {
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, RndVar::_width, 0, RndVar::_height);

    glMatrixMode(GL_MODELVIEW);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
}

void Renderer::End2D () {
    glEnable(GL_DEPTH_TEST);
}

int Renderer::UnuseProgram () {
    glUseProgram(0);
    return success;
}

void Renderer::ChangePerspective(float fovy, float depth) {
    RndVar::_fovy = fovy;
    RndVar::_depth = depth;
    gluPerspective(fovy, RndVar::_width / RndVar::_height, 0.1f, depth);
}

void Renderer::ShowGrid(int size) {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_COLOR);

    glm::vec3 user = NOBJECTS::User::Instance().GetPosition();

    glBegin(GL_LINES);

    glColor3d(1, 0, 0);

    int backNegX = user.x - defaultDepth;
    int backPosX = user.x + defaultDepth;
    int backNegZ = user.z - defaultDepth;
    int backPosZ = user.z + defaultDepth;

    for (int x = backNegX - backNegX % size; x < backPosX - backPosX % size; x += size) {
        glVertex3f(x, 0, user.z - defaultDepth);
        glVertex3f(x, 0, user.z + defaultDepth);
    }

    for (int z = backNegZ - backNegZ % size; z < backPosZ - backPosZ % size; z += size) {
        glVertex3f(user.x - defaultDepth, 0, z);
        glVertex3f(user.x + defaultDepth, 0, z);
    }

    glColor3d(1, 1, 1);

    glEnd();

    glEnable(GL_TEXTURE_2D);
}

bool Renderer::ShaderSupported() {
    return GLEE_ARB_shader_objects;
}
