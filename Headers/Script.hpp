#ifdef SCRIPT_HPP
#warning You should only include Nine.hpp to use this library
#else
#define SCRIPT_HPP

class Script {
    public:
    Script();
    Script(const std::string &path);

    int Execute();
    int Execute(const std::string &path);

    double GetNumber(const std::string &varName);
    std::string GetString(const std::string &varName);
    //double* GetTable(const std::string &varName);

    void PushNumber(lua_Number num);

    void AddFunction(const std::string &functionName, int (*function)(lua_State* L));
    int ActiveFunction(const std::string &func);
    float CallFunction(int numArg, int numReturn);

    protected:
    std::string _path;
    lua_State *_state;
};

#endif // SCRIPT_HPP
