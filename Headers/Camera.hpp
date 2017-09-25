#ifdef CAMERA_HPP
#warning You should only include Nine.hpp to use this library
#else
#define CAMERA_HPP

#define SENSIBILITY 0.0009

class Camera {

public:
    Camera();
    virtual ~Camera();

    static glm::vec3 GetForward();
    static glm::vec3 GetPosition();
    static glm::vec3 GetTarget();
    static void SetUserControls(NSYSTEM::Controls &ctr, glm::mat4 matrix);
    static void SetPosition(glm::vec3 &eye);
    static void SetTarget(glm::vec3 &target);

    static void LookAt();
};

#endif // CAMERA_HPP
