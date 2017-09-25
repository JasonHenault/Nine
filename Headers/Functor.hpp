#ifdef FUNCTOR_HPP
#warning You should only include Nine.hpp to use this library
#else
#define FUNCTOR_HPP

class Functor {
public :
    Functor(Function* func = NULL) : _function(func) {}

    std::string operator ()(const std::string& params = "") const {
        if (!_function)
            return "Tentative d'effectuer un appel à une fonction nulle via un foncteur";

        return _function->Execute(params);
    }

private :
    Function* _function;
};

#endif // FUNCTOR_HPP
