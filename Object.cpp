#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NOBJECTS;
using namespace NSYSTEM;

void ApplyForceAndTorque (const NewtonBody* body, dFloat timestep, int threadIndex) {
    if (NSYSTEM::Physics::GetState() != NSYSTEM::Physics::stop) {
        GLfloat mass;
        glm::vec3 slowness;
        glm::vec3 force;

        NewtonBodyGetMassMatrix(body, &mass, &slowness.x, &slowness.y, &slowness.z);

        if (NSYSTEM::Physics::GetGravity() != NSYSTEM::Physics::stop) {
            force.y = -mass * 9.81 * 10;

            NewtonBodyAddForce (body, &force.x);
        }
    }
}

Object::Object() : Entity(), _nwm(NULL), _md5Model(NULL), _md5Object(NULL), _sprite(NULL) {};

Object::Object(const std::string &path, glm::mat4 matrix, float mass, std::string name) : Entity(), _nwm(NULL), _md5Model(NULL), _md5Object(NULL), _sprite(NULL) {
    Init(path, matrix, mass, name);
}

Object::Object(const std::string &path, float size, float collideSize, glm::vec3 pos, GLfloat mass, std::string name) : Entity(), _nwm(NULL), _md5Model(NULL), _md5Object(NULL), _sprite(NULL) {
    Init(path, size, collideSize, pos, mass, name);
}

Object::~Object() {
    NewtonDestroyBody(NSYSTEM::Physics::GetWorld(), _pBody);
}

void Object::Init(const std::string &path, glm::mat4 matrix, float mass, std::string name) {
    _type = "Object";
    _mass = mass;

    if (path.find(".nwm") != std::string::npos) {
        _nwm = new NMODELS::Nwm(path);

        if (_nwm->GetDescriptionFilePath() != "")
            _dscFile.Execute(_nwm->GetDescriptionFilePath());

        _pBody = NewtonCreateBody(NSYSTEM::Physics::GetWorld(), _nwm->GetConvexHull(), &(glm::mat4(1.f))[0][0]);

        NewtonBodySetMatrix(_pBody, &matrix[0][0]);

        if (mass > 0.f) {
            glm::vec3 origin;

            NewtonConvexCollisionCalculateInertialMatrix(NewtonBodyGetCollision(_pBody), &_slowness.x, &origin.x);
            NewtonBodySetCentreOfMass(_pBody, &origin.x);

            NewtonBodySetMassMatrix(_pBody, mass, _slowness.x * mass, _slowness.y * mass, _slowness.z * mass);
            NewtonBodySetForceAndTorqueCallback(_pBody, ApplyForceAndTorque);
            NewtonBodySetLinearDamping(_pBody, 0.5f);
        }

    } else if (path.find(".loader") != std::string::npos) {
        Md5ParseScript(path);
        _md5mat = matrix;

        NewtonCollision *collision = NULL;
        collision = NewtonCreateSphere(NSYSTEM::Physics::GetWorld(), 2.f, 2.8f * 4.f, 2.f, 0, NULL);
        _pBody = NewtonCreateBody(NSYSTEM::Physics::GetWorld(), collision, &(glm::mat4(1.f))[0][0]);
        NewtonBodySetMatrix(_pBody, &matrix[0][0]);
    } else
        Error("Wrong object type")
    }

