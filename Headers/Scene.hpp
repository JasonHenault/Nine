#ifdef SCENE_HPP
#warning You should only include Nine.hpp to use this library
#else
#define SCENE_HPP

class Scene {
public:
    Scene();
    Scene(const std::string &path);
    virtual ~Scene();

    void SortSprites() {
        if (_sprites.size() > 2) {

            glm::vec3 uPos = NOBJECTS::User::Instance().GetPosition();

            bool sorting;

            do {
                sorting = false;

                for (int i = 0; i < _sprites.size() - 1; i++)
                    if (glm::distance(_sprites[i]->GetPosition(), uPos) != glm::distance(_sprites[i+1]->GetPosition(), uPos))
                        if (glm::distance(_sprites[i]->GetPosition(), uPos) < glm::distance(_sprites[i+1]->GetPosition(), uPos)) {
                            NRENDERING::Sprite *spr1 = _sprites[i];
                            _sprites[i] = _sprites[i+1];
                            _sprites[i+1] = spr1;

                            sorting = true;
                        }
            } while(sorting);
        }
    }

    void Refresh(float elapsedTime);

    NRENDERING::Sprite* CreateSprite(float size, glm::vec3 pos, std::string path);
    NLOGIC::Trigger* CreateTrigger(glm::vec3 downCorner, glm::vec3 upCorner, std::string funcName, std::string type = "user", bool autoLock = true);
    NOBJECTS::Object* CreateObject(const std::string &path, glm::mat4 mat, float weight);
    NOBJECTS::Object* CreateObject(const std::string &path, float size, float collideSize, glm::vec3 pos, float mass);
    NOBJECTS::Weapon* CreateWeapon(const std::string &path);
    NOBJECTS::Weapon* CreateWeapon(const std::string &path, glm::mat4 mat);
    NOBJECTS::Npc* CreateNpc(const std::string &path, glm::mat4 mat, float weight);
    NSCENE::Light* CreateLight(int x, int y, int z);
    NSCENE::Light* CreateLight(int x, int y, int z, float r, float v, float b);

    void Attach(NPATHERNS::Entity* entReceiving, NPATHERNS::Entity* entToAttach);
    void Attach(int idReceiving, int idToAttach);
    void Unattach(NPATHERNS::Entity* entReceiving, NPATHERNS::Entity* entToUnattach);
    void Unattach(int idReceiving, int idToUnattach);


    void operator+=(NPATHERNS::Entity *entity);
    void operator-=(NPATHERNS::Entity *entity);
    void operator+=(NRENDERING::Sprite *sprite);
    void operator-=(NRENDERING::Sprite *sprite);

    NPATHERNS::Entity* GetEntity(int id);

    NPATHERNS::Entity* Raycast(glm::vec3 start, glm::vec3 direction);

    bool Load(const std::string &path);
    bool SetSkybox(const std::string &path);
    bool SetMap(const std::string &path);
    float MapSize();
    Map* GetMap();

protected:
    void CreateWorldBox(glm::vec3 &min, glm::vec3 &max);

    Map *_map;
    Skybox *_skybox;
    std::vector<NPATHERNS::Entity*> _entities;
    std::vector<NRENDERING::Sprite*> _sprites;

    NTOOLS::Script _script;
};

#endif // SCENE_HPP
