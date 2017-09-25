#ifdef COMMANDS_HPP
#warning You should only include Nine.hpp to use this library
#else
#define COMMANDS_HPP

namespace Commands {
void LoadCommands(lua_State *state);

// Commands
int SendInfo(lua_State* L);
int SendError(lua_State* L);

int PlaySound(lua_State* L);
int PlayMusic(lua_State* L);

int SetTrigger(lua_State* L);

int CreateSprite(lua_State* L);
int CreateObject(lua_State* L);
int CreateObjectSprite(lua_State* L);
int CreateNpc(lua_State* L);
int CreateLight(lua_State* L);

int DeleteEntity(lua_State* L);

int Attach(lua_State* L);

int GetEntityPosition(lua_State* L);

int SetUserPosition(lua_State* L);
int SetEntityPosition(lua_State* L);
int SetObjectMass(lua_State* L);

int Impulse(lua_State* L);

int user(lua_State* L);
int Raycast(lua_State* L);
int RaycastObj(lua_State* L);
///////////
}

#endif // COMMANDS_HPP
