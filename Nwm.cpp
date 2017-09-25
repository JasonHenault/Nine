#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NMODELS;

Nwm::Nwm(const std::string &path) : _descriptionFilePath("") {
    _meshs = new std::vector<Mesh>;

    CreateResource<Nwm>(NTOOLS::DecodePath(path));
}

Nwm::~Nwm() {
    if (NRESOURCES::ResMng::Instance().Unregister(_path) == success)
        delete _meshs;
}

int Nwm::Load(const std::string &path) {
    std::ifstream file(NTOOLS::DecodePath(path).c_str());

    if (!file.is_open()) {
        Error("Impossible to open : \"" + path + "\".")
        return error;
    }

    std::vector<glm::vec3> v, vt, vn;

    try {
        GetVertex(file, v, "v");
        GetVertex(file, vt, "vt");
        GetVertex(file, vn, "vn");
    } catch (...) {
        Error("Vertex of awo cannot be loaded in \"" + path + "\"")
    }

    std::string token;
    bool error = false, invalid = false;

    _meshs->resize(1);
    Mesh *currentMesh = &((*_meshs)[0]);

    currentMesh->name = "default";

    do {
        file >> token;

        if (token == "#")
            getline(file, token);
        else if (token == "g") {
            file >> token;

            if (currentMesh->name == "default")
                currentMesh->name = token;
            else {
                if (currentMesh->material->GetShader() != NULL) {
                    currentMesh->tangent = GetTangentTable(v, vt, vn);
                    currentMesh->binormal = GetBinormalTable(v, vt, vn);
                    currentMesh->normal = GetNormalTable(v);
                }

                _meshs->resize(_meshs->size() + 1);
                currentMesh = &((*_meshs)[_meshs->size() - 1]);
                currentMesh->name = token;
            }
        } else if (token == "material") {
            char c = 0;
            file.get(c);

            while (c != '\"' && c != '\n') {
                file.get(c);
            }

            file.get(c);

            std::string path;

            while (c != '\"' && c != '\n') {
                path += c;
                file.get(c);
            }

            currentMesh->material = new NRENDERING::Material(path);
        } else if (token == "descriptionFile") {
            char c = 0;
            file.get(c);

            while (c != '\"' && c != '\n') {
                file.get(c);
            }

            file.get(c);

            while (c != '\"' && c != '\n') {
                _descriptionFilePath += c;
                file.get(c);
            }
        } else if (token == "f") {
            int pos = 1, add = 0;
            char c = 0, cBack = 0;
            NMODELS::face fTemp;
            file.get(c);

            do {
                file.get(c);

                if (c >= '0' && c <= '9') {
                    add *= 10;
                    add += atoi(&c);
                } else if (c == '/' || c == ' ' || c == '\n') {
                    if (cBack != '/')
                        switch (pos) {
                        case 1: { // v
                            currentMesh->v.push_back(v[add - 1]);
                            currentMesh->vertIndices.push_back(currentMesh->vertIndices.size());
                        }
                        break;
                        case 2: // vt
                            currentMesh->vt.push_back(vt[add - 1]);
                            break;
                        case 3: // vn
                            currentMesh->vn.push_back(vn[add - 1]);
                            break;
                        default:
                            Error("Quad vertex not supported, loading aborded of file : \"" + path + "\"")
                            invalid = true;
                            error = true;
                            break;
                        };

                    add = 0;
                    if (c == ' ')
                        pos = 1;
                    else
                        pos++;
                } else
                    invalid = true;

                cBack = c;
            } while (c != '\n' && !file.eof());
        } else if (token == "mtllib" || token == "usemtl") {
            Info("File is an original .obj, not awo : \"" + path + "\"")
            getline(file, token);
            invalid = true;
        } else {
            getline(file, token);
            invalid = true;
        }
    } while (!error && !file.eof());

    if (currentMesh->material->GetShader() != NULL) {
        currentMesh->tangent = GetTangentTable(v, vt, vn);
        currentMesh->binormal = GetBinormalTable(v, vt, vn);
        currentMesh->normal = GetNormalTable(v);
    }

    if (invalid)
        Warning("File invalid : \"" + path + "\".")

        if (error) {
            Error("Unknow problem when loading file : \"" + path + "\".")
            return error;
        } else
            return success;
}

