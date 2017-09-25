#ifdef ENTITY_HPP
#warning You should only include Nine.hpp to use this library
#else
#define ENTITY_HPP

class Entity {
public:
    Entity();
    virtual ~Entity();

    virtual void Refresh(float elapsedTime);
    virtual void Check(float elapsedTime) {};
    virtual void Draw(float elapsedTime) {};

    virtual glm::vec3 GetPosition() {   return _pos;    };

    virtual void SetPosition(glm::vec3 pos) {   _pos = pos;     };

    void Attach(NPATHERNS::Entity *ent);
    void Unattach(NPATHERNS::Entity *ent);

    int GetId() {   return _id;    }

    std::string GetType() {     return _type;    }

    NewtonBody* GetBody() {     return _pBody;    }

protected:
    int _id;
    std::string _type;
    glm::vec3 _pos;
    NewtonBody *_pBody;
    std::vector<NPATHERNS::Entity*> _attached;
};

#endif // ENTITY_HPP
