#ifdef TNB_HPP
#warning You should only include Nine.hpp to use this library
#else
#define TNB_HPP

#define TNB_Header "attribute vec3 vTangent;\n\
                vec3 normal = normalize(gl_NormalMatrix * gl_Normal);\n\
                vec3 tangent = normalize(gl_NormalMatrix * vTangent);\n\
                vec3 binormal = cross(normal, tangent);\n\n"

#endif // TNB_HPP
