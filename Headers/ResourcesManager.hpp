#ifdef RESOURCESMANAGER_HPP
#warning You should only include Nine.hpp to use this library
#else
#define RESOURCESMANAGER_HPP

class Resource; // Prototype for template function

class ResMng : public NPATHERNS::Singleton<ResMng> {
    friend class NPATHERNS::Singleton<ResMng>;

protected:
    ResMng();
    ~ResMng();

public:
    int Register(const std::string &path, const Resource *res);
    int Unregister(const std::string &path);

    int IsExist(Resource* res) {
        for(std::map<const std::string, const Resource*>::iterator it = _res.begin(); it != _res.end(); it++)
            if (it->second == res)
                return success;

        return error;
    }

    template <class T>
    const T* Get(const std::string &path) {
        static std::map<const std::string, const Resource*>::iterator it = _res.find(path);

        if (it != _res.end())
            return static_cast<const T*>(it->second);

        return NULL;
    }

protected:
    std::map<const std::string, int> _refCount;
    std::map<const std::string, const Resource*> _res;
};

#endif // RESOURCESMANAGER_HPP

