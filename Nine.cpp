#include "Nine.hpp"
#include "FastAccess.hpp"


/** Nine's internal functions declaration **/
namespace _int9func {
    int apow(const int &x, int y);
    std::string digit2str(short digit);

    NSCENE::Scene *_scene;
    bool _debug, _joystick;
}

using namespace _int9func;

/** Nine's general funtions **/

int Nine::Init() {
    _scene = new NSCENE::Scene;
    _debug = false;

    int returnInit = NSYSTEM::Physics::Init();
    NOBJECTS::User::Instance().Init();

    /*if (test conditionnel)
        returnInit = error;
    else
        returnInit |= success;*/

    return returnInit;
}

int Nine::Terminate() {
    UnloadScene();
    return NSYSTEM::Renderer::Destroy() | NSYSTEM::Physics::Destroy() | NTOOLS::Log::Destroy();
}


void Nine::SetDebug(bool debug) {
    _debug = debug;
}

bool Nine::GetDebug() {
    return _debug;
}


void Nine::SetJoystick(bool joystick) {
    _joystick = joystick;
}

bool Nine::GetJoystick() {
    return _joystick;
}


bool Nine::LoadScene(const std::string &path) {
    UnloadScene();

    _scene = new NSCENE::Scene();

    if (_scene->Load(path) == success) {
        NSYSTEM::Physics::SetGravity(NSYSTEM::Physics::run);
        return success;
    } else {
        UnloadScene();
        return error;
    }
}

bool Nine::UnloadScene() {
    NSYSTEM::Physics::SetGravity(NSYSTEM::Physics::stop);

    if (_scene != NULL) {
        delete _scene;
        _scene = new NSCENE::Scene;
        return success;
    } else {
        _scene = new NSCENE::Scene;
        return error;
    }
}

void Nine::Refresh(float elapsedTime) {
    if ( _scene != NULL)
        _scene->Refresh(elapsedTime);
}

NSCENE::Scene& Nine::GetScene() {
    return *_scene;
}

std::string Nine::Tools::itos(int num) {      // Convert int to string
    int b = 0, c = 1, n = 0;
    std::string result;

    while(apow(10,n) < num) n++;

    if(n != 0) n--;
    b = apow(10,n);

    while((b*c) < num) c++;

    if(n != 0) c--;
    num -= (b*c);
    result += digit2str(c);
    if(n != 0) result += itos(num);

    return result;
}

std::string Nine::Tools::DecodePath(const std::string path) {
    std::string retString = path;

    if (path.find("System::") != std::string::npos)
        return retString.replace(retString.find("System::"), 8, "System/");
    else
        return retString;
}


float Nine::Tools::Resources::Audio::GetVolume () {
    return sf::Listener::getGlobalVolume();
}

int Nine::Tools::Resources::Audio::SetVolume (float volume) {
    if (volume >= 0.f && volume <= 100.f) {
        sf::Listener::setGlobalVolume(volume);
        return Nine::success;
    } else
        return Nine::error;
}


/**
    Nine's internal functions and vars
                                        **/
namespace _int9func {
int apow(const int &x, int y) {
    /* Raise x to the power of y */
    int result = 1;
    if(y < 0)
        return apow(1/x, -y);
    for(int i = y; i > 0; i--)
        result *= x;
    return result;
}

std::string digit2str(short digit) {
    /* Convert a digit to a string */
    std::string tmp;
    if(digit >= 0 && digit <= 9) {
        tmp = digit + '0';
        return tmp;
    }
    return "";
}
}