void Object::Init(const std::string &path, float size, float collideSize, glm::vec3 pos, GLfloat mass, std::string name) {
    _type = "Object";
    _mass = mass;

    _sprite = new NRENDERING::Sprite(size, pos, path);

    NewtonCollision *collision = NewtonCreateBox(NSYSTEM::Physics::GetWorld(), collideSize, collideSize, collideSize, NULL, NULL);
    _pBody = NewtonCreateBody(NSYSTEM::Physics::GetWorld(), collision, &(glm::mat4(1.f))[0][0]);

    NewtonBodySetContinuousCollisionMode(_pBody, true);

    glm::mat4 matrix = glm::mat4();
    matrix[3] = glm::vec4(pos, 1.f);
    NewtonBodySetMatrix(_pBody, &matrix[0][0]);

    if (mass > 0.f) {
        glm::vec3 origin;

        NewtonConvexCollisionCalculateInertialMatrix(NewtonBodyGetCollision(_pBody), &_slowness.x, &origin.x);
        NewtonBodySetCentreOfMass(_pBody, &origin.x);

        NewtonBodySetMassMatrix(_pBody, mass, _slowness.x * mass, _slowness.y * mass, _slowness.z * mass);
        NewtonBodySetForceAndTorqueCallback(_pBody, ApplyForceAndTorque);
        NewtonBodySetLinearDamping(_pBody, 0.5f);
    }
}

void Object::Md5ParseScript(const std::string &filename) {
    std::string path = NTOOLS::DecodePath(filename);
    std::ifstream file( path.c_str(), std::ios::in );

    if( file.fail() ) {
        Error("Couldn't open " + path)
        return;
    } else
        Info("Parsing " + path + "...")

        try {
            while( !file.eof() ) {
                std::string token, buffer;
                std::string meshFile, animFile, textureFile;
                std::string meshName, animName;

                // Get a token
                file >> token;

                if( token == "model" ) {
                    // Get the rest of line
                    std::getline( file, buffer );

                    // Extract the model file path
                    std::string::size_type start = buffer.find_first_of( "\"" ) + 1;
                    std::string::size_type end = buffer.find_first_of( "\"", start ) - 2;
                    meshFile = buffer.substr( start, end );

                    _md5Model = new NMODELS::Md5Model;

                    // Load mesh model
                    if( _md5Model->loadModel( meshFile ) ) {
                        _md5Object = new NMODELS::Md5Object;

                        // Attach the model to object
                        _md5Object->setMd5Model( _md5Model );
                        _md5Object->setAnim( "NULL" );
                    } else {
                        delete _md5Model;
                    }
                } else if( token == "anim" ) {
                    // Get the rest of line
                    std::getline( file, buffer );

                    // Extract the animation file path
                    std::string::size_type start = buffer.find_first_of( "\"" ) + 1;
                    std::string::size_type end = buffer.find_first_of( "\"", start ) - 2;
                    animFile = buffer.substr( start, end );

                    // Load animation
                    _md5Model->addAnim( animFile );
                } else if( token == "hide" ) {
                    // Get the rest of line
                    std::getline( file, buffer );

                    // Extract the mesh name
                    std::string::size_type start = buffer.find_first_of( "\"" ) + 1;
                    std::string::size_type end = buffer.find_first_of( "\"", start ) - 2;
                    meshName = buffer.substr( start, end );

                    // Set mesh's render state
                    _md5Model->setMeshRenderState( meshName, NMODELS::Md5Mesh::kHide );
                } else if( (token == "decalMap") || (token == "specularMap") ||
                           (token == "normalMap") || (token == "heightMap") ) {
                    std::string::size_type start, end;

                    // Get the next token and extract the mesh name
                    file >> buffer;
                    start = buffer.find_first_of( "\"" ) + 1;
                    end = buffer.find_first_of( "\"", start ) - 1;
                    meshName = buffer.substr( start, end );

                    // Get the rest of line and extract the texture file name
                    std::getline( file, buffer );
                    start = buffer.find_first_of( "\"" ) + 1;
                    end = buffer.find_first_of( "\"", start ) - 2;
                    textureFile = buffer.substr( start, end );

                    // If the model has been loaded, setup
                    // the texture to the desired mesh
                    if( _md5Model ) {
                        if( token == "decalMap" ) {
                            _md5Model->setMeshDecalMap( meshName, textureFile );
                        } else if( token == "specularMap" ) {
                            _md5Model->setMeshSpecularMap( meshName, textureFile );
                        } else if( token == "normalMap" ) {
                            _md5Model->setMeshNormalMap( meshName, textureFile );
                        } else if( token == "heightMap" ) {
                            _md5Model->setMeshHeightMap( meshName, textureFile );
                        }
                    }
                } else if( token == "setAnim" ) {
                    // Get the rest of line
                    std::getline( file, buffer );

                    // Extract the animation name
                    std::string::size_type start = buffer.find_first_of( "\"" ) + 1;
                    std::string::size_type end = buffer.find_first_of( "\"", start ) - 2;
                    animName = buffer.substr( start, end );

                    // Set object's default animation
                    _md5Object->setAnim( animName );
                }
            }

            file.close();
        } catch( std::ios_base::failure &err ) {
            Error("Couldn't read " + path + " script file. Reason: " + err.what())
        }
}

