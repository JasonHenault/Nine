#ifdef FUNCTION_HPP
#warning You should only include Nine.hpp to use this library
#else
#define FUNCTION_HPP

class Function {
public :
    virtual ~Function() {}

    virtual std::string Execute(const std::string& params) = 0;
};

#endif // FUNCTION_HPP
