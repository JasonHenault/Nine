#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NRENDERING;
using namespace std;

Shader::Shader(const string &path) {
    if (GLEE_ARB_shader_objects == GL_FALSE)
        Warning(static_cast<string>("Impossible to load : \"") + NTOOLS::DecodePath(path) + "\n -> Shaders not supported")
    else {
        GLuint shader = NULL;

        if (path.find(".vert") != string::npos)
            shader = CreateVertexShader(path);
        else if (path.find(".frag") != string::npos)
            shader = CreateFragmentShader(path);
        else
            Error("Wrong type of shader (need to be .frag or .vert .")

        _program = glCreateProgram();

        if (_program != NULL) {
            glAttachShader(_program, shader);
            glLinkProgram(_program);
        }

        DebugCompile(_program, program);
    }
}

Shader::Shader(const string &path1, const string &path2) {
    if (GLEE_ARB_shader_objects == GL_FALSE)
        Warning(static_cast<string>("Impossible to load : \"") + path1 + "\" and \"" + path2 + "\" -> Shaders not supported")
    else {
        GLuint vertShader = NULL, fragShader = NULL;

        if (path1.find(".frag") != string::npos)
            fragShader = CreateFragmentShader(path1);
        else if (path1.find(".vert") != string::npos)
            vertShader = CreateVertexShader(path1);
        else
            Error("Wrong type of shader (need to be .frag or .vert .")

        if (path2.find(".frag") != string::npos && fragShader == NULL)
            fragShader = CreateFragmentShader(path2);
        else if (path2.find(".vert") != string::npos && vertShader == NULL)
            vertShader = CreateVertexShader(path2);
        else
            Error("Wrong type of shader (need to be .frag or .vert .")

        _program = glCreateProgram();

        if (_program != NULL) {
            glAttachShader(_program, vertShader);
            glAttachShader(_program, fragShader);
            glLinkProgram(_program);
        }

        DebugCompile(_program, program);
    }
}

Shader::~Shader() {
    if (glIsProgram(_program))
        glDeleteProgram(_program);
}

GLuint Shader::CreateFragmentShader(const string &path) {
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

    if (frag == 0)
        return error;

    string src = LoadShader(NTOOLS::DecodePath(path));
    const GLchar *srcPtr = static_cast<const GLchar*>(src.c_str());
    glShaderSource(frag, 1, &srcPtr, NULL);
    glCompileShader(frag);

    DebugCompile(frag, shader, path);

    return frag;
}

GLuint Shader::CreateVertexShader(const string &path) {
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);

    if (vert == 0)
        return error;

    string src = LoadShader(NTOOLS::DecodePath(path));
    const GLchar *srcPtr = static_cast<const GLchar*>(src.c_str());
    glShaderSource(vert, 1, &srcPtr, NULL);
    glCompileShader(vert);

    DebugCompile(vert, shader, path);

    return vert;
}

int Shader::DeleteShader(GLuint &shader) {
    if (glIsShader(shader) == GL_TRUE) {
        glDeleteShader(shader);
        return success;
    } else
        return error;
}

int Shader::Use () {
    if (glIsProgram(_program) == GL_TRUE) {
        glUseProgram(_program);
        return success;
    } else
        return error;
}

int Shader::Unuse () {
    for (vector<int>::iterator it = _attTable.begin(); it != _attTable.end(); it++)
        glDisableVertexAttribArray(*it);

    for (int i = 0; i < _attTable.size(); i++)
        _attTable.pop_back();

    glUseProgram(0);

    return success;
}

string Shader::LoadShader(const string &path) {
    ifstream file(path.c_str());

    if (!file.is_open()) {
        Error(static_cast<string>("Impossible to open : \"") + path + "\".")
        return NULL;
    }

    char c;
    string src;

    do {
        file.get(c);
        src += c;
    } while (!file.eof());

    return src;
}

int Shader::DebugCompile(GLuint shaderOrProgram, int type, const string &path) {
    GLint compileStatus;

    if (type == shader)
        glGetShaderiv(shaderOrProgram, GL_COMPILE_STATUS, &compileStatus);
    else if (type == program)
        glGetProgramiv(shaderOrProgram, GL_LINK_STATUS, &compileStatus);
    else {
        Error("Shader's loader internal error")
        return error;
    }

    if (compileStatus != GL_TRUE) {
        GLint logSize = 0;

        if (type == shader)
            glGetShaderiv(shaderOrProgram, GL_INFO_LOG_LENGTH, &logSize);
        else if (type == program)
            glGetProgramiv(shaderOrProgram, GL_INFO_LOG_LENGTH, &logSize);

        GLchar* log = new GLchar[logSize / sizeof(GLchar)];

        if (type == shader)
            glGetShaderInfoLog(shaderOrProgram, logSize, &logSize, log);
        else if (type == program)
            glGetProgramInfoLog(shaderOrProgram, logSize, &logSize, log);

        if (logSize > 0 && (path != "" || path != "undefined"))
            Error(static_cast<string>("Impossible to compile : \"") + path + "\" -> " + log + ".")
        else if (type == shader)
            Error(static_cast<string>("Unknow problem occured during shader's compilation : \"") + path + "\".")
        else
            Error(static_cast<string>("Unknow problem occured during program's compilation : \"") + path + "\".")
        return error;

    } else
        return success;

}

