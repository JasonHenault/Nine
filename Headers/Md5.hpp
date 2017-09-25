/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// Md5.h -- by David Henry
// last modification: aug. 11, 2005
//
// Declarations for MD5 model object, mesh, anim and skeleton classes.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MD5_H__
#define __MD5_H__

#ifdef _WIN32
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // _WIN32

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <GL/gl.h>

#include "../Mathlib.h"

using std::string;
using std::vector;
using std::map;

// Forward declarations
class Md5Skeleton;
class Md5Mesh;
class Md5Model;
class Md5Animation;
class Md5Object;


extern const int kMd5Version;

// OpenGL vector types
typedef GLfloat vec2_t[2];
typedef GLfloat vec3_t[3];
typedef GLfloat vec4_t[4];

struct Md5Joint_t
{
  string name;
  int parent;

  Vector3f pos;
  Quaternionf orient;

  // Provide operator < for depth sorting
  bool operator<( const Md5Joint_t &j ) const {
	return (pos._z < j.pos._z);
  }
};


struct Md5Vertex_t
{
  float st[2]; // Texture coordinates

  int startWeight; // Start index weights
  int countWeight; // Number of weights
};


struct Md5Triangle_t
{
  int index[3]; // Vertex indices
};


struct Md5Weight_t
{
  int joint; // Joint index
  float bias;

  Vector3f pos;
  Vector3f norm;
  Vector3f tan;
};


struct BoundingBox_t {
  Vector3f min;
  Vector3f max;
};


struct OBBox_t {
  Matrix4x4f world;

  Vector3f center;
  Vector3f extent;
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md5Skeleton - Skeleton model data class.
//
/////////////////////////////////////////////////////////////////////////////

class Md5Skeleton
{
 public:
  // Constructor/Destructor
  Md5Skeleton( void )
	: _numJoints(0) { }
  ~Md5Skeleton( void );

 public:
  // Public interface
  bool load( std::ifstream &file );
  void draw( const Matrix4x4f &modelView, bool labelJoints );

  Md5Joint_t *getJoint( unsigned int index ) {
	return _joints[ index ];
  }

  // Insert a joint at a given index from the beginning
  void insertJoint( unsigned int index, Md5Joint_t *thisJoint ) {
	_joints.insert( _joints.begin() + index, thisJoint );
  }

  // Set the number of joints and reserve memory to hold them all
  void setNumJoints( unsigned int numJoints ) {
	_numJoints = numJoints;
	_joints.reserve( _numJoints );
  }

  // Fill the joint vector with new joints
  void preAllocJoints( void ) {
	for( unsigned int i = 0; i < _numJoints; ++i ) {
	  _joints.push_back( new Md5Joint_t );
	}
  }

  unsigned int getNumJoints( void ) {
	return _numJoints;
  }

  Md5Skeleton *clone( void ) {
	Md5Skeleton *cpy = new Md5Skeleton;
	cpy->setNumJoints( _numJoints );

	for( unsigned int i = 0; i < _numJoints; ++i ) {
	  cpy->insertJoint( i, new Md5Joint_t( *_joints[i] ) );
	}

	return cpy;
  }

 private:
  // Member variables
  unsigned int _numJoints;
  vector<Md5Joint_t *> _joints;
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md5Mesh - Mesh data class.
//
/////////////////////////////////////////////////////////////////////////////

class Md5Mesh
{
 public:
  // Public internal types/enums
  enum {
	kHide = 1, // Skip mesh
	kNoDraw, // Don't draw but prepare vertices
	kShow // Draw mesh
  };

 public:
  // Constructor/Destructor
  Md5Mesh( void )
	: _renderState(kShow), _numVerts(0), _numTris(0), _numWeights(0),
	  _vertexArray(NULL), _normalArray(NULL), _tangentArray(NULL),
	  _texCoordArray(NULL), _vertIndices(NULL), _decal(NULL), _specMap(NULL),
	  _normalMap(NULL), _heightMap(NULL) { }
  ~Md5Mesh( void );

 public:
  // Public interface
  bool load( std::ifstream &file );
  void setupVertexArrays( Md5Skeleton *skel );
  void setupTexCoordArray( void );
  void computeWeightNormals( Md5Skeleton &skel );
  void computeWeightTangents( Md5Skeleton &skel );
  void computeBoundingBox( Md5Skeleton &skel );
  void renderVertexArrays( void );
  void drawNormals( void );

