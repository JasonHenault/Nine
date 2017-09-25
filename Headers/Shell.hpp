#ifdef SHELL_HPP
#warning You should only include Nine.hpp to use this library
#else
#define SHELL_HPP

class Shell : public NPATHERNS::Singleton<Shell> {
    public:
    void Show();
    void SendCaracter(char c);
    void Execute();

    protected:
    Shell();
    virtual ~Shell();
};

#endif // SHELL_HPP

