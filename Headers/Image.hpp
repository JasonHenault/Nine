#ifdef IMAGE_HPP
#warning You should only include Nine.hpp to use this library
#else
#define IMAGE_HPP

// Files types supported
#include "Image/Dds_file.hpp"

class Image : public NRESOURCES::Resource {

public:
    Image();
    Image(const std::string &path);
    ~Image();

    int Load(const std::string &path);
    int Bind();
    int Active(const int texNb);
    int GetActTex() { return *_actTex; }
    GLuint GetId();

protected:
    gl_texture_t* ReadFile (const std::string &path);

    int *_actTex;
    GLuint *_tex;
};

#endif // IMAGE_HPP
