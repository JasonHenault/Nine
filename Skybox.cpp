#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NSCENE;

Skybox::Skybox(std::string bottom, std::string top, std::string north, std::string east, std::string south, std::string west) {
    Load(bottom, top, north, east, south, west);
}

void Skybox::Load(std::string bottom, std::string top, std::string north, std::string east, std::string south, std::string west) {
    _bottom = new NRENDERING::Image(NTOOLS::DecodePath(bottom));
    _top = new NRENDERING::Image(NTOOLS::DecodePath(top));
    _north = new NRENDERING::Image(NTOOLS::DecodePath(north));
    _east = new NRENDERING::Image(NTOOLS::DecodePath(east));
    _south = new NRENDERING::Image(NTOOLS::DecodePath(south));
    _west = new NRENDERING::Image(NTOOLS::DecodePath(west));
}

void Skybox::Draw() {
    if (_bottom != NULL) {
        glDepthMask(GL_FALSE);
        glPushMatrix();
        glMultMatrixf(&glm::translate(NOBJECTS::User::Instance().GetPosition())[0][0]);

        _north->Bind();
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1);
        glVertex3f(  100.f, -100.f, -100.f );
        glTexCoord2f(1, 1);
        glVertex3f( -100.f, -100.f, -100.f );
        glTexCoord2f(1, 0);
        glVertex3f( -100.f,  100.f, -100.f );
        glTexCoord2f(0, 0);
        glVertex3f(  100.f,  100.f, -100.f );
        glEnd();

        _west->Bind();
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1);
        glVertex3f(  100.f, -100.f,  100.f );
        glTexCoord2f(1, 1);
        glVertex3f(  100.f, -100.f, -100.f );
        glTexCoord2f(1, 0);
        glVertex3f(  100.f,  100.f, -100.f );
        glTexCoord2f(0, 0);
        glVertex3f(  100.f,  100.f,  100.f );
        glEnd();

        _south->Bind();
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1);
        glVertex3f( -100.f, -100.f,  100.f );
        glTexCoord2f(1, 1);
        glVertex3f(  100.f, -100.f,  100.f );
        glTexCoord2f(1, 0);
        glVertex3f(  100.f,  100.f,  100.f );
        glTexCoord2f(0, 0);
        glVertex3f( -100.f,  100.f,  100.f );

        glEnd();

        _east->Bind();
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1);
        glVertex3f( -100.f, -100.f, -100.f );
        glTexCoord2f(1, 1);
        glVertex3f( -100.f, -100.f,  100.f );
        glTexCoord2f(1, 0);
        glVertex3f( -100.f,  100.f,  100.f );
        glTexCoord2f(0, 0);
        glVertex3f( -100.f,  100.f, -100.f );
        glEnd();

        _top->Bind();
        glBegin(GL_QUADS);
        glTexCoord2f(1, 1);
        glVertex3f( -100.f,  100.f, -100.f );
        glTexCoord2f(1, 0);
        glVertex3f( -100.f,  100.f,  100.f );
        glTexCoord2f(0, 0);
        glVertex3f(  100.f,  100.f,  100.f );
        glTexCoord2f(0, 1);
        glVertex3f(  100.f,  100.f, -100.f );
        glEnd();

        _bottom->Bind();
        glBegin(GL_QUADS);
        glTexCoord2f(1, 1);
        glVertex3f( -100.f, -100.f, -100.f );
        glTexCoord2f(1, 0);
        glVertex3f( -100.f, -100.f,  100.f );
        glTexCoord2f(0, 0);
        glVertex3f(  100.f, -100.f,  100.f );
        glTexCoord2f(0, 1);
        glVertex3f(  100.f, -100.f, -100.f );
        glEnd();

        glPopMatrix();
        glDepthMask(GL_TRUE);
    }
}
