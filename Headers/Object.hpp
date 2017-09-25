#ifdef OBJECT_HPP
#warning You should only include Nine.hpp to use this library
#else
#define OBJECT_HPP

class Object : public NPATHERNS::Entity {
    friend void ApplyForceAndTorqueCallback (const NewtonBody * nBody);

public:
    Object();
    Object(const std::string &path, glm::mat4 matrix, GLfloat mass, std::string name = "object");
    Object(const std::string &path, float size, float collideSize, glm::vec3 pos, GLfloat mass, std::string name = "object");
    virtual ~Object();

    void Init(const std::string &path, glm::mat4 matrix, float mass, std::string name = "object");
    void Init(const std::string &path, float size, float collideSize, glm::vec3 pos, float mass, std::string name = "object");

    virtual glm::vec3 GetPosition();
    void ActivateGravity(bool activate);
    void Impulse(glm::vec3 imp);

    virtual void SetPosition(glm::vec3 pos);
    virtual void SetRotation(glm::mat4 rot);
    virtual void SetMatrix(glm::mat4 matrix);
    virtual void SetMass(float mass);

    void IncreaseLife(int value);
    void DecreaseLife(int value);

    int Use();

    virtual void Destroy();

    void TemporaryDisableAutoSleep();

    void SetAnim(std::string animName) {    _md5Object->setAnim(animName);    }
    virtual void Check(float elapsedTime);
    virtual void Draw(float elapsedTime);
    const NMODELS::Md5Object* GetMd5Object() {
        return _md5Object;
    }

protected:
    void Md5ParseScript(const std::string &filename);

    bool _tdas; // Temporary Disable Auto Sleep
    int _life;
    float _mass;
    glm::vec3 _slowness;
    glm::mat4 _md5mat;
    std::string _name;
    NMODELS::Nwm *_nwm;
    NMODELS::Md5Model *_md5Model;
    NMODELS::Md5Object *_md5Object;
    NRENDERING::Sprite *_sprite;
    NTOOLS::Script _dscFile;
};

#endif // OBJECT_HPP