glm::vec3 Object::GetPosition() {
    glm::mat4 matrix;
    NewtonBodyGetMatrix(_pBody, &matrix[0][0]);
    return glm::vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
}

void Object::ActivateGravity(bool activate) {
    GLfloat mass;

    if (activate) {
        SetMass(_mass);
        NewtonBodySetFreezeState (_pBody, 0);
    } else {
        float backMass = _mass;
        SetMass(0.f);
        _mass = backMass;
    }
}

void Object::Impulse(glm::vec3 imp) {
    NewtonBodyAddImpulse(_pBody, &(imp.x), &(GetPosition().x));
}

void Object::SetPosition(glm::vec3 pos) {
    glm::mat4 matrix;
    NewtonBodyGetMatrix(_pBody, &matrix[0][0]);
    matrix[3] = glm::vec4(pos, 1.f);
    NewtonBodySetMatrix(_pBody, &matrix[0][0]);
}

void Object::SetRotation(glm::mat4 rot) {
    glm::mat4 matrix;
    NewtonBodyGetMatrix(_pBody, &matrix[0][0]);
    rot[3] = matrix[3];
    NewtonBodySetMatrix(_pBody, &rot[0][0]);
}

void Object::SetMatrix(glm::mat4 matrix) {
    NewtonBodySetMatrix(_pBody, &matrix[0][0]);
}

void Object::SetMass(float mass) {
    _mass = mass;
    NewtonBodySetMassMatrix(_pBody, _mass, _slowness.x * _mass, _slowness.y * _mass, _slowness.z * _mass);
}

void Object::IncreaseLife(int value) {
    _life += value;
}

void Object::DecreaseLife(int value) {
    _life -= value;

    if (_life <= 0)
        Destroy();
}

int Object::Use() {
    /*if (_dscFile.ActiveFunction("OnUse")) {
        _dscFile.CallFunction(0, 0);
        return success;
    }*/

    return error;
}

void Object::Destroy() {
    delete this;
}

void Object::TemporaryDisableAutoSleep() {
    _tdas = true;
}

void Object::Check(float elapsedTime) {
    if (!_attached.empty()) {
        glm::vec3 pos = GetPosition();
        for(std::vector<NPATHERNS::Entity*>::iterator it = _attached.begin(); it != _attached.end(); it++)
            (*it)->SetPosition(pos);
    }

    if (_tdas) {
        //NewtonBodySetFreezeState(_pBody, 1);
        _tdas = false;
    }

    if (_sprite != NULL)
        _sprite->SetPosition(GetPosition());
}

void Object::Draw(float elapsedTime) {
    glPushMatrix();

    glm::mat4 matrix;
    NewtonBodyGetMatrix(_pBody, &matrix[0][0]);
    glMultMatrixf(&matrix[0][0]);

    if (_nwm != NULL)
        _nwm->Draw();
    else if (_md5Object != NULL) {
        //glMultMatrixf(&_md5mat[0][0]);
        _md5Object->setRenderFlags(NMODELS::Md5Object::kDrawModel);
        _md5Object->animate( true ? elapsedTime : 0.0f );
        _md5Object->computeBoundingBox();
        _md5Object->prepare(false);
        _md5Object->render();
    }

    glPopMatrix();
}
