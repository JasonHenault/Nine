#ifdef LOG_HPP
#warning You should only include Nine.hpp to use this library
#else
#define LOG_HPP

#define Info(info) NTOOLS::Log::Instance().RegisterInfo(info);
#define Warning(warn) NTOOLS::Log::Instance().RegisterWarning(warn, __FILE__, __LINE__);
#define Error(error) NTOOLS::Log::Instance().RegisterError(error, __FILE__, __LINE__);
#define Package(path, type) NTOOLS::Log::Instance().RegisterPackage(path, type);

class Log : public NPATHERNS::Singleton<Log> {
    friend class NPATHERNS::Singleton<Log>;

    struct WarnAndError {
        int line;
        std::string mes, file;
    };

    struct Package {
        std::string path, type;
    };

private:
    Log();
    ~Log();

public:
    void RegisterInfo (const std::string &info);
    void RegisterWarning (const std::string &warn, std::string file, int line);
    void RegisterError (const std::string &error, std::string file, int line);
    void RegisterPackage (const std::string &path, const std::string type);

    void SaveReport();

private:
    std::vector<std::string> _info;
    std::vector<WarnAndError> _warning, _error;
    std::vector<Package> _package;
};

#endif // LOG_HPP
