#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NMODELS;

// Shader related global variables
extern GLuint lightProg;
extern bool bShaders;
extern bool bParallax;


/////////////////////////////////////////////////////////////////////////////
//
// global stuff
//
/////////////////////////////////////////////////////////////////////////////

// Object deletion for containers... We should use a smart
// pointer instead to be exception-safe.
struct DeleteObjectPtr {
    template<typename T>
    void operator()( const T *ptr ) const {
        delete ptr;
    }
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md5Skeleton implementation.
//
/////////////////////////////////////////////////////////////////////////////


// --------------------------------------------------------------------------
// Md5Skeleton::~Md5Skeleton
//
// Destructor.  Free all data allocated for skeleton's joints.
// --------------------------------------------------------------------------

Md5Skeleton::~Md5Skeleton( void ) {
    // Not exception-safe!
    std::for_each( _joints.begin(), _joints.end(), DeleteObjectPtr() );
}


// --------------------------------------------------------------------------
// Md5Skeleton::load
//
// Load skeleton data from a <.md5mesh> file.
// --------------------------------------------------------------------------

bool Md5Skeleton::load( std::ifstream &file ) {
    string token, buffer;

    if( !file.is_open() ) {
        return false;
    }

    // Read all joints
    for( unsigned int i = 0; i < _numJoints; ++i ) {
        // NOTE: hope there isn't any comment between
        // two lines of joints data...
        Md5Joint_t *j = new Md5Joint_t;

        file >> j->name;
        file >> j->parent;
        file >> token; // "("
        file >> j->pos._x;
        file >> j->pos._y;
        file >> j->pos._z;
        file >> token; // ")"
        file >> token; // "("
        file >> j->orient._x;
        file >> j->orient._y;
        file >> j->orient._z;
        file >> token; // ")"

        // Eat up rest of the line
        std::getline( file, buffer );

        // Compute orient quaternion's w value
        j->orient.computeW();

        // Add joint to joints vector
        _joints.push_back( j );
    }

    return true;
}



// --------------------------------------------------------------------------
// Md5Skeleton::draw
//
// Draw skeleton's bones and joints.
// --------------------------------------------------------------------------

void Md5Skeleton::draw( const Matrix4x4f &modelView, bool labelJoints ) {
    unsigned int i;

    // Draw each joint
    glPointSize( 5.0f );
    glColor3f( 1.0f, 0.0f, 0.0f );
    glBegin( GL_POINTS );
    for( i = 0; i < _numJoints; ++i ) {
        glVertex3fv( _joints[i]->pos._v );
    }
    glEnd();
    glPointSize( 1.0f );

    // Draw each bone
    glColor3f( 0.0f, 1.0f, 0.0f );
    glBegin( GL_LINES );
    for( i = 0; i < _numJoints; ++i ) {
        if( _joints[i]->parent != -1 ) {
            glVertex3fv( _joints[ _joints[i]->parent ]->pos._v );
            glVertex3fv( _joints[i]->pos._v );
        }
    }
    glEnd();

    // Label each joint
    if( labelJoints ) {
        vector<Md5Joint_t> jointlist( _numJoints );

        // Copy joint's position and name
        for( i = 0; i < _numJoints; ++i ) {
            jointlist.push_back( *_joints[i] );
        }

        // Sort joints about depth because of alpha blending
        std::sort( jointlist.begin(), jointlist.end() );

        GLfloat mat[16];
        glGetFloatv( GL_MODELVIEW_MATRIX, mat );

        glPushMatrix();
        // Setup billboard matrix
        mat[0] = 1.0f;
        mat[1] = 0.0f;
        mat[2] = 0.0f;
        mat[4] = 0.0f;
        mat[5] = 1.0f;
        mat[6] = 0.0f;
        mat[8] = 0.0f;
        mat[9] = 0.0f;
        mat[10]= 1.0f;

        glLoadMatrixf( mat );

        glPushAttrib( GL_POLYGON_BIT );
        glFrontFace( GL_CCW );
        glPolygonMode( GL_FRONT, GL_FILL );
        glColor3f( 1.0f, 1.0f, 1.0f );

        glLoadIdentity();
        glScalef( 0.1f, 0.1f, 0.1f );

        for( i = 0; i < _numJoints; ++i ) {
            glPushMatrix();
            // Move to joint's position
            glTranslatef( jointlist[i].pos._x * 10.0f,
                          jointlist[i].pos._y * 10.0f,
                          jointlist[i].pos._z * 10.0f );

            // font->printText( jointlist[i].name.c_str() );
            glPopMatrix();
        }
        glPopAttrib();
        glPopMatrix();
    }
}


/////////////////////////////////////////////////////////////////////////////
//
// class Md5Mesh implementation.
//
/////////////////////////////////////////////////////////////////////////////


// --------------------------------------------------------------------------
// Md5Mesh::~Md5Mesh
//
// Destructor.  Free all data allocated for the mesh, i.e. vertices,
// triangles, weights and vertex arrays.
// --------------------------------------------------------------------------

Md5Mesh::~Md5Mesh( void ) {
    // Not exception-safe!
    std::for_each( _verts.begin(), _verts.end(), DeleteObjectPtr() );
    std::for_each( _tris.begin(), _tris.end(), DeleteObjectPtr() );
    std::for_each( _weights.begin(), _weights.end(), DeleteObjectPtr() );

    // Free memory allocated for vertex arrays
    freeVertexArrays();
}


// --------------------------------------------------------------------------
// Md5Mesh::load
//
// Load mesh data from a <.md5mesh> file.
// --------------------------------------------------------------------------

bool Md5Mesh::load( std::ifstream &file ) {
    string token, buffer;

    if( !file.is_open() ) {
        return false;
    }

    do {
        // Read first token line from file
        file >> token;

        if( token == "shader" ) {
            file >> _shader;

            // Remove quote marks from the shader string
            _shader = _shader.substr( _shader.find_first_of( "\"" ) + 1,
                                      _shader.find_last_of( "\"" ) - 1 );

            loadTextures();

            // Get mesh name from shader string
            _name = _shader.c_str() + _shader.find_last_of( "/" ) + 1;
        } else if( token == "numverts" ) {
            file >> _numVerts;
            _verts.reserve( _numVerts );
        } else if( token == "numtris" ) {
            file >> _numTris;
            _tris.reserve( _numTris );
        } else if( token == "numweights" ) {
            file >> _numWeights;
            _weights.reserve( _numWeights );
        } else if( token == "vert" ) {
            Md5Vertex_t *v = new Md5Vertex_t;

            // Read vertex data
            file >> token; // index
            file >> token; // "("
            file >> v->st[0];
            file >> v->st[1];
            file >> token; // ")"
            file >> v->startWeight;
            file >> v->countWeight;

            _verts.push_back( v );
        } else if( token == "tri" ) {
            Md5Triangle_t *tri = new Md5Triangle_t;

            // Read triangle data
            file >> token; // index
            file >> tri->index[0];
            file >> tri->index[1];
            file >> tri->index[2];

            _tris.push_back( tri );
        } else if( token == "weight" ) {
            Md5Weight_t *w = new Md5Weight_t;

            // Read weight data
            file >> token; // index
            file >> w->joint;
            file >> w->bias;
            file >> token; // "("
            file >> w->pos._x;
            file >> w->pos._y;
            file >> w->pos._z;
            file >> token; // ")"

            _weights.push_back( w );
        }

        // Eat up rest of the line
        std::getline( file, buffer );

    } while( (token != "}") && !file.eof() );

    // Memory allocation for vertex arrays
    allocVertexArrays();

    // Setup texture coordinates array
    setupTexCoordArray();

    return true;
}


// --------------------------------------------------------------------------
// Md5Mesh::setupVertexArray
//
// Compute vertices' position, normal and tangent.
// --------------------------------------------------------------------------

void Md5Mesh::setupVertexArrays( Md5Skeleton *skel ) {
    for( unsigned int i = 0; i < _numVerts; ++i ) {
        Vector3f finalVertex = kZeroVectorf;
        Vector3f finalNormal = kZeroVectorf;
        Vector3f finalTangent = kZeroVectorf;

        // Calculate final vertex to draw with weights
        for( int j = 0; j < _verts[i]->countWeight; ++j ) {
            const Md5Weight_t *pWeight = _weights[ _verts[i]->startWeight + j ];
            const Md5Joint_t *pJoint = skel->getJoint( pWeight->joint );

            // Calculate transformed vertex for this weight
            Vector3f wv = pWeight->pos;
            pJoint->orient.rotate( wv );

            // The sum of all pWeight->bias should be 1.0
            finalVertex += (pJoint->pos + wv) * pWeight->bias;

            // Calculate transformed normal for this weight
            Vector3f wn = pWeight->norm;
            pJoint->orient.rotate( wn );

            finalNormal += wn * pWeight->bias;

            // Calculate transformed tangent for this weight
            Vector3f wt = pWeight->tan;
            pJoint->orient.rotate( wt );

            finalTangent += wt * pWeight->bias;
        }

        // We can omit to normalize normal and tangent,
        // because they should have been already normalized
        // when they were computed. We can gain some time
        // avoiding some heavy calculus.

        finalNormal.normalize();
        finalTangent.normalize();

        _vertexArray[i][0] = finalVertex._x;
        _vertexArray[i][1] = finalVertex._y;
        _vertexArray[i][2] = finalVertex._z;

        _normalArray[i][0] = finalNormal._x;
        _normalArray[i][1] = finalNormal._y;
        _normalArray[i][2] = finalNormal._z;

        _tangentArray[i][0] = finalTangent._x;
        _tangentArray[i][1] = finalTangent._y;
        _tangentArray[i][2] = finalTangent._z;
    }
}


// --------------------------------------------------------------------------
// Md5Mesh::setupTexCoordArray
//
// Compute vertices' texture coordinate.
// --------------------------------------------------------------------------

void Md5Mesh::setupTexCoordArray( void ) {
    for( unsigned int i = 0; i < _numVerts; ++i ) {
        _texCoordArray[i][0] = _verts[i]->st[0];
        _texCoordArray[i][1] = 1.0f - _verts[i]->st[1];
    }
}


// --------------------------------------------------------------------------
// Md5Mesh::computeWeightNormals
//
// der_ton said:
//
// * First you have to get the bind-pose model-space normals by calculating
//   them from the model geometry in bind-pose.
//
// * Then you calculate the weight's normal (which is in bone-space) by
//   invert-transforming the normal by the bone-space matrix.
//
// * So afterwards when animating, you'll transform the weight normal with
//   the animated bone-space matrix and add them all up and you'll get
//   back your animated vertex normal.
// --------------------------------------------------------------------------

void Md5Mesh::computeWeightNormals( Md5Skeleton &skel ) {
    vector<Vector3f> bindposeVerts( _numVerts );
    vector<Vector3f> bindposeNorms( _numVerts );

    for( unsigned int i = 0; i < _numVerts; ++i ) {
        // Zero out final vertex position and final vertex normal
        bindposeVerts[i] = kZeroVectorf;
        bindposeNorms[i] = kZeroVectorf;

        for( int j = 0; j < _verts[i]->countWeight; ++j ) {
            const Md5Weight_t *pWeight = _weights[ _verts[i]->startWeight + j ];
            const Md5Joint_t *pJoint = skel.getJoint( pWeight->joint );

            // Calculate transformed vertex for this weight
            Vector3f wv = pWeight->pos;
            pJoint->orient.rotate( wv );

            bindposeVerts[i] += (pJoint->pos + wv) * pWeight->bias;
        }
    }

    // Compute triangle normals
    for( unsigned int i = 0; i < _numTris; ++i ) {
        const Md5Triangle_t *pTri = _tris[i];

        Vector3f triNorm( -ComputeNormal( bindposeVerts[ pTri->index[0] ],
                                          bindposeVerts[ pTri->index[1] ], bindposeVerts[ pTri->index[2] ] ) );

        for( int j = 0; j < 3; ++j ) {
            bindposeNorms[ pTri->index[j] ] += triNorm;
        }
    }

    // "Average" the surface normals, by normalizing them
    for( unsigned int i = 0; i < _numVerts; ++i ) {
        bindposeNorms[i].normalize();
    }

    //
    // At this stage we have all vertex normals computed
    // for the model geometry in bind-pos
    //

    // Zero out all weight normals
    for( unsigned int i = 0; i < _numWeights; ++i ) {
        _weights[i]->norm = kZeroVectorf;
    }

    // Compute weight normals by invert-transforming the normal
    // by the bone-space matrix
    for( unsigned int i = 0; i < _numVerts; ++i ) {
        for( int j = 0; j < _verts[i]->countWeight; ++j ) {
            Md5Weight_t *pWeight = _weights[ _verts[i]->startWeight + j ];
            const Md5Joint_t *pJoint = skel.getJoint( pWeight->joint );

            Vector3f wn = bindposeNorms[i];

            // Compute inverse quaternion rotation
            Quaternionf invRot = Inverse( pJoint->orient );
            invRot.rotate( wn );

            pWeight->norm += wn;
        }
    }

    // Normalize all weight normals
    for( unsigned int i = 0; i < _numWeights; ++i ) {
        _weights[i]->norm.normalize();
    }
}


// --------------------------------------------------------------------------
// Md5Mesh::computeWeightTangents
//
// Compute per-vertex tangent vectors and then, calculate the weight
// tangent.
// --------------------------------------------------------------------------

void Md5Mesh::computeWeightTangents( Md5Skeleton &skel ) {
    vector<Vector3f> bindposeVerts( _numVerts );
    vector<Vector3f> bindposeNorms( _numVerts );
    vector<Vector3f> bindposeTans( _numVerts );

    vector<Vector3f> sTan( _numVerts );
    vector<Vector3f> tTan( _numVerts );

    // Zero out all weight tangents (thank you Valgrind :s)
    for( unsigned int i = 0; i < _numWeights; ++i ) {
        _weights[i]->tan = kZeroVectorf;
    }

    // Compute bind-pose vertices and normals
    for( unsigned int i = 0; i < _numVerts; ++i ) {
        // Zero out final vertex position, normal and tangent
        bindposeVerts[i] = kZeroVectorf;
        bindposeNorms[i] = kZeroVectorf;
        bindposeTans[i] = kZeroVectorf;

        // Zero s-tangents and t-tangents
        sTan[i] = kZeroVectorf;
        tTan[i] = kZeroVectorf;

        for( int j = 0; j < _verts[i]->countWeight; ++j ) {
            const Md5Weight_t *pWeight = _weights[ _verts[i]->startWeight + j ];
            const Md5Joint_t *pJoint = skel.getJoint( pWeight->joint );

            // Calculate transformed vertex for this weight
            Vector3f wv = pWeight->pos;
            pJoint->orient.rotate( wv );

            bindposeVerts[i] += (pJoint->pos + wv) * pWeight->bias;

            // Calculate transformed normal for this weight
            Vector3f wn = pWeight->norm;
            pJoint->orient.rotate( wn );

            bindposeNorms[i] += wn * pWeight->bias;
        }
    }

    // Calculate s-tangeants and t-tangeants at triangle level
    for( unsigned int i = 0; i < _numTris; ++i ) {
        const Md5Triangle_t *pTri = _tris[i];

        const Vector3f &v0 = bindposeVerts[ pTri->index[0] ];
        const Vector3f &v1 = bindposeVerts[ pTri->index[1] ];
        const Vector3f &v2 = bindposeVerts[ pTri->index[2] ];

        const vec2_t &w0 = _verts[ pTri->index[0] ]->st;
        const vec2_t &w1 = _verts[ pTri->index[1] ]->st;
        const vec2_t &w2 = _verts[ pTri->index[2] ]->st;

        float x1 = v1._x - v0._x;
        float x2 = v2._x - v0._x;
        float y1 = v1._y - v0._y;
        float y2 = v2._y - v0._y;
        float z1 = v1._z - v0._z;
        float z2 = v2._z - v0._z;

        float s1 = w1[0] - w0[0];
        float s2 = w2[0] - w0[0];
        float t1 = w1[1] - w0[1];
        float t2 = w2[1] - w0[1];

        float r = s1 * t2 - s2 * t1;

        if( r == 0.0f ) {
            // Prevent division by zero
            r = 1.0f;
        }

        float oneOverR = 1.0f / r;

        Vector3f sDir( (t2 * x1 - t1 * x2) * oneOverR,
                       (t2 * y1 - t1 * y2) * oneOverR,
                       (t2 * z1 - t1 * z2) * oneOverR );
        Vector3f tDir( (s1 * x2 - s2 * x1) * oneOverR,
                       (s1 * y2 - s2 * y1) * oneOverR,
                       (s1 * z2 - s2 * z1) * oneOverR );

        for( unsigned int j = 0; j < 3; ++j ) {
            sTan[ pTri->index[j] ] += sDir;
            tTan[ pTri->index[j] ] += tDir;
        }
    }

    // Calculate vertex tangent
    for( unsigned int i = 0; i < _numVerts; ++i ) {
        const Vector3f &n = bindposeNorms[i];
        const Vector3f &t = sTan[i];

        // Gram-Schmidt orthogonalize
        bindposeTans[i] = (t - n * DotProduct( n, t ));
        bindposeTans[i].normalize();

        // Calculate handedness
        if( DotProduct( CrossProduct( n, t ), tTan[i] ) < 0.0f ) {
            bindposeTans[i] = -bindposeTans[i];
        }

        // Compute weight tangent
        for( int j = 0; j < _verts[i]->countWeight; ++j ) {
            Md5Weight_t *pWeight = _weights[ _verts[i]->startWeight + j ];
            const Md5Joint_t *pJoint = skel.getJoint( pWeight->joint );

            Vector3f wt = bindposeTans[i];

            // Compute inverse quaternion rotation
            Quaternionf invRot = Inverse( pJoint->orient );
            invRot.rotate( wt );

            pWeight->tan += wt;
        }
    }

    // Normalize all weight tangents
    for( unsigned int i = 0; i < _numWeights; ++i ) {
        _weights[i]->tan.normalize();
    }
}


// --------------------------------------------------------------------------
// Md5Mesh::computeBoundingBox
//
// Compute mesh bounding box for a given skeleton.
// --------------------------------------------------------------------------

void Md5Mesh::computeBoundingBox( Md5Skeleton &skel ) {
    Vector3f max( -99999.0f, -99999.0f, -99999.0f );
    Vector3f min(  99999.0f,  99999.0f,  99999.0f );

    for( unsigned int i = 0; i < _numVerts; ++i ) {
        Vector3f finalVertex = kZeroVectorf;

        // Calculate final vertex to draw with weights
        for( int j = 0; j < _verts[i]->countWeight; ++j ) {
            const Md5Weight_t *pWeight = _weights[ _verts[i]->startWeight + j ];
            const Md5Joint_t *pJoint = skel.getJoint( pWeight->joint );

            // Calculate transformed vertex for this weight
            Vector3f wv = pWeight->pos;
            pJoint->orient.rotate( wv );

            // The sum of all pWeight->bias should be 1.0
            finalVertex += (pJoint->pos + wv) * pWeight->bias;
        }

        if( finalVertex._x > max._x ) {
            max._x = finalVertex._x;
        }

        if( finalVertex._x < min._x ) {
            min._x = finalVertex._x;
        }

        if( finalVertex._y > max._y ) {
            max._y = finalVertex._y;
        }

        if( finalVertex._y < min._y ) {
            min._y = finalVertex._y;
        }

        if( finalVertex._z > max._z ) {
            max._z = finalVertex._z;
        }

        if( finalVertex._z < min._z ) {
            min._z = finalVertex._z;
        }
    }

    _BoundingBox.min = min;
    _BoundingBox.max = max;
}


// --------------------------------------------------------------------------
// Md5Mesh::renderVertexArray
//
// Render mesh with vertex array.
// --------------------------------------------------------------------------

void Md5Mesh::renderVertexArrays( void ) {
    GLint tangentLoc = -1;
    /*bool useShader = bShaders && lightProg && _normalMap;

    if( useShader ) {
    if( GLEW_VERSION_2_0 ) {
      // Enable shader
      glUseProgram( lightProg );
      GLint texLoc, parallaxLoc;

      // Set uniform parameters
      texLoc = glGetUniformLocation( lightProg, "decalMap" );
      glUniform1i( texLoc, 0 );

      texLoc = glGetUniformLocation( lightProg, "glossMap" );
      glUniform1i( texLoc, 1 );

      texLoc = glGetUniformLocation( lightProg, "normalMap" );
      glUniform1i( texLoc, 2 );

      texLoc = glGetUniformLocation( lightProg, "heightMap" );
      glUniform1i( texLoc, 3 );

      parallaxLoc = glGetUniformLocation( lightProg, "parallaxMapping" );
      glUniform1i( parallaxLoc, bParallax );

      // Get attribute location
      tangentLoc = glGetAttribLocation( lightProg, "tangent" );

      if( tangentLoc != -1 ) {
    	glEnableVertexAttribArray( tangentLoc );

    	// Upload tangents
    	glVertexAttribPointer( tangentLoc, 3, GL_FLOAT, GL_FALSE, 0, _tangentArray );
      }
    }
    else if( GLEW_VERSION_1_5 ) {
      // Enable shader
      glUseProgramObjectARB( lightProg );
      GLint texLoc, parallaxLoc;

      // Set uniform parameters
      texLoc = glGetUniformLocationARB( lightProg, "decalMap" );
      glUniform1iARB( texLoc, 0 );

      texLoc = glGetUniformLocationARB( lightProg, "glossMap" );
      glUniform1iARB( texLoc, 1 );

      texLoc = glGetUniformLocationARB( lightProg, "normalMap" );
      glUniform1iARB( texLoc, 2 );

      texLoc = glGetUniformLocationARB( lightProg, "heightMap" );
      glUniform1iARB( texLoc, 3 );

      parallaxLoc = glGetUniformLocationARB( lightProg, "parallaxMapping" );
      glUniform1iARB( parallaxLoc, bParallax );

      // Get attribute location
      tangentLoc = glGetAttribLocationARB( lightProg, "tangent" );

      if( tangentLoc != -1 ) {
    	glEnableVertexAttribArrayARB( tangentLoc );

    	// Upload tangents
    	glVertexAttribPointerARB( tangentLoc, 3, GL_FLOAT, GL_FALSE, 0, _tangentArray );
      }
    }
    } */

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    // Upload mesh data to OpenGL
    glVertexPointer( 3, GL_FLOAT, 0, _vertexArray );
    glNormalPointer( GL_FLOAT, 0, _normalArray );
    glTexCoordPointer( 2, GL_FLOAT, 0, _texCoordArray );

    if (_heightMap != NULL) {
        glActiveTexture( GL_TEXTURE3 );
        _heightMap->Bind();
    }

    if (_normalMap != NULL) {
        glActiveTexture( GL_TEXTURE2 );
        _normalMap->Bind();
    }

    if (_specMap != NULL) {
        glActiveTexture( GL_TEXTURE1 );
        _specMap->Bind();
    }

    if (_decal != NULL) {
        glActiveTexture( GL_TEXTURE0 );
        _decal->Bind();
    }

    // Draw the mesh
    glDrawElements( GL_TRIANGLES, _numTris * 3,
                    GL_UNSIGNED_INT, _vertIndices );

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    /*if( useShader ) {
    // Disable shader
    if( GLEW_VERSION_2_0 ) {
      if( tangentLoc != -1 ) {
    	glDisableVertexAttribArray( tangentLoc );
      }

      glUseProgram( 0 );
    }
    else if( GLEW_VERSION_1_5 ) {
      if( tangentLoc != -1 ) {
    	glDisableVertexAttribArrayARB( tangentLoc );
      }

      glUseProgramObjectARB( 0 );
    }
    }*/
}


// --------------------------------------------------------------------------
// Md5Mesh::drawNormals
//
// Draw mesh's vertex normals.
// --------------------------------------------------------------------------

void Md5Mesh::drawNormals( void ) {
    glPushAttrib( GL_ENABLE_BIT );
    glDisable( GL_LIGHTING );
    glDisable( GL_TEXTURE_2D );

    glColor3f( 0.0, 0.0, 1.0 );

    // Draw normals
    glBegin( GL_LINES );
    for( unsigned int i = 0; i < _numVerts; ++i ) {
        Vector3f thisVertex;

        thisVertex._x = _vertexArray[i][0];
        thisVertex._y = _vertexArray[i][1];
        thisVertex._z = _vertexArray[i][2];

        Vector3f thisNormal;

        thisNormal._x = _normalArray[i][0];
        thisNormal._y = _normalArray[i][1];
        thisNormal._z = _normalArray[i][2];

        Vector3f normVec( thisVertex + thisNormal );

        glVertex3fv( thisVertex._v );
        glVertex3fv( normVec._v );
    }
    glEnd();

    glColor3f( 1.0, 0.0, 1.0 );

    // Draw tangents
    glBegin( GL_LINES );
    for( unsigned int i = 0; i < _numVerts; ++i ) {
        Vector3f thisVertex;

        thisVertex._x = _vertexArray[i][0];
        thisVertex._y = _vertexArray[i][1];
        thisVertex._z = _vertexArray[i][2];

        Vector3f thisTangent;

        thisTangent._x = _tangentArray[i][0];
        thisTangent._y = _tangentArray[i][1];
        thisTangent._z = _tangentArray[i][2];

        Vector3f tanVec( thisVertex + thisTangent );

        glVertex3fv( thisVertex._v );
        glVertex3fv( tanVec._v );
    }
    glEnd();

    glPopAttrib();
}


// --------------------------------------------------------------------------
// Md5Mesh::allocVertexArray
//
// Allocate memory for vertex array.
// --------------------------------------------------------------------------

void Md5Mesh::allocVertexArrays( void ) {
    _vertexArray = new vec3_t[ _numVerts ];
    _normalArray = new vec3_t[ _numVerts ];
    _tangentArray = new vec3_t[ _numVerts ];
    _texCoordArray = new vec2_t[ _numVerts ];
    _vertIndices = new GLuint[ _numTris * 3 ];

    // We can already initialize the vertex index array (we won't have
    // to do it each time we want to draw!)
    for( unsigned int k = 0, i = 0; i < _numTris; ++i ) {
        for( int j = 0; j < 3; ++j, ++k ) {
            _vertIndices[k] = _tris[i]->index[j];
        }
    }
}


// --------------------------------------------------------------------------
// Md5Mesh::freeVertexArray
//
// Free allocated memory for vertex array.
// --------------------------------------------------------------------------

void Md5Mesh::freeVertexArrays( void ) {
    if( _vertexArray ) {
        delete [] _vertexArray;
        _vertexArray = NULL;
    }

    if( _normalArray ) {
        delete [] _normalArray;
        _normalArray = NULL;
    }

    if( _tangentArray ) {
        delete [] _tangentArray;
        _tangentArray = NULL;
    }

    if( _texCoordArray ) {
        delete [] _texCoordArray;
        _texCoordArray = NULL;
    }

    if( _vertIndices ) {
        delete [] _vertIndices;
        _vertIndices = NULL;
    }
}


// --------------------------------------------------------------------------
// Md5Mesh::loadTextures
//
// Load mesh's textures.
// --------------------------------------------------------------------------

void Md5Mesh::loadTextures( void ) {
    return;
}


/////////////////////////////////////////////////////////////////////////////
//
// class Md5Model implementation.
//
/////////////////////////////////////////////////////////////////////////////


// --------------------------------------------------------------------------
// Md5Model::~Md5Model
//
// Destructor.  Free all data allocated for the model.
// --------------------------------------------------------------------------

Md5Model::~Md5Model( void ) {
    // Not exception-safe!
    std::for_each( _meshes.begin(), _meshes.end(), DeleteObjectPtr() );

    // Simply awful
    for( AnimMap::iterator itor = _animList.begin();
            itor != _animList.end(); ++itor ) {

        delete itor->second;
    }

    _animList.clear();
}


// --------------------------------------------------------------------------
// Md5Model::loadMd5Mesh
//
// Load MD5 mesh from file.
// --------------------------------------------------------------------------

bool Md5Model::loadModel( const string &filename ) {
    // Open file
    _modelPath = filename;
    std::string path = filename;
    std::ifstream file( path.c_str(), std::ios::in );

    if( file.fail() ) {
        Error("Couldn't load " + path)
        return false;
    }

    try {
        while( !file.eof() ) {
            string token, buffer;
            int version;

            // Read next token
            file >> token;

            if( token == "//" ) {
                // This is the begining of a comment
                // Eat up rest of the line
                std::getline( file, buffer );
            } else if( token == "MD5Version" ) {
                file >> version;

                if( version != kMd5Version ) {
                    Error("MD5Version of " + path + " is " + NTOOLS::itos(version) + " and should be " + NTOOLS::itos(kMd5Version))
                    file.close();
                    return false;
                }
            } else if( token == "numJoints" ) {
                file >> _numJoints;
                _baseSkeleton.setNumJoints( _numJoints );
            } else if( token == "numMeshes" ) {
                file >> _numMeshes;
                _meshes.reserve( _numMeshes );
            } else if( token == "joints" ) {
                // Base skeleton data
                file >> token; // "{"

                if( !_baseSkeleton.load( file ) ) {
                    // An error has occured when loading skeleton data
                    file.close();
                    return false;
                }

                file >> token; // "}"
            } else if( token == "mesh" ) {
                file >> token; // "{"

                // Create a new model mesh
                Md5Mesh *mesh = new Md5Mesh;

                // Load it
                if( !mesh->load( file ) ) {
                    // An error has occured when loading mesh data
                    delete mesh;
                    file.close();
                    return false;
                } else {
                    // Compute bounding box in bind-pose
                    mesh->computeBoundingBox( _baseSkeleton );

                    // Compute weight normals
                    mesh->computeWeightNormals( _baseSkeleton );

                    // Compute weight tangents
                    mesh->computeWeightTangents( _baseSkeleton );
                }

                _meshes.push_back( mesh );
            }
        }

        file.close();

        // Compute the bounding box in bind-pose
        computeBindPoseBoundingBox();

        Info("Md5Model Mesh: " + path + " loaded")
        return true;
    } catch( std::ios_base::failure &err ) {
        Error("Couldn't read " + path + " script file. Reason: " + err.what())
        return false;
    }
}


// --------------------------------------------------------------------------
// Md5Model::drawModel
//
// Draw each mesh of the model.
// --------------------------------------------------------------------------

void Md5Model::drawModel( void ) {
    for( unsigned int i = 0; i < _numMeshes; ++i ) {
        if( _meshes[i]->show() ) {
            _meshes[i]->renderVertexArrays();

            if( 0 ) {
                _meshes[i]->drawNormals();
            }
        }
    }
}


// --------------------------------------------------------------------------
// Md5Model::prepare
//
// Prepare each mesh of the model for drawing, i.e. compute final vertex
// positions, normals and other vertex's related data.
// --------------------------------------------------------------------------

void Md5Model::prepare( Md5Skeleton &skel ) {
    for( unsigned int i = 0; i < _numMeshes; ++i ) {
        if( !_meshes[i]->hiden() ) {
            // Prepare for drawing with interpolated skeleton
            _meshes[i]->setupVertexArrays( &skel );
        }
    }
}


// --------------------------------------------------------------------------
// Md5Model::addAnim
//
// Load a MD5 animation and add it to the animation list.  Return true if
// the animation has been loaded successfully.
// --------------------------------------------------------------------------

bool Md5Model::addAnim( const string &filename ) {
    Md5Animation *pAnim = new Md5Animation;

    if( pAnim->load( filename ) ) {
        // Check for compatibility
        if( !validityCheck( pAnim ) ) {
            Error(filename + " isn't a valid animation for the model " + _modelPath)
            delete pAnim;
            return false;
        }

        string name = pAnim->getName();

        // If there is already an animation with same name,
        // delete it
        AnimMap::iterator itor = _animList.find( name );
        if( itor != _animList.end() ) {
            delete itor->second;
            _animList.erase( itor );
        }

        // Insert the new animation
        _animList.insert( AnimMap::value_type( name, pAnim ) );
    } else {
        // Failled to load the animation file, free memory
        // and exit.
        delete pAnim;
        return false;
    }

    return true;
}


// --------------------------------------------------------------------------
// Md5Model::getAnim
//
// Return animation from list.
// --------------------------------------------------------------------------

Md5Animation *Md5Model::getAnim( const string &name ) {
    AnimMap::iterator itor = _animList.find( name );
    if( itor != _animList.end() ) {
        return itor->second;
    }

    return NULL;
}


// --------------------------------------------------------------------------
// Md5Model::computeBindPoseBoundingBox
//
// Compute model's bounding box in bind-pose.
// --------------------------------------------------------------------------

void Md5Model::computeBindPoseBoundingBox( void ) {
    Vector3f max( -99999.0f, -99999.0f, -99999.0f );
    Vector3f min(  99999.0f,  99999.0f,  99999.0f );

    // Get the min and the max from all mesh bounding boxes
    for( unsigned int i = 0; i < _numMeshes; ++i ) {
        BoundingBox_t meshBox = _meshes[i]->getBoundingBox();

        if( meshBox.max._x > max._x ) {
            max._x = meshBox.max._x;
        }

        if( meshBox.min._x < min._x ) {
            min._x = meshBox.min._x;
        }

        if( meshBox.max._y > max._y ) {
            max._y = meshBox.max._y;
        }

        if( meshBox.min._y < min._y ) {
            min._y = meshBox.min._y;
        }

        if( meshBox.max._z > max._z ) {
            max._z = meshBox.max._z;
        }

        if( meshBox.min._z < min._z ) {
            min._z = meshBox.min._z;
        }
    }

    _bindPoseBox.min = min;
    _bindPoseBox.max = max;
}


// --------------------------------------------------------------------------
// Md5Model::validityCheck
//
// Check if an animation is valid for this model or not.  A valid
// animation must have a skeleton with the same number of joints with
// model's skeleton and for each skeleton joint, name and parent Id must
// match.
// --------------------------------------------------------------------------

bool Md5Model::validityCheck( Md5Animation *anim ) {
    if( anim && (_numJoints == anim->getFrame(0)->getNumJoints()) ) {
        for( unsigned int i = 0; i < _numJoints; ++i ) {
            Md5Joint_t *modelJoint = _baseSkeleton.getJoint(i);
            Md5Joint_t *animJoint = anim->getFrame(0)->getJoint(i);

            if( modelJoint->name != animJoint->name ) {
                return false;
            }

            if( modelJoint->parent != animJoint->parent ) {
                return false;
            }
        }
    } else {
        return false;
    }

    return true;
}


// --------------------------------------------------------------------------
// Md5Model::getMeshByName
//
// Get pointer to a mesh given its name.  Return NULL if there is no mesh
// with such a name.
// --------------------------------------------------------------------------

Md5Mesh *Md5Model::getMeshByName( const string &name ) {
    for( unsigned int i = 0; i < _numMeshes; ++i ) {
        if( _meshes[i]->getName() == name ) {
            return _meshes[i];
        }
    }

    return NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
// class Md5Animation implementation.
//
/////////////////////////////////////////////////////////////////////////////


// --------------------------------------------------------------------------
// Md5Animation::~Md5Animation
//
// Destructor.  Free all data allocated for the animation.
// --------------------------------------------------------------------------

Md5Animation::~Md5Animation( void ) {
    // Not exception-safe!
    std::for_each( _skelframes.begin(), _skelframes.end(), DeleteObjectPtr() );
    std::for_each( _bboxes.begin(), _bboxes.end(), DeleteObjectPtr() );
}


// --------------------------------------------------------------------------
// Md5Animation::load
//
// Load MD5 animation from <.md5anim> file.
// --------------------------------------------------------------------------

bool Md5Animation::load( const string &filename ) {
    vector<JointInfo_t> jointInfos;
    vector<BaseFrameJoint_t> baseFrame;
    vector<float> animFrameData;
    int numJoints = 0;
    int numAnimatedComponents = 0;

    // Open file
    std::ifstream file( filename.c_str(), std::ios::in );

    if( file.fail() ) {
        Error("Couldn't load " + filename)
        return false;
    }

    try {
        while( !file.eof() ) {
            string token, buffer;
            int version;
            int i;

            // Read next token
            file >> token;

            if( token == "//" ) {
                // This is the begining of a comment
                // Eat up rest of the line
                std::getline( file, buffer );
            } else if( token == "MD5Version" ) {
                file >> version;

                if( version != kMd5Version ) {
                    Error("MD5Version of " + filename + " is " + NTOOLS::itos(version) + " and should be " + NTOOLS::itos(kMd5Version))
                    file.close();
                    return false;
                }
            } else if( token == "numFrames" ) {
                file >> _numFrames;
                _skelframes.reserve( _numFrames );
                _bboxes.reserve( _numFrames );
            } else if( token == "numJoints" ) {
                file >> numJoints;
                jointInfos.reserve( numJoints );
                baseFrame.reserve( numJoints );
            } else if( token == "frameRate" ) {
                file >> _frameRate;
            } else if( token == "numAnimatedComponents" ) {
                file >> numAnimatedComponents;
                animFrameData.reserve( numAnimatedComponents );
            } else if( token == "hierarchy" ) {
                // Read all joint infos
                file >> token; // "{"

                // Read all joint infos
                for( i = 0; i < numJoints; ++i ) {
                    JointInfo_t jinfo;

                    file >> jinfo.name;
                    file >> jinfo.parent;
                    file >> jinfo.flags;
                    file >> jinfo.startIndex;

                    jointInfos.push_back( jinfo );

                    // Eat up rest of the line
                    std::getline( file, buffer );
                }

                file >> token; // "}"
            } else if( token == "bounds" ) {
                file >> token; // "{"

                // Read frame bounds
                for( unsigned int i = 0; i < _numFrames; ++i ) {
                    BoundingBox_t *bbox = new BoundingBox_t;

                    file >> token; // "("
                    file >> bbox->min._x;
                    file >> bbox->min._y;
                    file >> bbox->min._z;
                    file >> token; // ")"
                    file >> token; // "("
                    file >> bbox->max._x;
                    file >> bbox->max._y;
                    file >> bbox->max._z;
                    file >> token; // ")"

                    _bboxes.push_back( bbox );
                }

                file >> token; // "}"
            } else if( token == "baseframe" ) {
                // We should have an opening bracket for the baseframe joint list
                file >> token; // "{"

                // Read baseframe data
                for( i = 0; i < numJoints; ++i ) {
                    BaseFrameJoint_t bfj;

                    file >> token; // "("
                    file >> bfj.pos._x;
                    file >> bfj.pos._y;
                    file >> bfj.pos._z;
                    file >> token; // ")"
                    file >> token; // "("
                    file >> bfj.orient._x;
                    file >> bfj.orient._y;
                    file >> bfj.orient._z;
                    file >> token; // ")"

                    baseFrame.push_back( bfj );

                    // Eat up rest of the line
                    std::getline( file, buffer );
                }

                file >> token; // "}"
            } else if( token == "frame" ) {
                int frameIndex;
                file >> frameIndex;
                file >> token; // "{"

                animFrameData.clear();

                // Read all frame data
                float afvalue;
                for( i = 0; i < numAnimatedComponents; ++i ) {
                    // NOTE about coding style: beeuuarg *vomit*
                    file >> afvalue;
                    animFrameData.push_back( afvalue );
                }

                file >> token; // "}"

                // Build skeleton for this frame
                buildFrameSkeleton( jointInfos, baseFrame, animFrameData, frameIndex );
            }
        }

        file.close();

        // Find animation's name
        string szfile = filename;
        string::size_type start = szfile.find_last_of( "/" );
        string::size_type end = szfile.find_last_of( ".md5anim" );
        _name = szfile.substr( start + 1, end - start - 8 );

        Info("ModelAnimation: " + filename + " loaded")
        return true;
    } catch( std::ios_base::failure &err ) {
        Error("Couldn't load " + filename + " script file. Reason: " + err.what())
        return false;
    }
}


// --------------------------------------------------------------------------
// Md5Animation::buildFrameSkeleton
//
// Build a skeleton for a particular frame.  The skeleton is transformed
// by the given modelview matrix so that it is possible to obtain the
// skeleton in absolute coordinates.
//
// NOTE: You must delete yourself the skeleton returned!
// --------------------------------------------------------------------------

void Md5Animation::buildFrameSkeleton( vector<JointInfo_t> &jointInfos,
                                       vector<BaseFrameJoint_t> &baseFrame,
                                       vector<float> &animFrameData,
                                       int iframe ) {
    // Allocate memory for this frame
    Md5Skeleton *skelframe = new Md5Skeleton;
    _skelframes.push_back( skelframe );

    skelframe->setNumJoints( jointInfos.size() );

    // Setup all joints for this frame
    for( unsigned int i = 0; i < jointInfos.size(); ++i ) {
        BaseFrameJoint_t *baseJoint = &baseFrame[i];
        Vector3f animatedPos = baseJoint->pos;
        Quaternionf animatedOrient = baseJoint->orient;
        int j = 0;

        if( jointInfos[i].flags & 1 ) { // Tx
            animatedPos._x = animFrameData[ jointInfos[i].startIndex + j ];
            ++j;
        }

        if( jointInfos[i].flags & 2 ) { // Ty
            animatedPos._y = animFrameData[ jointInfos[i].startIndex + j ];
            ++j;
        }

        if( jointInfos[i].flags & 4 ) { // Tz
            animatedPos._z = animFrameData[ jointInfos[i].startIndex + j ];
            ++j;
        }

        if( jointInfos[i].flags & 8 ) { // Qx
            animatedOrient._x = animFrameData[ jointInfos[i].startIndex + j ];
            ++j;
        }

        if( jointInfos[i].flags & 16 ) { // Qy
            animatedOrient._y = animFrameData[ jointInfos[i].startIndex + j ];
            ++j;
        }

        if( jointInfos[i].flags & 32 ) { // Qz
            animatedOrient._z = animFrameData[ jointInfos[i].startIndex + j ];
            ++j;
        }

        // Compute orient quaternion's w value
        animatedOrient.computeW();

        // NOTE: we assume that this joint's parent has
        // already been calculated, i.e. joint's ID should
        // never be smaller than its parent ID.
        Md5Joint_t *thisJoint = new Md5Joint_t;
        skelframe->insertJoint( i, thisJoint );

        int parent = jointInfos[i].parent;
        thisJoint->parent = parent;
        thisJoint->name = jointInfos[i].name;

        // has parent?
        if( thisJoint->parent < 0 ) {
            thisJoint->pos = animatedPos;
            thisJoint->orient = animatedOrient;
        } else {
            Md5Joint_t *parentJoint = skelframe->getJoint( parent );

            parentJoint->orient.rotate( animatedPos );
            thisJoint->pos = animatedPos + parentJoint->pos;

            thisJoint->orient = parentJoint->orient * animatedOrient;
            thisJoint->orient.normalize();
        }
    }
}


// --------------------------------------------------------------------------
// Md5Animation::interpolate
//
// Build an interpolated skeleton given two frame indexes and an
// interpolation percentage.
// --------------------------------------------------------------------------

void Md5Animation::interpolate( unsigned int frameA, unsigned int frameB,
                                float interp, Md5Skeleton *out ) {
    for( unsigned int i = 0; i < out->getNumJoints(); ++i ) {
        Md5Joint_t *pJointA = _skelframes[ frameA ]->getJoint(i);
        Md5Joint_t *pJointB = _skelframes[ frameB ]->getJoint(i);
        Md5Joint_t *pFinalJoint = out->getJoint(i);

        pFinalJoint->parent = pJointA->parent;
        pFinalJoint->pos = pJointA->pos + interp * (pJointB->pos - pJointA->pos);
        pFinalJoint->orient = Slerp( pJointA->orient, pJointB->orient, interp );
    }
}


/////////////////////////////////////////////////////////////////////////////
//
// class Md5Object implementation.
//
/////////////////////////////////////////////////////////////////////////////


// --------------------------------------------------------------------------
// Md5Object::~Md5Object
//
// Destructor.  Free all data allocated for the objct.
// --------------------------------------------------------------------------

Md5Object::~Md5Object( void ) {
    if( _animatedSkeleton ) {
        delete _animatedSkeleton;
    }
}


// --------------------------------------------------------------------------
// Md5Object::setMd5Model
//
// Attach MD5 Model to Object.
// --------------------------------------------------------------------------

void Md5Object::setMd5Model( Md5Model *pModel ) {
    if( _model != pModel ) {
        _model = pModel; // Link to the model

        // Delete previous skeletons because the new
        // model is different and its skeleton can hold
        // more joints.
        if( _animatedSkeleton ) {
            delete _animatedSkeleton;
        }

        // Copy skeleton joints name
        _animatedSkeleton = _model->getBaseSkeleton()->clone();
    }
}


// --------------------------------------------------------------------------
// Md5Object::setAnim
//
// Set animation to play.
// --------------------------------------------------------------------------

void Md5Object::setAnim( const string &name ) {
    if( _model && _currAnimName != name ) {
        // Retrieve animation from model's animation list
        if( (_currAnim = _model->getAnim( name )) ) {

            // Compute max frame time and reset _last_time
            _max_time =  1.0 / static_cast<double>(_currAnim->getFrameRate());
            _last_time = 0.0;
        } else {
            if( _animatedSkeleton ) {
                delete _animatedSkeleton;
            }

            // Rebuild animated skeleton with model's base skeleton
            _animatedSkeleton = _model->getBaseSkeleton()->clone();
        }

        _currAnimName = name;

        // Reset current and next frames
        _currFrame = 0;
        _nextFrame = _currFrame + 1;
    }
}


// --------------------------------------------------------------------------
// Md5Object::animate
//
// Compute current and next frames for model's animation.
// --------------------------------------------------------------------------

void Md5Object::animate( double dt ) {
    // Animate only if there is an animation...
    if( _currAnim ) {
        _last_time += dt;

        // Move to next frame
        if( _last_time >= _max_time ) {
            _currFrame++;
            _nextFrame++;
            _last_time = 0.0f;

            unsigned int maxFrames = _currAnim->getMaxFrames();

            if( _currFrame > maxFrames ) {
                _currFrame = 0;
            }

            if( _nextFrame > maxFrames ) {
                _nextFrame = 0;
            }
        }
    }
}


// --------------------------------------------------------------------------
// Md5Object::computeBoundingBox
//
// Compute object's oriented bounding box.
// --------------------------------------------------------------------------

void Md5Object::computeBoundingBox( void ) {
    BoundingBox_t bbox;

    if( _currAnim ) {
        // Interpolate frames' bounding box in order
        // to get animated AABB in object space
        BoundingBox_t *boxA, *boxB;
        boxA = _currAnim->getFrameBounds( _currFrame );
        boxB = _currAnim->getFrameBounds( _nextFrame );

        float interp = _last_time * _currAnim->getFrameRate();

        bbox.min = boxA->min + (boxB->min - boxA->min) * interp;
        bbox.max = boxA->max + (boxB->max - boxA->max) * interp;
    } else {
        // Get bind-pose model's bouding box
        bbox = _model->getBindPoseBoundingBox();
    }

    // Compute oriented bounding box
    _bbox.world = _modelView;
    _bbox.center = Vector3f( (bbox.max._x + bbox.min._x) * 0.5f,
                             (bbox.max._y + bbox.min._y) * 0.5f,
                             (bbox.max._z + bbox.min._z) * 0.5f );
    _bbox.extent = Vector3f( bbox.max._x - _bbox.center._x,
                             bbox.max._y - _bbox.center._y,
                             bbox.max._z - _bbox.center._z );
}


// --------------------------------------------------------------------------
// Md5Object::prepare
//
// Prepare object to be drawn.
// --------------------------------------------------------------------------

void Md5Object::prepare( bool softwareTransformation ) {
    _softwareTransformation = softwareTransformation;

    if( _renderFlags & kDrawModel ) {
        if( _currAnim ) {
            // Interpolate current and next frame skeletons
            float interp = _last_time * _currAnim->getFrameRate();
            _currAnim->interpolate( _currFrame, _nextFrame, interp,
                                    _animatedSkeleton );
        } else {
            // If there is no animated skeleton, fall to
            // model's base skeleton
            if( _animatedSkeleton ) {
                delete _animatedSkeleton;
            }

            _animatedSkeleton = _model->getBaseSkeleton()->clone();
        }

        if( _softwareTransformation || _renderFlags & kDrawJointLabels ) {
            // Force software transformation if joint labels have
            // to be drawn
            _softwareTransformation = true;

            Quaternionf rot;
            rot.fromMatrix( _modelView );

            // Applly Model-View transformation for each joint
            for( unsigned int i = 0; i < _animatedSkeleton->getNumJoints(); ++i ) {
                Md5Joint_t *thisJoint = _animatedSkeleton->getJoint(i);

                thisJoint->pos = _modelView * thisJoint->pos;
                thisJoint->orient = rot * thisJoint->orient;
            }
        }

        // Setup vertex arrays
        _model->prepare( *_animatedSkeleton );
    }
}


// --------------------------------------------------------------------------
// Md5Object::render
//
// Draw object.
// --------------------------------------------------------------------------

void Md5Object::render( void ) {
    glPushMatrix();

    glScalef(0.2, 0.2, 0.2);
    //glRotatef( -90.0, 1.0, 0.0, 0.0 );
    //glRotatef( 90.0, 0.0, 1.0, 0.0 );
    glRotatef(-90.0, 1.0, 0.0, 0.0);

    /*if( !_softwareTransformation ) {
        glMultMatrixf( _modelView._m );
    }*/

    glPushAttrib( GL_POLYGON_BIT | GL_ENABLE_BIT );
    glFrontFace( GL_CW );
    glCullFace( GL_BACK );
    glDisable( GL_BLEND );

    if( _renderFlags & kDrawModel ) {
        _model->drawModel();
    }

    if( _renderFlags & kDrawSkeleton ) {
        glDisable( GL_TEXTURE_2D );
        glDisable( GL_LIGHTING );

        _animatedSkeleton->draw( _modelView, _renderFlags & kDrawJointLabels );
    }
    glPopAttrib();
    glPopMatrix();
}
