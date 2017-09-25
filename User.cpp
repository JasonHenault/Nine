#include "Nine.hpp"
#include "FastAccess.hpp"

#define STEP 0.1

using namespace NOBJECTS;

namespace pUser {
bool noClip;
NSYSTEM::Controls _ctr;
NewtonBody *_pBody;
int _sight_size, _state_size;
NRENDERING::Image _sight, _state;
}

using namespace pUser;

void ApplyUserForceAndTorque (const NewtonBody* body, dFloat timestep, int threadIndex) {
    if (NSYSTEM::Physics::GetState() != NSYSTEM::Physics::stop) {
        GLfloat mass;
        glm::vec3 slowness;
        glm::mat4 matrix;
        NewtonBodyGetMatrix(body, &matrix[0][0]);

        NewtonBodyGetMassMatrix(body, &mass, &slowness.x, &slowness.y, &slowness.z);

        NSCENE::Camera::SetUserControls(_ctr, matrix);

        glm::vec3 force, forward = NSCENE::Camera::GetForward();

        glm::vec3 left = glm::normalize(glm::cross(glm::vec3(0, 1, 0), forward));

        if (Nine::GetJoystick()) {
            force.x += (_ctr.antPost / 50)*forward.x * mass * 9.81 * 10;
            force.z += (_ctr.antPost / 50)*forward.z * mass * 9.81 * 10;

            force.x -= (_ctr.levoRect / 50)*left.x * mass * 9.81 * 10;
            force.z -= (_ctr.levoRect / 50)*left.z * mass * 9.81 * 10;
        } else {
            if (_ctr.front) {
                force.x += (forward.x + forward.x * _ctr.run) * mass * 9.81 * 10;
                force.z += (forward.z + forward.z * _ctr.run) * mass * 9.81 * 10;
                if (noClip)
                    force.y += (forward.y + forward.y * _ctr.run) * mass * 9.81 * 10;
            }
            if (_ctr.back) {
                force.x -= (forward.x + forward.x * _ctr.run) * mass * 9.81 * 10;
                force.z -= (forward.z + forward.z * _ctr.run) * mass * 9.81 * 10;
                if (noClip)
                    force.y += (-forward.y - forward.y * _ctr.run) * mass * 9.81 * 10;
            }
            if (_ctr.right) {
                force.x -= (left.x + left.x * _ctr.run) * mass * 9.81 * 10;
                force.z -= (left.z + left.z * _ctr.run) * mass * 9.81 * 10;
                if (noClip)
                    force.y += (-left.y - left.y * _ctr.run) * mass * 9.81 * 10;
            }
            if (_ctr.left) {
                force.x += (left.x + left.x * _ctr.run) * mass * 9.81 * 10;
                force.z += (left.z + left.z * _ctr.run) * mass * 9.81 * 10;
                if (noClip)
                    force.y += (left.y + left.y * _ctr.run) * mass * 9.81 * 10;
            }
        }

        if (!noClip && NSYSTEM::Physics::GetGravity() != NSYSTEM::Physics::stop)
            force.y -= mass * 9.81 * 10; // check coef

        NewtonBodyAddForce(body, &force.x);

        glm::vec3 target = NSCENE::Camera::GetTarget();
        sf::Listener::setPosition(matrix[3][0], matrix[3][1], matrix[3][2]);
        sf::Listener::setDirection(target.x, target.y, target.z);
    }
}

User::User() : _obj(NULL) {
}

User::~User() {
}

void User::Init() {
    _useBack = false;
    _shootBack = false;
    _secondBack = false;
    _obj = NULL;

    NewtonWorld *nWorld = NSYSTEM::Physics::GetWorld();
    NewtonCollision *collision = NULL;
    collision = NewtonCreateSphere(nWorld, 2.f, 2.8f * 4.f, 2.f, 0, NULL);
    _pBody = NewtonCreateBody(nWorld, collision, &(glm::mat4(1.f))[0][0]);

    NewtonJoint* upVector;
    glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
    upVector = NewtonConstraintCreateUpVector(nWorld, &upDirection.x, _pBody);

    NewtonBodySetContinuousCollisionMode(_pBody, true);

    glm::mat4 matrix = glm::translate(glm::mat4(1.0f), 0.0f, 5.0f, 0.0f);
    NewtonBodySetMatrix(_pBody, &matrix[0][0]);

    if (_pBody == NULL)
        Error("Impossible to load the user's body")
        else {
            NewtonReleaseCollision(nWorld, collision);

            glm::vec3 slowness;

            slowness.x = 0.7f * 80.f * (0.4f * 0.4f + 1.8f * 1.8f) / 12;
            slowness.y = 0.7f * 80.f * (0.2f * 0.2f + 1.8f * 1.8f) / 12;
            slowness.z = 0.7f * 80.f * (0.2f * 0.2f + 0.4f * 0.4f) / 12;

            NewtonBodySetMassMatrix(_pBody, 80.f, slowness.x, slowness.y, slowness.z);

            noClip = false;

            NewtonBodySetForceAndTorqueCallback(_pBody, ApplyUserForceAndTorque);
        }
}

