#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NRESOURCES;
using namespace std;

ResMng::ResMng() {
}

ResMng::~ResMng() {
    if (_res.size() != 0) {
        std::string warn = "Some resources are not clearely deleted : ";
        warn += "\n";
        for (map<const string, const Resource*>::iterator it = _res.begin(); it != _res.end(); it++) {
            warn += "  - " + it->first + "\n";
            delete it->second;
        }

        Warning(warn)
    }
}

int ResMng::Register(const string &path, const Resource *res) {
    if (_res.find(path) == _res.end()) {
        _refCount.insert(pair<const string, int>(path, 1));
        _res.insert(pair<const string, const Resource*>(path, res));
        return success;
    }

    _refCount[path]++;
    return error;
}

int ResMng::Unregister(const string &path) {
    static map<const string, const Resource*>::iterator it = _res.find(path);

    if (it != _res.end()) {
        if (_refCount[path] > 1) {
            _res.erase(it);
            return success;
        }

        _refCount[path]--;
        return error;
    }

    return error;
}