  const string &getName( void ) const {
	return _name;
  }

  const BoundingBox_t &getBoundingBox( void ) const {
	return _BoundingBox;
  }

  // Hide/NoDraw/Show state
  void setState( int state ) {
	_renderState = state;
  }

  // Texture setters
  void setDecalMap( const std::string filepath ) {
      _decal = new NRENDERING::Image(filepath);
  }

  void setSpecularMap( const std::string filepath ) {
      _specMap = new NRENDERING::Image(filepath);
   }

  void setNormalMap( const std::string filepath ) {
      _normalMap = new NRENDERING::Image(filepath);
   }

  void setHeightMap( const std::string filepath ) {
      _heightMap = new NRENDERING::Image(filepath);
   }

  // Mesh render state
  bool hiden( void ) const { return (_renderState == kHide);  }
  bool noDraw( void ) const { return (_renderState == kNoDraw); }
  bool show( void ) const { return (_renderState == kShow); }

 private:
  // Internal functions
  void allocVertexArrays( void );
  void freeVertexArrays( void );

  void loadTextures( void );

 private:
  // Member variables
  string _name;
  string _shader;
  int _renderState;

  BoundingBox_t _BoundingBox;

  unsigned int _numVerts;
  unsigned int _numTris;
  unsigned int _numWeights;

  // Original mesh data
  vector<Md5Vertex_t *> _verts;
  vector<Md5Triangle_t *> _tris;
  vector<Md5Weight_t *> _weights;

  // Final mesh data; vertex arrays for fast rendering
  vec3_t *_vertexArray;
  vec3_t *_normalArray;
  vec3_t *_tangentArray;
  vec2_t *_texCoordArray;
  GLuint *_vertIndices;

  // Textures
  NRENDERING::Image *_decal;
  NRENDERING::Image *_specMap;
  NRENDERING::Image *_normalMap;
  NRENDERING::Image *_heightMap;
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md5Model - MD5 Mesh Model class.
//
/////////////////////////////////////////////////////////////////////////////

class Md5Model
{
 public:
  // Constructor/Destructor
  Md5Model( void )
	: _numJoints(0), _numMeshes(0) { }
  ~Md5Model( void );

 public:
  // Internal type definitions
  typedef map<string, Md5Animation *> AnimMap;

 public:
  // Public interface
  bool loadModel( const string &filename );
  void prepare( Md5Skeleton &skel );
  void drawModel( void );

  bool addAnim( const string &filename );
  Md5Animation *getAnim( const string &name );

  unsigned int getNumJoints( void ) {
	return _numJoints;
  }

  Md5Skeleton *getBaseSkeleton( void ) {
	return &_baseSkeleton;
  }

  const AnimMap &getAnimList( void ) const {
	return _animList;
  }

  const BoundingBox_t &getBindPoseBoundingBox( void ) const {
	return _bindPoseBox;
  }

  void setMeshRenderState( const string &name, int state ) {
	if( Md5Mesh *mesh = getMeshByName( name ) ) {
	  mesh->setState( state );
	}
  }

  void setMeshDecalMap( const string &name, const std::string filepath ) {
	if( Md5Mesh *mesh = getMeshByName( name ) ) {
	  mesh->setDecalMap( filepath );
	}
  }

  void setMeshSpecularMap( const string &name, const std::string filepath ) {
	if( Md5Mesh *mesh = getMeshByName( name ) ) {
	  mesh->setSpecularMap( filepath );
	}
  }

  void setMeshNormalMap( const string &name, const std::string filepath ) {
	if( Md5Mesh *mesh = getMeshByName( name ) ) {
	  mesh->setNormalMap( filepath );
	}
  }

  void setMeshHeightMap( const string &name, const std::string filepath ) {
	if( Md5Mesh *mesh = getMeshByName( name ) ) {
	  mesh->setHeightMap( filepath );
	}
  }

 private:
  // Internal functions
  void computeBindPoseBoundingBox( void );

  // Check if an animation is valid for this model,
  // i.e. anim's skeleton matches with model's skeleton
  bool validityCheck( Md5Animation *anim );