void User::Init(const std::string &path) {
    Init();

    NTOOLS::Script script;
    if (script.Execute(path) == success) {
        if (script.GetString("sight") != "error")
            _sight.Load(script.GetString("sight"));
        if (script.GetNumber("sight_size") != error)
            _sight_size = script.GetNumber("sight_size");
        if (script.GetString("state") != "error")
            _state.Load(script.GetString("state"));
        if (script.GetNumber("state_size") != error)
            _state_size = script.GetNumber("state_size");
        if (script.GetString("weapon") != "error")
            _weapon = GetScene().CreateWeapon(script.GetString("weapon"));

        _sight_size * 1024 / NSYSTEM::Renderer::GetWidth();
        _state_size * 1024 / NSYSTEM::Renderer::GetWidth();
    } else
        Error("User path is wrong")
    }

void User::SetNoClip(bool active) {
    noClip = active;
}

void User::Refresh() {
    if (_ctr.use) {
        if (!_useBack) {
            Use();
            _useBack = true;
        }
    } else
        _useBack = false;

    if (_ctr.shoot) {
        if (!_shootBack)
            if (_weapon != NULL) {
                _weapon->Shoot();
                _shootBack = true;
            }
    } else
        _shootBack = false;

    if (_ctr.second) {
        if (!_secondBack)
            if (_weapon != NULL) {
                _weapon->Second();
                _secondBack = true;
            }
    } else
        _secondBack = false;

    if (_obj != NULL) {
        glm::vec4 pos = glm::vec4((4.f + 1.f) * NSCENE::Camera::GetTarget() - 4.f * GetPosition(), 0);
        glm::mat4 mat = GetMatrix();
        mat[3] = pos;
        _obj->SetMatrix(mat);
        _obj->TemporaryDisableAutoSleep();
    }
}

glm::vec3 User::GetPosition() {
    glm::mat4 matrix;
    NewtonBodyGetMatrix(_pBody, &matrix[0][0]);
    return glm::vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
}

glm::mat4 User::GetMatrix() {
    glm::mat4 matrix;
    NewtonBodyGetMatrix(_pBody, &matrix[0][0]);
    return matrix;
}

glm::mat4 User::GetWeaponMatrix() {
    glm::vec4 pos = glm::vec4((4.f + 1.f) * NSCENE::Camera::GetTarget() - 4.f * GetPosition(), 0);
    glm::mat4 mat = GetMatrix();
    mat[3] = pos;

    return mat;
}

NewtonBody* User::GetBody() {
    return _pBody;
}

void User::SetPosition(glm::vec3 pos) {
    glm::mat4 matrix;
    NewtonBodyGetMatrix(_pBody, &matrix[0][0]);
    matrix[3] = glm::vec4(pos, 1.f);
    NewtonBodySetMatrix(_pBody, &matrix[0][0]);
}

void User::SetControls(NSYSTEM::Controls &ctr) {
    _ctr = ctr;
}

void User::Use() {
    if (_obj == NULL) {
        NPATHERNS::Entity* entity = GetScene().Raycast(GetPosition(), NSCENE::Camera::GetTarget());

        if (entity != NULL)
            if (entity->GetType() == "Object") {
                Object *raycasted = static_cast<Object*>(entity);

                if (raycasted->Use() == error)
                    AttachObject(raycasted);
            }
    } else {
        _obj->ActivateGravity(true);
        _obj = NULL;
    }
}

void User::AttachObject(NOBJECTS::Object *obj) {
    _obj = obj;

    if (_obj != NULL)
        _obj->ActivateGravity(false);
}

void User::DrawHUD() {
    int width = NSYSTEM::Renderer::GetWidth();
    int height = NSYSTEM::Renderer::GetHeight();

    NSYSTEM::Renderer::Begin2D();

    // Draw sight
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int Xmin = width/2 - _sight_size;
    int Xmax = width/2 + _sight_size;
    int Ymin = height/2 - _sight_size;
    int Ymax = height/2 + _sight_size;

    _sight.Bind();
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0);
    glVertex2d(Xmin, Ymin);
    glTexCoord2d(0, 1);
    glVertex2d(Xmin, Ymax);
    glTexCoord2d(1, 1);
    glVertex2d(Xmax, Ymax);
    glTexCoord2d(1, 0);
    glVertex2d(Xmax, Ymin);
    glEnd();
    /////////////

    // Draw life and ammo (size = y)
    Xmin = (5/100)*width;
    Xmax = _state_size*2 + Xmin;
    Ymin = (5/100)*height;
    Ymax = _state_size + Ymin;

    _state.Bind();
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0);
    glVertex2d(Xmin, Ymin);
    glTexCoord2d(0, 1);
    glVertex2d(Xmin, Ymax);
    glTexCoord2d(1, 1);
    glVertex2d(Xmax, Ymax);
    glTexCoord2d(1, 0);
    glVertex2d(Xmax, Ymin);
    glEnd();
    /////////////////////

    NSYSTEM::Renderer::End2D();
}
