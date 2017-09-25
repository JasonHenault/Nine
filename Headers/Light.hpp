#ifdef LIGHT_HPP
#warning You should only include Nine.hpp to use this library
#else
#define LIGHT_HPP

class Light : public NPATHERNS::Entity {
    enum {ambient, spot, laser, strobe};

    public:
    Light();
    Light(glm::vec3 pos);
    Light(glm::vec3 pos, glm::vec3 color);
    virtual ~Light();

    void SetPosition(glm::vec3 pos);
    void SetRotation(glm::vec3 angles);
    void SetMatrix(glm::mat4 matrix);
    glm::vec3 GetRotation();
    glm::mat4 GetMatrix();

    void operator*=(glm::mat4 m);
    void RotateInDirection(glm::vec3 pos);

    void SetColor(glm::vec3 color);

    void AttachLight(NPATHERNS::Entity *ent);
    void AttachDirection(NPATHERNS::Entity *ent);

    void Check(float elapsedTime);

    protected:
    int _mode;
    GLenum _numLight;
    glm::vec4 _col, _mat, _color;
    glm::vec3 _angles;
    glm::mat4 _matrix;
    NRENDERING::Sprite *_sprite;
    NOBJECTS::Object *_attachedLight, *_attachedRay;
};

#endif // LIGHT_HPP
