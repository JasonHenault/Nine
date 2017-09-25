#ifndef GAME_HPP
#define GAME_HPP

namespace engine {
    enum {run, stop};
}

class Game : public NPATHERNS::Singleton<Game> {
    friend class NPATHERNS::Singleton<Game>;

public:
    Game(void);
    virtual ~Game(void);

    void CheckAndDraw(void);

    bool LoadScene(std::string path);
    bool UnloadScene(void);
    Scene* GetScene();
    NewtonWorld* GetWorld(void);

private:
    Scene *_scn;
};

#endif // GAME_HPP
