#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NTOOLS;
using namespace NSYSTEM;
using namespace NSCENE;
using namespace NPATHERNS;
using namespace NOBJECTS;
using namespace NRENDERING;

void Commands::LoadCommands(lua_State *state) {
    lua_register(state,"Info",Commands::SendInfo);
    lua_register(state,"Error",Commands::SendError);
    lua_register(state,"PlaySound",Commands::PlaySound);
    lua_register(state,"PlayMusic",Commands::PlayMusic);
    lua_register(state,"SetTrigger",Commands::SetTrigger);
    lua_register(state,"CreateSprite",Commands::CreateSprite);
    lua_register(state,"CreateObject",Commands::CreateObject);
    lua_register(state,"CreateObjectSprite",Commands::CreateObjectSprite);
    lua_register(state,"CreateNpc",Commands::CreateNpc);
    lua_register(state,"CreateLight",Commands::CreateLight);
    lua_register(state,"DeleteEntity",Commands::DeleteEntity);
    lua_register(state,"Attach",Commands::DeleteEntity);
    lua_register(state,"GetEntityPosition",Commands::GetEntityPosition);
    lua_register(state,"SetUserPosition",Commands::SetUserPosition);
    lua_register(state,"SetEntityPosition",Commands::SetEntityPosition);
    lua_register(state,"SetObjectMass",Commands::SetObjectMass);
    lua_register(state,"Impulse",Commands::Impulse);
    lua_register(state,"user",Commands::user);
    lua_register(state,"Raycast",Commands::Raycast);
    lua_register(state,"RaycastObj",Commands::RaycastObj);
}

int Commands::SendInfo(lua_State* L) {  // Info()
    if (lua_isstring(L, 1))
        Info(lua_tostring(L, 1))
    return 0;
}

int Commands::SendError(lua_State* L) {  // Error()
    if (lua_isstring(L, 1))
        Error(lua_tostring(L, 1))
    return 0;
}

int Commands::PlaySound(lua_State* L) {
    if (lua_isstring(L, 1)) {
        NAUDIO::Sound *snd = new NAUDIO::Sound(lua_tostring(L, 1));
        snd->play();
        snd = NULL;
        Error(lua_tostring(L, 1))
    }
    return 0;
}

int Commands::PlayMusic(lua_State* L) {
    static NAUDIO::Music *music = NULL;
    if (lua_isstring(L, 1)) {
        if (music != NULL)
            delete music;
        music = new NAUDIO::Music(lua_tostring(L, 1));
        music->play();
    }
    return 0;
}

int Commands::SetTrigger(lua_State* L) {
    glm::vec3 downCorner = glm::vec3(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
    glm::vec3 upCorner = glm::vec3(lua_tonumber(L, 4), lua_tonumber(L, 5), lua_tonumber(L, 6));

    if (lua_isstring(L, 8)) {
        if (lua_isnumber(L,9))
            GetScene().CreateTrigger(downCorner, upCorner, lua_tostring(L, 7), lua_tostring(L, 8), lua_tonumber(L, 9));
        GetScene().CreateTrigger(downCorner, upCorner, lua_tostring(L, 7), lua_tostring(L, 8));
    } else
    GetScene().CreateTrigger(downCorner, upCorner, lua_tostring(L, 7));

    return 0;
}

int Commands::CreateSprite(lua_State* L) {
    Sprite *spr = GetScene().CreateSprite(lua_tonumber(L, 1), glm::vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)), lua_tostring(L, 5));

    lua_pushnumber(L,spr->GetId());
    return 1;
}

int Commands::CreateObject(lua_State* L) {
    glm::mat4 mat;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            mat[j][i] = lua_tonumber(L, 2+i*4+j);

    Object *obj = GetScene().CreateObject(lua_tostring(L, 1), mat, lua_tonumber(L, 18));

    return obj->GetId();
}

int Commands::CreateObjectSprite(lua_State* L) {
    Object *obj = GetScene().CreateObject(lua_tostring(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), glm::vec3(lua_tonumber(L, 4), lua_tonumber(L, 5), lua_tonumber(L, 6)), lua_tonumber(L, 7));

    lua_pushnumber(L,obj->GetId());
    return 1;
}

int Commands::CreateNpc(lua_State* L) {
    glm::mat4 mat;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            mat[j][i] = lua_tonumber(L, 2+i*4+j);

    GetScene().CreateNpc(lua_tostring(L, 1), mat, lua_tonumber(L, 18));

    return 0;
}

