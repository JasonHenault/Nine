#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NRENDERING;

namespace ScriptMaterial {
    NRENDERING::Material *currentMat = NULL;

    void Active(Material *mat) {
        currentMat = mat;
    }

    void Desactive() {
        currentMat = NULL;
    }

    int Uniform(lua_State* L) {
        if (currentMat != NULL) {
            switch (lua_gettop(L)) {
            case 2: {
                if (lua_isboolean(L,2))
                    currentMat->Add(lua_tostring(L,1), (bool)lua_toboolean(L,2));   // bool
                else if (lua_isnumber(L,2))
                    currentMat->Add(lua_tostring(L,1), (float)lua_tonumber(L,2));     // int or float
                else
                    currentMat->Add(lua_tostring(L,1), static_cast<std::string>(lua_tostring(L,2)));     // image
            }
            break;

            case 3:
                currentMat->Add(lua_tostring(L,1), glm::vec2((float)lua_tonumber(L,2), (float)lua_tonumber(L,3)));   // vec2
                break;

            case 4:
                currentMat->Add(lua_tostring(L,1), glm::vec3((float)lua_tonumber(L,2), (float)lua_tonumber(L,3), (float)lua_tonumber(L,4)));     // vec3
                break;

            case 6: {
                if (lua_isboolean(L,6))
                    if (lua_toboolean(L,6) == false)
                        currentMat->Add(lua_tostring(L,1), glm::vec4((float)lua_tonumber(L,2), (float)lua_tonumber(L,3), (float)lua_tonumber(L,4), (float)lua_tonumber(L,5)));     // vec4
                    else
                        currentMat->Add(lua_tostring(L,1), glm::mat2((float)lua_tonumber(L,2), (float)lua_tonumber(L,3), (float)lua_tonumber(L,4), (float)lua_tonumber(L,5)));     // mat2
            }
            break;

            case 10: {  // mat3
                glm::mat3 mat3;

                for (int i = 0; i < 3; i++)
                    for (int j = 0; j < 3; j++)
                        mat3[j][i] = lua_tonumber(L, 1+i*3+j);

                currentMat->Add(lua_tostring(L,1), mat3);
            }
            break;

            case 17: {  // mat4
                glm::mat4 mat4;

                for (int i = 0; i < 4; i++)
                    for (int j = 0; j < 4; j++)
                        mat4[j][i] = lua_tonumber(L, 1+i*4+j);

                currentMat->Add(lua_tostring(L,1), mat4);
            }
            break;

            default:
                Error("Too less or many arguments")
            };
        }
    }

    int Attrib(lua_State* L) {
        if (currentMat != NULL) {
            if (lua_tostring(L,1) == "normal")
                currentMat->ActiveNormal();
            if (lua_tostring(L,1) == "binormal")
                currentMat->ActiveBinormal();
            if (lua_tostring(L,1) == "tangent")
                currentMat->ActiveTangent();
        }
    }
}

Material::Material() {
    *_normal = false;
    *_binormal = false;
    *_tangent = false;
}

Material::Material(const std::string &path) {
    _normal = new bool;
    _binormal = new bool;
    _tangent = new bool;

    *_normal = false;
    *_binormal = false;
    *_tangent = false;

    _shader = NULL;
    _tex = new NRENDERING::Image;
    _imgs = new std::vector<NRENDERING::Image*>;
    _script = new NTOOLS::Script;

    // Uniforms
    _uBool = new std::vector<uniform<bool> >;
    _uFloat = new std::vector<uniform<float> >;
    _uImg = new std::vector<uniform<NRENDERING::Image*> >;
    _uVec2 = new std::vector<uniform<glm::vec2> >;
    _uVec3 = new std::vector<uniform<glm::vec3> >;
    _uVec4 = new std::vector<uniform<glm::vec4> >;
    _uMat2 = new std::vector<uniform<glm::mat2> >;
    _uMat3 = new std::vector<uniform<glm::mat3> >;
    _uMat4 = new std::vector<uniform<glm::mat4> >;

    _script->AddFunction("Uniform", ScriptMaterial::Uniform);
    _script->AddFunction("Attrib", ScriptMaterial::Attrib);

    CreateResource<Material>(NTOOLS::DecodePath(path));
}

Material::~Material() {
    if (NRESOURCES::ResMng::Instance().Unregister(_path) == success) {
        delete _normal;
        delete _binormal;
        delete _tangent;
        delete _shader;
        delete _tex;
        delete _imgs;
        delete _script;
        delete _uBool;
        delete _uFloat;
        delete _uImg;
        delete _uVec2;
        delete _uVec3;
        delete _uVec4;
        delete _uMat2;
        delete _uMat3;
        delete _uMat4;
    }
}

