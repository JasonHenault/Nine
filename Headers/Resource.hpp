#ifdef RESOURCE_HPP
#warning You should only include Nine.hpp to use this library
#else
#define RESOURCE_HPP

class Resource {
public:
    Resource() : _path("unknow") { }

    virtual ~Resource() { }

    int IsExist() {
        return ResMng::Instance().IsExist(this);
    }

    template <class T>
    int CreateResource(const std::string &path) {
        _path = path;

        Package(path, typeid(T).name())

        if (ResMng::Instance().Register(path, this) == success) {
            Load(path);
            return success;
        }

        *(static_cast<const T*>(this)) = *(ResMng::Instance().Get<T>(path));
        return error;
    }

    const std::string& GetPath() { return _path; }

    virtual int Load(const std::string &path) = 0;

protected:
    std::string _path;
};

#endif // RESOURCE_HPP