int Commands::CreateLight(lua_State* L) {
    Light *light;
    if (lua_isnumber(L, 4))
        light = GetScene().CreateLight(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5), lua_tonumber(L, 6));
    else
        light = GetScene().CreateLight(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));

    lua_pushnumber(L,light->GetId());
    return 1;
}

int Commands::DeleteEntity(lua_State* L) {
    Entity *entity = GetScene().GetEntity(lua_tonumber(L,1));
    if (entity != NULL) {
        delete entity;
        return 0;
    } else
        return 1;
}

int Commands::Attach(lua_State* L) {
    if (!lua_isnumber(L,1) || !lua_isnumber(L, 2))
        return 0;

    GetScene().Attach(lua_isnumber(L,1), lua_isnumber(L, 2));
}

int Commands::GetEntityPosition(lua_State* L) {
    if (!lua_isnumber(L,1) || !lua_isstring(L, 2))
        return 0;

    Entity* entity = GetScene().GetEntity(lua_tonumber(L,1));
    if (entity == NULL)
        return 0;

    if (lua_tostring(L, 2) == "x")
        lua_pushnumber(L, entity->GetPosition().x);
    else if (lua_tostring(L, 2) == "y")
        lua_pushnumber(L, entity->GetPosition().y);
    else if (lua_tostring(L, 2) == "z")
        lua_pushnumber(L, entity->GetPosition().z);
    else
        return 0;

    return 1;
}

int Commands::SetUserPosition(lua_State* L) {
    User::Instance().SetPosition(glm::vec3(lua_tonumber(L,1), lua_tonumber(L,2), lua_tonumber(L,3)));
    return 0;
}

int Commands::SetEntityPosition(lua_State* L) {
    int id = lua_tonumber(L,4);

    if (id == 0)
        User::Instance().SetPosition(glm::vec3(lua_tonumber(L,1), lua_tonumber(L,2), lua_tonumber(L,3)));
    else {
        Entity* entity = GetScene().GetEntity(id);
        if (entity == NULL)
            return 0;
        entity->SetPosition(glm::vec3(lua_tonumber(L,1), lua_tonumber(L,2), lua_tonumber(L,3)));
    }

    return 0;
}

int Commands::SetObjectMass(lua_State* L) {
    if (lua_isnumber(L,1) && lua_isnumber(L,2)) {
        Entity* entity = GetScene().GetEntity(lua_isnumber(L,1));
        if (entity != NULL)
            if(entity->GetType() == "Object" || entity->GetType() == "Npc")
                static_cast<Object*>(entity)->SetMass(lua_isnumber(L,2));
            else
                return 1;
        else
            return 1;

        return 0;
    }

    return 1;
}

int Commands::Impulse(lua_State* L) {
    if (lua_isnumber(L,1) && lua_isnumber(L,2)) {
        Entity* entity = GetScene().GetEntity(lua_isnumber(L,1));
        if (entity != NULL)
            if(entity->GetType() == "Object" || entity->GetType() == "Npc")
                static_cast<Object*>(entity)->Impulse(glm::vec3(lua_isnumber(L,2), lua_isnumber(L,3), lua_isnumber(L,4)));
    }

    return 0;
}

int Commands::user(lua_State* L) {
    if (!lua_isstring(L, 1))
        return 0;

    if (lua_tostring(L, 1) == "x")
        lua_pushnumber(L, User::Instance().GetPosition().x);
    else if (lua_tostring(L, 1) == "y")
        lua_pushnumber(L, User::Instance().GetPosition().y);
    else if (lua_tostring(L, 1) == "z")
        lua_pushnumber(L, User::Instance().GetPosition().z);
    else
        return 0;

    return 1;
}

int Commands::Raycast(lua_State* L) {
    Entity* entity = GetScene().Raycast(User::Instance().GetPosition(), NSCENE::Camera::GetTarget());

    if (entity != NULL) {
        lua_pushnumber(L, entity->GetId());
        return 1;
    }

    return 0;
}

int Commands::RaycastObj(lua_State* L) {
    Entity* entity = GetScene().Raycast(User::Instance().GetPosition(), NSCENE::Camera::GetTarget());

    if (entity != NULL)
        if(entity->GetType() == "Object" || entity->GetType() == "Npc")
            return static_cast<Object*>(entity)->GetId();
    else
        return 0;
}
