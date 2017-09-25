#ifdef PHYSICS_HPP
#warning You should only include Nine.hpp to use this library
#else
#define PHYSICS_HPP

class Physics : public NPATHERNS::Singleton<Physics> {
    friend class NPATHERNS::Singleton<Physics>;

protected:
    Physics();
    virtual ~Physics();

public:
    enum {stop, run};

    static int Init();
    static void Refresh();

    static int SetGravity(int state);
    static int GetGravity();

    static int SetState(int state);
    static int GetState();

    static void SetAccelerate(int accelerate);

    static NewtonWorld* GetWorld();
    static float GetStandardGravity();

    static int CreateMaterial();
    static void CreateMaterialPair(int *mat1, int *mat2);

    static NewtonBody* Raycast(glm::vec3 start, glm::vec3 direction);
};

#endif // PHYSICS_HPP