int Material::Load(const std::string &path) {
    ScriptMaterial::Active(this);

    if (_script->Execute(NTOOLS::DecodePath(path)) == error)
        return error;

    std::string vert = _script->GetString("vert"), frag = _script->GetString("frag");

    if (vert != "error" && frag != "error")
        _shader = new Shader(vert, frag);
    else if (vert != "error")
        _shader = new Shader(vert);
    else if (frag != "error")
        _shader = new Shader(frag);
    else
        Info("No shader in material's file : \"" + path + "\".")

    if (_script->GetString("texture") != "error")
        _tex = new Image(NTOOLS::DecodePath(_script->GetString("texture")));

    if (_script->ActiveFunction("update") != error)
        _script->CallFunction(0,0);

    ScriptMaterial::Desactive();
}

void Material::Use(std::vector<glm::vec3> &tangent, std::vector<glm::vec3> &binormal, std::vector<glm::vec3> &normal) {
    if (_shader != NULL && NSYSTEM::Renderer::ShaderSupported()) {
        _shader->Use();

        if (_uBool->size() > 0)
            for (int i = 0; i < _uBool->size(); i++)
                _shader->SetParameter((*_uBool)[i].name, (*_uBool)[i].data);
        if (_uFloat->size() > 0)
            for (int i = 0; i < _uFloat->size(); i++)
                _shader->SetParameter((*_uFloat)[i].name, (*_uFloat)[i].data);
        if (_uImg->size() > 0)
            for (int i = 0; i < _uImg->size(); i++)
                _shader->SetParameter((*_uImg)[i].name, (*_uImg)[i].data, i);
        if (_uVec2->size() > 0)
            for (int i = 0; i < _uVec2->size(); i++)
                _shader->SetParameter((*_uVec2)[i].name, (*_uVec2)[i].data);
        if (_uVec3->size() > 0)
            for (int i = 0; i < _uVec3->size(); i++)
                _shader->SetParameter((*_uVec3)[i].name, (*_uVec3)[i].data);
        if (_uVec4->size() > 0)
            for (int i = 0; i < _uVec4->size(); i++)
                _shader->SetParameter((*_uVec4)[i].name, (*_uVec4)[i].data);
        if (_uMat2->size() > 0)
            for (int i = 0; i < _uMat2->size(); i++)
                _shader->SetParameter((*_uMat2)[i].name, (*_uMat2)[i].data);
        if (_uMat3->size() > 0)
            for (int i = 0; i < _uMat3->size(); i++)
                _shader->SetParameter((*_uMat3)[i].name, (*_uMat3)[i].data);
        if (_uMat4->size() > 0)
            for (int i = 0; i < _uMat4->size(); i++)
                _shader->SetParameter((*_uMat4)[i].name, (*_uMat4)[i].data);


        /*if (_tangent) {
            if (_shader->ActiveAttributTable("tangent", tangent) == error)
                Error("Tangent attribut table impossible to be created (desactivate shaders)");
        }
        if (_binormal) {
            if(_shader->ActiveAttributTable("binormal", binormal) == error)
                Error("Binormal attribut table impossible to be created (desactivate shaders)");
        }
        if (_normal) {
            if(_shader->ActiveAttributTable("normal", normal) == error)
                Error("Normal attribut table impossible to be created (desactivate shaders)");
        }*/

    } else
        _tex->Bind();
}

void Material::Unuse() {
    if (_shader != NULL)
        _shader->Unuse();
}

void Material::Add(std::string name, const bool var) {
    uniform<bool> u;
    u.name = name;
    u.data = var;

    _uBool->push_back(u);
}

void Material::Add(std::string name, const float var) {
    uniform<float> u;
    u.name = name;
    u.data = var;

    _uFloat->push_back(u);
}

void Material::Add(std::string name, const std::string &var) {
    _uImg->resize(_uImg->size()+1);
    (*_uImg)[_uImg->size()-1].name = name;
    (*_uImg)[_uImg->size()-1].data = new NRENDERING::Image(var);
}

void Material::Add(std::string name, const glm::vec2 var) {
    uniform<glm::vec2> u;
    u.name = name;
    u.data = var;

    _uVec2->push_back(u);
}

void Material::Add(std::string name, const glm::vec3 var) {
    uniform<glm::vec3> u;
    u.name = name;
    u.data = var;

    _uVec3->push_back(u);
}

void Material::Add(std::string name, const glm::vec4 var) {
    uniform<glm::vec4> u;
    u.name = name;
    u.data = var;

    _uVec4->push_back(u);
}

void Material::Add(std::string name, const glm::mat2 var) {
    uniform<glm::mat2> u;
    u.name = name;
    u.data = var;

    _uMat2->push_back(u);
}

void Material::Add(std::string name, const glm::mat3 var) {
    uniform<glm::mat3> u;
    u.name = name;
    u.data = var;

    _uMat3->push_back(u);
}

void Material::Add(std::string name, const glm::mat4 var) {
    uniform<glm::mat4> u;
    u.name = name;
    u.data = var;

    _uMat4->push_back(u);
}