int Nwm::GetVertex(std::ifstream &file, std::vector<glm::vec3> &table, const std::string type) {
    bool tex;
    std::string token;
    glm::vec3 v;

    file >> token;

    do {
        if (token == type) {
            tex = (token == "vt");

            file >> token;
            v.x = atof(token.c_str());

            file >> token;
            v.y = atof(token.c_str());

            file >> token;
            if (tex)
                v.z = 1.f;
            else {
                v.z = atof(token.c_str());
                file >> token;
            }

            table.push_back(v);

        } else {
            getline(file, token);
            file >> token;
        }
    } while (!file.eof());

    file.clear();
    file.seekg(0);

    return success;
}

void Nwm::Draw() {
    for (std::vector<Mesh>::iterator it = _meshs->begin(); it != _meshs->end(); it++) {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, &(it->v[0].x));
        glNormalPointer(GL_FLOAT, 0, &(it->vn[0].x));
        glTexCoordPointer(3, GL_FLOAT, 0, &(it->vt[0].x));

        if (it->material != NULL)
            it->material->Use(it->tangent, it->binormal, it->normal);

        glDrawElements(GL_TRIANGLES, it->vertIndices.size(), GL_UNSIGNED_INT, &(it->vertIndices[0]));

        if (it->material != NULL)
            it->material->Unuse();

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}

NewtonCollision* Nwm::GetCollisionTree() {
    int strideInBytes = sizeof(float) * 3;
    NewtonCollision * collision = NewtonCreateTreeCollision(NSYSTEM::Physics::GetWorld(), 0);

    NewtonTreeCollisionBeginBuild(collision);

    for (std::vector<glm::vec3>::iterator it = (*_meshs)[0].v.begin(); it != (*_meshs)[0].v.end(); it+=3) {
        glm::vec3 face[3] = {*it, *(it+1), *(it+2)};
        NewtonTreeCollisionAddFace(collision, 3, &(face[0].x), strideInBytes, 0);
    }

    NewtonTreeCollisionEndBuild(collision, 1);

    return collision;
}

NewtonCollision* Nwm::GetConvexHull() {
    NewtonCollision * collision = NULL;

    collision = NewtonCreateConvexHull(NSYSTEM::Physics::GetWorld(), (*_meshs)[0].v.size(), &((*_meshs)[0].v[0].x), sizeof(float) * 3, 0.0001f, 0, NULL);

    return collision;
    //return NewtonCreateConvexHull(NSYSTEM::Physics::GetWorld(), (*_meshs)[0].v.size(), &((*_meshs)[0].v[0].x), sizeof(float) * 3, NULL, NULL);
}

glm::vec3 Nwm::GetMin() {
    glm::vec3 min((*_meshs)[0].v[0]);

    for (std::vector<Mesh>::iterator itMeshs = _meshs->begin(); itMeshs != _meshs->end(); itMeshs++)
        for (std::vector<glm::vec3>::iterator it = itMeshs->v.begin(); it != itMeshs->v.end(); it++) {
            if (it->x < min.x)
                min.x = it->x;
            if (it->y < min.y)
                min.y = it->y;
            if (it->z < min.z)
                min.z = it->z;
        }

    return min;
}

glm::vec3 Nwm::GetMax() {
    glm::vec3 max((*_meshs)[0].v[0]);

    for (std::vector<Mesh>::iterator itMeshs = _meshs->begin(); itMeshs != _meshs->end(); itMeshs++)
        for (std::vector<glm::vec3>::iterator it = itMeshs->v.begin(); it != itMeshs->v.end(); it++) {
            if (it->x > max.x)
                max.x = it->x;
            if (it->y > max.y)
                max.y = it->y;
            if (it->z > max.z)
                max.z = it->z;
        }

    return max;
}

NMODELS::BoundingBox Nwm::GetBoundingBox() {
    NMODELS::BoundingBox bb;

    bb.min = GetMin();
    bb.max = GetMax();

    return bb;
}
