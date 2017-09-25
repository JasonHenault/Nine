#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NRESOURCES;

Resource::Resource() : _path("unknow") {
}

Resource::~Resource() {
    ResMng::Unregister(_path);
    std::cout << "C'est coooooooooooooool" << std::endl;
}

template <class T>
int Resource::CreateResource(const std::string &path, T *res) {
    _path = path;

    if (ResMng::Register(path, this) == success) {
        Load(path);
        return success;
    }

    *res = ResMng::Get<T>(path);
    return error;
}

std::string Resource::GetPath(void) {
    return _path;
}

