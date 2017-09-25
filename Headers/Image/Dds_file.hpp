#ifdef DDS_FILE_HPP
#warning You should only include Nine.hpp to use this library
#else
#define DDS_FILE_HPP

#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
  (GLuint)( \
    (((GLuint)(GLubyte)(ch3) << 24) & 0xFF000000) | \
    (((GLuint)(GLubyte)(ch2) << 16) & 0x00FF0000) | \
    (((GLuint)(GLubyte)(ch1) <<  8) & 0x0000FF00) | \
     ((GLuint)(GLubyte)(ch0)        & 0x000000FF) )

#define FOURCC_DXT1 MAKEFOURCC('D', 'X', 'T', '1')
#define FOURCC_DXT3 MAKEFOURCC('D', 'X', 'T', '3')
#define FOURCC_DXT5 MAKEFOURCC('D', 'X', 'T', '5')

struct gl_texture_t {
    GLsizei width;
    GLsizei height;

    GLenum format;
    GLint internalFormat;
    GLuint id;

    GLubyte *texels;

    GLint numMipmaps;

};

struct DDPixelFormat {
    GLuint size;
    GLuint flags;
    GLuint fourCC;
    GLuint bpp;
    GLuint redMask;
    GLuint greenMask;
    GLuint blueMask;
    GLuint alphaMask;

};


struct DDSCaps {
    GLuint caps;
    GLuint caps2;
    GLuint caps3;
    GLuint caps4;

};


struct DDColorKey {
    GLuint lowVal;
    GLuint highVal;

};


struct DDSurfaceDesc {
    GLuint size;
    GLuint flags;
    GLuint height;
    GLuint width;
    GLuint pitch;
    GLuint depth;
    GLuint mipMapLevels;
    GLuint alphaBitDepth;
    GLuint reserved;
    GLuint surface;

    DDColorKey ckDestOverlay;
    DDColorKey ckDestBlt;
    DDColorKey ckSrcOverlay;
    DDColorKey ckSrcBlt;

    DDPixelFormat format;
    DDSCaps caps;

    GLuint textureStage;

};

#endif // DDS_FILE_HPP
