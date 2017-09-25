#ifdef SHADER_HPP
#warning You should only include Nine.hpp to use this library
#else
#define SHADER_HPP

#include "TNB.hpp"

class Shader {
public:
    Shader() {} ;
    Shader(const std::string &path);
    Shader(const std::string &path1, const std::string &path2);
    ~Shader();

    enum {shader, program};

    int Use ();
    int Unuse ();
    std::string LoadShader(const std::string &path);

    int SetParameter(const std::string &var, bool param);    // Just one if a constant is sending
    int SetParameter(const std::string &var, int param);
    int SetParameter(const std::string &var, float param);
    int SetParameter(const std::string &var, NRENDERING::Image* param, const int texNb);
    int SetParameter(const std::string &var, glm::vec2 param);
    int SetParameter(const std::string &var, glm::vec3 param);
    int SetParameter(const std::string &var, glm::vec4 param);
    int SetParameter(const std::string &var, glm::mat2 param);
    int SetParameter(const std::string &var, glm::mat3 param);
    int SetParameter(const std::string &var, glm::mat4 param);

    int SetAttribut(const std::string &var, bool param);
    int SetAttribut(const std::string &var, int param);
    int SetAttribut(const std::string &var, float param);
    int SetAttribut(const std::string &var, glm::vec2 param);
    int SetAttribut(const std::string &var, glm::vec3 param);
    int SetAttribut(const std::string &var, glm::vec4 param);

    int ActiveAttributTable(const std::string &var, const std::vector<glm::vec3> &varArray);

protected:
    GLuint CreateFragmentShader(const std::string &path);
    GLuint CreateVertexShader(const std::string &path);
    int DeleteShader(GLuint &shader);
    int DebugCompile(GLuint shaderOrProgram, int type, const std::string &path = "undefined");

    GLuint _program;
    std::vector<int> _attTable;
};

#endif // SHADER_H

