#ifdef RENDERER_HPP
#warning You should only include Nine.hpp to use this library
#else
#define RENDERER_HPP

class Renderer : public NPATHERNS::Singleton<Renderer> {
    friend class NPATHERNS::Singleton<Renderer>;

protected:
    Renderer();

public:
    // Draw
    static void SetResolution(int width, int height);
    static int GetWidth();
    static int GetHeight();
    static void Init();
    static void Start();
    static void End();

    static void ChangePerspective(float fovy = defaultFovy, float depth = defaultDepth);
    static void ShowGrid(int size);
    static int UnuseProgram();

    static bool ShaderSupported();

    static void Begin2D();
    static void End2D();
};
#endif // RENDERER_HPP
