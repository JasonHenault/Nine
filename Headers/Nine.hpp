#ifndef ISLAND_HPP
#define ISLAND_HPP

#include <GLee.h>
#include <Newton.h>
#include <GLM/glm.h>

#include "Renderer.hpp"
#include "Game.hpp"
#include "User.hpp"
#include "AwoMng.hpp"
#include "ImgMng.hpp"
#include "Scene.hpp"
#include "Report.hpp"
#include "Physic.hpp"
#include "Pnj.hpp"
#include "global_parameters.hpp"

namespace Island {
    enum {error = -1, success,
          run, stop,
          shader, program, vertexShader, fragmentShader
         };

    int Init(void);
}

#endif // ISLAND_HPP
