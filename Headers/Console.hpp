#ifdef CONSOLE_HPP
#warning You should only include Nine.hpp to use this library
#else
#define CONSOLE_HPP

/*class Console : public NPATHERNS::Singleton<Console> {
    friend class NPATHERNS::Singleton<Console>;

    void RegisterCommand(const std::string& name, const Console::Functor& function);
    void SendChar(char c);
    void Enable(bool enabled);

private :
    void ProcessCurrent();

    typedef std::map<std::string, Console::Functor> CommandTable;

    CommandTable _commands;
    std::string _current;
    bool _enabled;
}; */

#endif // CONSOLE_HPP
