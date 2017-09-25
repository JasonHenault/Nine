#ifdef MODELS_HPP
#warning You should only include Nine.hpp to use this library
#else
#define MODELS_HPP

// Usefull tools for nwm, md5 ...


// For shaders
glm::vec3 ComputeNormal( const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3 );
void ComputeW(glm::quat *q);    // For MD5

std::vector<glm::vec3> GetTangentTable(std::vector<glm::vec3> &v, std::vector<glm::vec3> &vt, std::vector<glm::vec3> &vn);
std::vector<glm::vec3> GetBinormalTable(std::vector<glm::vec3> &v, std::vector<glm::vec3> &vt, std::vector<glm::vec3> &vn);
std::vector<glm::vec3> GetNormalTable(std::vector<glm::vec3> &v);

#endif // MODELS_HPP
