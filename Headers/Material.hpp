#ifdef MATERIAL_HPP
#warning You should only include Nine.hpp to use this library
#else
#define MATERIAL_HPP

class Material : public NRESOURCES::Resource {
    template <typename T>
    struct uniform {
        std::string name;
        T data;
    };

public:
    Material();
    Material(const std::string &path);
    ~Material();

    int Load(const std::string &path);
    void Use(std::vector<glm::vec3> &tangent, std::vector<glm::vec3> &binormal, std::vector<glm::vec3> &normal);
    void Unuse();

    void Add(std::string name, const bool var);
    void Add(std::string name, const float var);
    void Add(std::string name, const std::string &var);
    void Add(std::string name, const glm::vec2 var);
    void Add(std::string name, const glm::vec3 var);
    void Add(std::string name, const glm::vec4 var);
    void Add(std::string name, const glm::mat2 var);
    void Add(std::string name, const glm::mat3 var);
    void Add(std::string name, const glm::mat4 var);

    Shader* GetShader() {
        return _shader;
    }

    // Attribut modifiers
    void ActiveNormal() {
        *_normal = true;
    }
    void ActiveBinormal() {
        *_binormal = true;
    }
    void ActiveTangent() {
        *_tangent = true;
    }
    /////////////////////

    // Attribut accessors
    bool GetNormal() {
        return *_normal;
    }
    bool GetBinormal() {
        return*_binormal;
    }
    bool GetTangent() {
        return *_tangent;
    }
    /////////////////////

protected:
    bool *_normal, *_binormal, *_tangent;
    NRENDERING::Image *_tex;
    std::vector<NRENDERING::Image*> *_imgs;
    NTOOLS::Script *_script;
    Shader *_shader;

    // Uniforms
    std::vector<uniform<bool> > *_uBool;
    std::vector<uniform<float> > *_uFloat;
    std::vector<uniform<NRENDERING::Image*> > *_uImg;
    std::vector<uniform<glm::vec2> > *_uVec2;
    std::vector<uniform<glm::vec3> > *_uVec3;
    std::vector<uniform<glm::vec4> > *_uVec4;
    std::vector<uniform<glm::mat2> > *_uMat2;
    std::vector<uniform<glm::mat3> > *_uMat3;
    std::vector<uniform<glm::mat4> > *_uMat4;
    //////////
};

#endif // MATERIAL_H
