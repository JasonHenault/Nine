#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NSCENE;

Map::Map(const std::string &path) {
    _nwm = new NMODELS::Nwm(NTOOLS::DecodePath(path));
    _pBody = NewtonCreateBody(NSYSTEM::Physics::GetWorld(), _nwm->GetCollisionTree(), &(glm::mat4(1.f))[0][0]);
    // NewtonReleaseCollision(NSYSTEM::Physics::GetWorld(), NwmMng::Instance().GetConvexHull(_nwm, NSYSTEM::Physics::GetWorld()));

    _size = glm::distance(Min(), Max());
}

Map::~Map() {
        delete _nwm;
        delete _pBody;
}

void Map::Draw(float elapsedTime) {
    _nwm->Draw();
}

glm::vec3 Map::Min() {
    return _nwm->GetMin();
}

glm::vec3 Map::Max() {
    return _nwm->GetMax();
}
