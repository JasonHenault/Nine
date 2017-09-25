#ifdef NWM_HPP
#warning You should only include Nine.hpp to use this library
#else
#define NWM_HPP

class Nwm : public NRESOURCES::Resource {
public:
    Nwm(const std::string &path);
    virtual ~Nwm();

    void Draw();

    NewtonCollision* GetCollisionTree();
    NewtonCollision* GetConvexHull();
    glm::vec3 GetMin();
    glm::vec3 GetMax();
    NMODELS::BoundingBox GetBoundingBox();

    std::string& GetDescriptionFilePath() {   return _descriptionFilePath;   }

protected:
    int Load(const std::string &path);
    int GetVertex(std::ifstream &file, std::vector<glm::vec3> &table, const std::string type);

    std::string _descriptionFilePath;
    std::vector<Mesh> *_meshs;
};

#endif // NWM_HPP
