#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NSCENE;

namespace pCamera {
    static float _theta, _phy;
    static glm::vec3 _eye, _target, _forward;
}
using namespace pCamera;

Camera::Camera() {
    _eye = glm::vec3(0.f);
    _target = glm::vec3(1.f, 0.f, 0.f);
}

Camera::~Camera() {
}

glm::vec3 Camera::GetForward() {
    return _forward;
}

void Camera::SetUserControls(NSYSTEM::Controls &ctr, glm::mat4 matrix) {
    _theta += ctr.x * SENSIBILITY;
    _phy -= ctr.y * SENSIBILITY;

    if (_phy > 1.5534)  // 1.5534rd = 89°
        _phy = 1.5534;
    else if (_phy < -1.5534)
        _phy = -1.5534;

    _eye.x = matrix[3][0];
    _eye.y = matrix[3][1];
    _eye.z = matrix[3][2];

    _forward.x = cos(_theta) * cos(_phy);
    _forward.y = sin(_phy);
    _forward.z = sin(_theta) * cos(_phy);

    _forward = glm::normalize(_forward);

    _target = _eye + _forward;
}

glm::vec3 Camera::GetPosition() {
    return _eye;
}

glm::vec3 Camera::GetTarget() {
    return _target;
}

void Camera::SetPosition(glm::vec3 &eye) {
    _eye = eye;
}

void Camera::SetTarget(glm::vec3 &target) {
    _target = target;
}

void Camera::LookAt() {
    gluLookAt(_eye.x, _eye.y, _eye.z, _target.x, _target.y, _target.z, 0.f, 1.f, 0.f);
}
