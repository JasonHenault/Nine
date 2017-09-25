#ifndef NINE_HPP
#define NINE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <typeinfo>
#include <iterator>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <GLee.h>
#include <gl/glu.h>
#include <newton2.33\coreLibrary_200\source\newton\Newton.h>
#include <GLM/glm.hpp>
#include <GLM/ext.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Lua/lua.hpp>

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>

/*
#define __in
#define __out
#define __reserved
#include <DirectX SDK\XInput.h>

& joy en bas
*/

#include "FastAccess.hpp"

namespace Nine {
enum {error = -1, success};

namespace Tools {
std::string itos(int num);
std::string DecodePath(const std::string path);

namespace Patherns {
#include "Headers/Entity.hpp"
#include "Headers/Singleton.hpp"
}

#include "Headers/Log.hpp"

namespace Resources {
#include "Headers/ResourcesManager.hpp"
#include "Headers/Resource.hpp"
}

#include "Headers/Fonction.hpp"
#include "Headers/Functor.hpp"
#include "Headers/Commands.hpp"
#include "Headers/Shell.hpp"
#include "Headers/Script.hpp"

namespace Resources {
namespace Rendering {
#include "Headers/Image.hpp"
#include "Headers/Shader.hpp"
#include "Headers/Material.hpp"
#include "Headers/Sprite.hpp"
}

namespace Audio {
float GetVolume();
int SetVolume(float volume);

#include "Headers/Sound.hpp"
#include "Headers/Music.hpp"
}
}
}

namespace System {
const float defaultFovy = 70.f;
const float defaultDepth = 500.f;
const float defaultMaterial = 0;

struct Controls {
    bool front, back, right, left, use, run, crouch, jump, shoot, second, light;
    float antPost, levoRect, rotHoriz, rotVert;
    int x, y;
};
namespace Console {
#include "Headers/Console.hpp"
};

#include "Headers/Renderer.hpp"
#include "Headers/Physics.hpp"
//#include "Headers/Joystick.hpp"
}


namespace Tools {
namespace Resources {
namespace Rendering {
namespace Models {
    const int kMd5Version = 10;

struct face {
    glm::vec3 v[3];
    glm::vec2 vt[3];
    glm::vec3 vn[3];
    NRENDERING::Image *mat;
};

struct Mesh {
    std::string name;
    std::vector<glm::vec3> v;
    std::vector<glm::vec3> vt;
    std::vector<glm::vec3> vn;
    std::vector<unsigned int> vertIndices;
    NRENDERING::Material *material;
    std::vector<glm::vec3> tangent, binormal, normal;
};

struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
};

#include "Headers/Models.hpp"
#include "Headers/Nwm.hpp"
#include "Headers/Md5.hpp"
}
}
}
}

namespace Game {
namespace Scene {
#include "Headers/Camera.hpp"

namespace Logic {
#include "Headers/Trigger.hpp"
}

namespace Objects {
#include "Headers/Object.hpp"
#include "Headers/Weapon.hpp"
#include "Headers/Npc.hpp"
#include "Headers/User.hpp"
}

#include "Headers/Light.hpp"
#include "Headers/Map.hpp"
#include "Headers/Skybox.hpp"
#include "Headers/Scene.hpp"
}
}

/**
    Global functions
                      **/
int Init();
int Terminate();

void SetDebug(bool debug);
bool GetDebug();

void SetJoystick(bool joystick);
bool GetJoystick();

bool LoadScene(const std::string &path);
bool UnloadScene();
void Refresh(float elapsedTime);
NSCENE::Scene& GetScene();
}

#include "UnuseFastAccess.hpp"

#endif // NINE_HPP