  Md5Mesh *getMeshByName( const string &name );

 private:
  // Member variables
  unsigned int _numJoints;
  unsigned int _numMeshes;

  std::string _modelPath;

  Md5Skeleton _baseSkeleton;

  vector<Md5Mesh *> _meshes;
  AnimMap _animList;

  BoundingBox_t _bindPoseBox;
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md5Animation - MD5 model animation class.
//
/////////////////////////////////////////////////////////////////////////////

class Md5Animation
{
 public:
  // Constructor/Destructor
  Md5Animation( void )
	: _numFrames(0), _frameRate(0) { }
  ~Md5Animation( void );

 public:
  // Public interface
  bool load( const string &filename );

  void interpolate( unsigned int frameA, unsigned int frameB,
					float interp, Md5Skeleton *out );

  // Accessors
  unsigned int getMaxFrames( void ) const {
	return _numFrames - 1;
  }

  const string &getName( void ) const {
	return _name;
  }

  unsigned int getFrameRate( void ) const {
	return _frameRate;
  }

  Md5Skeleton *getFrame( unsigned int frame ) {
	assert( frame < _numFrames );
	return _skelframes[ frame ];
  }

  BoundingBox_t *getFrameBounds( unsigned int frame ) {
	return _bboxes[ frame ];
  }

 private:
  // Internal type
  struct JointInfo_t {
	string name;
	int parent;
	int flags;
	int startIndex;
  };

  struct BaseFrameJoint_t {
	Vector3f pos;
	Quaternionf orient;
  };

 private:
  // Internal functions
  void buildFrameSkeleton( vector<JointInfo_t> &jointInfos,
	vector<BaseFrameJoint_t> &baseFrame, vector<float> &animFrameData, int iframe );

 private:
  // Member variables
  unsigned int _numFrames;
  unsigned int _frameRate;

  string _name;

  // Store each frame as a skeleton
  vector<Md5Skeleton *> _skelframes;

  // Bounding boxes for each frame
  vector<BoundingBox_t *> _bboxes;
};


/////////////////////////////////////////////////////////////////////////////
//
// class Md5Object - MD5 object class.
//
/////////////////////////////////////////////////////////////////////////////

class Md5Object
{
 public:
  // Public internal types/enums
  enum {
	kDrawModel = 1,
	kDrawSkeleton = 2,
	kDrawJointLabels = 4
  };

 public:
  // Contructor/Destructor
  Md5Object( void )
	: _model(NULL), _animatedSkeleton(NULL), _softwareTransformation(false),
	  _currAnim(NULL), _currFrame(0), _nextFrame(1), _last_time(0.0),
	  _max_time(0.0), _renderFlags(kDrawModel) { }
  virtual ~Md5Object( void );

 public:
  // Public interface
  void setMd5Model( Md5Model *pModel );
  void setAnim( const string &name );
  void setModelViewMatrix( const Matrix4x4f &modelView ) {
	_modelView = modelView;
  }

  void setRenderFlags( int flags ) {
	_renderFlags = flags;
  }

  int getRenderFlags( void ) {
	return _renderFlags;
  }

  Md5Model *getModelPtr( void ) {
	return _model;
  }

  const string getCurrAnimName( void ) const {
	if( _currAnim ) {
	  return _currAnim->getName();
	}
	else {
	  return "NULL";
	}
  }

  bool isLastFrame() {
      if (_currFrame == 0 && _last_time == 0)
            return true;

      return false;
  }

  const OBBox_t &getBoundingBox( void ) const {
	return _bbox;
  }

  void animate( double dt );
  void computeBoundingBox( void );
  void prepare( bool softwareTransformation );
  void render( void );

 protected:
  // Member variables;
  Md5Model *_model;
  Md5Skeleton *_animatedSkeleton;

  Matrix4x4f _modelView;
  bool _softwareTransformation;

  Md5Animation *_currAnim;
  unsigned int _currFrame;
  unsigned int _nextFrame;

  double _last_time;
  double _max_time;

  int _renderFlags;

  string _currAnimName;

  //BoundingBox_t _bbox;
  OBBox_t _bbox;
};

#endif // __MD5_H__
