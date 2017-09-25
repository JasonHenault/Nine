#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace glm;

// For shaders


glm::vec3 NMODELS::ComputeNormal( const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3 ) {
  glm::vec3 vec1(v1 - v2);
  glm::vec3 vec2(v1 - v3);

  glm::vec3 result(glm::cross( vec1, vec2 ));
  result = glm::normalize(result);

  return result;
}

void NMODELS::ComputeW(glm::quat *q) {    // For MD5
    float t = 1.0f - (q->x * q->x) - (q->y * q->y) - (q->z * q->z);

    if (t < 0.0f)
        q->w = 0.0f;
    else
        q->w = -sqrt(t);
}

std::vector<vec3> NMODELS::GetTangentTable(std::vector<glm::vec3> &v, std::vector<glm::vec3> &vt, std::vector<glm::vec3> &vn) {
    std::vector<vec3> table;

    for (int i = 0; i < v.size() - 2; i+=3) {
        vec3 vx0(v[i].x, vn[i].x, vn[i].y);
        vec3 vx1(v[i+1].x, vn[i+1].x, vn[i+1].y);
        vec3 vx2(v[i+2].x, vn[i+2].x, vn[i+2].y);

        vec3 vy0(v[i].y, vn[i].x, vn[i].y);
        vec3 vy1(v[i+1].y, vn[i+1].x, vn[i+1].y);
        vec3 vy2(v[i+2].y, vn[i+2].x, vn[i+2].y);

        vec3 vz0(v[i].z, vn[i].x, vn[i].y);
        vec3 vz1(v[i+1].z, vn[i+1].x, vn[i+1].y);
        vec3 vz2(v[i+2].z, vn[i+2].x, vn[i+2].y);

        vec3 abc0(cross((vx0-vx1), (vx0-vx2)));
        vec3 abc1(cross((vy0-vy1), (vy0-vy2)));
        vec3 abc2(cross((vz0-vz1), (vz0-vz2)));

        table.push_back(normalize(vec3(-abc0.y/abc0.x,-abc1.y/abc1.x, -abc2.y/abc2.x)));
        table.push_back(normalize(vec3(-abc0.y/abc0.x,-abc1.y/abc1.x, -abc2.y/abc2.x)));
        table.push_back(normalize(vec3(-abc0.y/abc0.x,-abc1.y/abc1.x, -abc2.y/abc2.x)));
    }

    table.push_back(table[0]);
    table.push_back(table[0]);
    table.push_back(table[0]);

    return table;
}

std::vector<vec3> NMODELS::GetBinormalTable(std::vector<glm::vec3> &v, std::vector<glm::vec3> &vt, std::vector<glm::vec3> &vn) {
    std::vector<vec3> table;

    for (int i = 0; i < v.size() - 2; i+=3) {
        vec3 vx0(v[i].x, vn[i].x, vn[i].y);
        vec3 vx1(v[i+1].x, vn[i+1].x, vn[i+1].y);
        vec3 vx2(v[i+2].x, vn[i+2].x, vn[i+2].y);

        vec3 vy0(v[i].y, vn[i].x, vn[i].y);
        vec3 vy1(v[i+1].y, vn[i+1].x, vn[i+1].y);
        vec3 vy2(v[i+2].y, vn[i+2].x, vn[i+2].y);

        vec3 vz0(v[i].z, vn[i].x, vn[i].y);
        vec3 vz1(v[i+1].z, vn[i+1].x, vn[i+1].y);
        vec3 vz2(v[i+2].z, vn[i+2].x, vn[i+2].y);

        vec3 abc0(cross((vx0-vx1), (vx0-vx2)));
        vec3 abc1(cross((vy0-vy1), (vy0-vy2)));
        vec3 abc2(cross((vz0-vz1), (vz0-vz2)));

        table.push_back(normalize(vec3(-abc0.z/abc0.x,-abc1.z/abc1.x, -abc2.z/abc2.x)));
        table.push_back(normalize(vec3(-abc0.z/abc0.x,-abc1.z/abc1.x, -abc2.z/abc2.x)));
        table.push_back(normalize(vec3(-abc0.z/abc0.x,-abc1.z/abc1.x, -abc2.z/abc2.x)));
    }

    table.push_back(table[0]);
    table.push_back(table[0]);
    table.push_back(table[0]);

    return table;
}

std::vector<vec3> NMODELS::GetNormalTable(std::vector<glm::vec3> &v) {
    std::vector<vec3> table;

    for (int i = 0; i < v.size() - 2; i+=3) {
        table.push_back(normalize(cross((v[i+1] - v[i]),(v[i+2] - v[i]))));
        table.push_back(normalize(cross((v[i+1] - v[i]),(v[i+2] - v[i]))));
        table.push_back(normalize(cross((v[i+1] - v[i]),(v[i+2] - v[i]))));
    }

    table.push_back(table[0]);
    table.push_back(table[0]);
    table.push_back(table[0]);

    return table;
}
