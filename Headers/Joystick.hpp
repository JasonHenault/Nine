#ifdef JOYSTICK_HPP
#warning You should only include Nine.hpp to use this library
#else
#define JOYSTICK_HPP

class Joystick : public NPATHERNS::Singleton<Joystick> {
    friend class NPATHERNS::Singleton<Joystick>;

public:
    Joystick() {};
    virtual ~Joystick() {};

    bool IsConnected() {
        ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

        DWORD Result = XInputGetState(0, &_controllerState);

        if(Result == ERROR_SUCCESS)
            return true;

        return false;
    }

    void SetLeftVibrator(float percent) {
        _leftVibrator = percent;
    }

    void SetRightVibrator(float percent) {
        _rightVibrator = percent;
    }

    void Vibrate() {
        XINPUT_VIBRATION Vibration;

        ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

        Vibration.wLeftMotorSpeed = _leftVibrator;
        Vibration.wRightMotorSpeed = _rightVibrator;

        XInputSetState(0, &Vibration);     //XInputSetState(_controllerNum, &Vibration);
        XInputSetState(1, &Vibration);     //XInputSetState(_controllerNum, &Vibration);
    }

    void Vibrate(float left, float right) {
        _leftVibrator = left;
        _rightVibrator = right;
        Vibrate();
    }

private:
    int _controllerNum;
    float _leftVibrator, _rightVibrator;
    XINPUT_STATE _controllerState;
};

#endif // JOYSTICK_HPP

