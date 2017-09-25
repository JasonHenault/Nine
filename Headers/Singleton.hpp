#ifdef SINGLETON_HPP
#warning You should only include Nine.hpp to use this library
#else
#define SINGLETON_HPP

template <class T>
class Singleton {
public :
    static T& Instance() {
        if (!Inst)
            Inst = new T;

        return *Inst;
    }

    static int Destroy() {
        try {
        delete Inst;
        Inst = NULL;
        } catch(...) {
            return error;
        }

        return success;
    }

protected :
    Singleton() {}
    ~Singleton() {}

private :
    static T* Inst;

    Singleton(Singleton&);
    void operator =(Singleton&);
};

template <class T> T* NPATHERNS::Singleton<T>::Inst = new T;

#endif // SINGLETON_HPP