/*
    All types of uniforms
*/

int Shader::SetParameter(const string &var, bool param) {
    int id = glGetUniformLocation(_program, var.c_str());

    if(id == error)
        return error;

    glUniform1i(id, param);

    return success;
}

int Shader::SetParameter(const string &var, int param) {
    int id = glGetUniformLocation(_program, var.c_str());

    if(id == error)
        return error;

    glUniform1i(id, param);

    return success;
}

int Shader::SetParameter(const string &var, float param) {
    int id = glGetUniformLocation(_program, var.c_str());

    if(id == error)
        return error;

    glUniform1f(id, param);

    return success;
}

int Shader::SetParameter(const string &var, NRENDERING::Image* param, const int texNb) {
    int id = glGetUniformLocation(_program, var.c_str());

    if(id == error)
        return error;

    param->Active(texNb);
    glUniform1i(id, texNb);

    return success;
}

int Shader::SetParameter(const string &var, glm::vec2 param) {
    int id = glGetUniformLocation(_program, var.c_str());

    if(id == error)
        return error;

    glUniform2f(id, param.x, param.y);

    return success;
}

int Shader::SetParameter(const string &var, glm::vec3 param) {
    int id = glGetUniformLocation(_program, var.c_str());

    if(id == error)
        return error;

    glUniform3f(id, param.x, param.y, param.z);

    return success;
}

int Shader::SetParameter(const string &var, glm::vec4 param) {
    int id = glGetUniformLocation(_program, var.c_str());

    if(id == error)
        return error;

    glUniform4f(id, param.x, param.y, param.z, param.w);

    return success;
}

int Shader::SetParameter(const string &var, glm::mat2 param) {
    int id = glGetUniformLocation(_program, var.c_str());

    if(id == error)
        return error;

    glUniformMatrix2fv(id, 1, 0, &param[0][0]);

    return success;
}

int Shader::SetParameter(const string &var, glm::mat3 param) {
    int id = glGetUniformLocation(_program, var.c_str());

    if(id == error)
        return error;

    glUniformMatrix3fv(id, 1, 0, &param[0][0]);

    return success;
}

int Shader::SetParameter(const string &var, glm::mat4 param) {
    int id = glGetUniformLocation(_program, var.c_str());

    if(id == error)
        return error;

    glUniformMatrix4fv(id, 1, 0, &param[0][0]);

    return success;
}

// All type of attribut

int Shader::SetAttribut(const string &var, bool param) {
    int id = glGetAttribLocation(_program, var.c_str());

    if(id == error)
        return error;

    glVertexAttrib1d(id, param);

    return success;
}

int Shader::SetAttribut(const string &var, int param) {
    int id = glGetAttribLocation(_program, var.c_str());

    if(id == error)
        return error;

    glVertexAttrib1d(id, param);

    return success;
}

int Shader::SetAttribut(const string &var, float param) {
    int id = glGetAttribLocation(_program, var.c_str());

    if(id == error)
        return error;

    glVertexAttrib1f(id, param);

    return success;
}

int Shader::SetAttribut(const string &var, glm::vec2 param) {
    int id = glGetAttribLocation(_program, var.c_str());

    if(id == error)
        return error;

    glVertexAttrib2f(id, param.x, param.y);

    return success;
}

int Shader::SetAttribut(const string &var, glm::vec3 param) {
    int id = glGetAttribLocation(_program, var.c_str());

    if(id == error)
        return error;

    glVertexAttrib3f(id, param.x, param.y, param.z);

    return success;
}

int Shader::SetAttribut(const string &var, glm::vec4 param) {
    int id = glGetAttribLocation(_program, var.c_str());

    if(id == error)
        return error;

    glVertexAttrib4f(id, param.x, param.y, param.z, param.w);

    return success;
}

int Shader::ActiveAttributTable(const string &var, const vector<glm::vec3> &varArray) {
    int attLoc = glGetAttribLocation(_program, var.c_str());

     if(attLoc != -1) {
        _attTable.push_back(attLoc);
		glEnableVertexAttribArray(attLoc);
		glVertexAttribPointer(attLoc, 3, GL_FLOAT, GL_FALSE, 0, &varArray[0].x);

		return success;
	  }

    return error;
}
