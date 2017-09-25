#ifdef USER_HPP
#warning You should only include Nine.hpp to use this library
#else
#define USER_HPP

class User : public NPATHERNS::Singleton<User> {
    friend class NPATHERNS::Singleton<User>;
    friend void ApplyUserForceAndTorqueCallback(const NewtonBody * nBody);

public:
    User();
    virtual ~User();

    void Init();
    void Init(const std::string &path);
    void SetNoClip(bool active);
    void Refresh();
    void DrawHUD();
    void Use();
    void AttachObject(NOBJECTS::Object *obj);

    glm::vec3 GetPosition();
    glm::mat4 GetMatrix();
    glm::mat4 GetWeaponMatrix();
    NewtonBody* GetBody();

    void SetPosition(glm::vec3 pos);
    void SetControls(NSYSTEM::Controls &ctr);

private:
    bool _useBack, _shootBack, _secondBack;
    NOBJECTS::Object *_obj;
    NOBJECTS::Weapon *_weapon;
};

#endif // USER_HPP
