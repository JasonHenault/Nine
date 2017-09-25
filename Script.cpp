#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NTOOLS;
using namespace std;

Script::Script() : _path("unknow") {
    _state = luaL_newstate();
    luaL_openlibs(_state);

    NTOOLS::Commands::LoadCommands(_state);
}

Script::Script(const string &path) {
    _state = luaL_newstate();
    luaL_openlibs(_state);

    NTOOLS::Commands::LoadCommands(_state);

    _path = NTOOLS::DecodePath(path);
}

int Script::Execute() {
    if (_path == "unknow")
        return error;

    if (luaL_dofile(_state, _path.c_str())!=0) {
        Error("Script not loaded : " + _path + "\n                    Lua's error : " + lua_tostring(_state,-1))
        return error;
    }

    return success;
}

int Script::Execute(const string &path) {
    _path = NTOOLS::DecodePath(path);

    if (luaL_dofile(_state, _path.c_str())!=0) {
        Error(static_cast<string>("Script not loaded : ") + _path + "\n                    Lua's error : " + lua_tostring(_state,-1))
        return error;
    }

    return success;
}

double Script::GetNumber(const string &varName) {
    lua_settop(_state, 0);
    lua_getglobal(_state, varName.c_str());

    double ret = error;
    if (lua_isnumber(_state, 1))
        ret = lua_tonumber(_state, 1);
    else
        Warning(static_cast<string>("Error in script : ") + _path + static_cast<string>("\n                  ") + varName + "(double) not exist")

    lua_pop(_state, 1);
    return ret;
}

string Script::GetString (const string &varName) {
    lua_settop(_state, 0);
    lua_getglobal(_state, varName.c_str());

    string ret = "error";
    if (lua_isstring(_state, 1))
        ret = lua_tostring(_state, 1);
    else
        Warning(static_cast<string>("Error in script : ") + _path + static_cast<string>("\n                  ") + varName + "(string) not exist")

    lua_pop(_state, 1);
    return ret;
}

/*double* Script::GetTable(const string &varName) { // incomplete function
    lua_settop(_state, 0);
    lua_getglobal(_state, varName.c_str());

    if (lua_istable(_state, 1)) {
        lua_pushnumber(_state, 2);
        lua_gettable(_state, -2);

        if (lua_isnumber(_state, -1)) {
            lua_tonumber(_state, -1);
        } else
            Error () cout << "Error in script : variable (table) " << varName << " contain char" << endl;

        lua_pop(_state, 2);
    } else {
        Error () cout << "Error in script : variable (table) " << varName << " not exist" << endl;
        lua_pop(_state, 1);
        double ret = error;
        return &ret;
    }
}*/

void Script::PushNumber(lua_Number num) {
    lua_pushnumber(_state, num);
}

void Script::AddFunction(const string &functionName, int (*function)(lua_State* L)) {
    lua_register(_state, functionName.c_str(), function);
}

int Script::ActiveFunction(const string &func) {
    lua_getglobal(_state, func.c_str());

    if (lua_isfunction(_state,-1))
        return success;
    else
        return error;
}

float Script::CallFunction(int numArg, int numReturn) {
    lua_call(_state, numArg, numReturn);
    return static_cast<float>(lua_tonumber(_state,-1));
}
