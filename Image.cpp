#include "Nine.hpp"
#include "FastAccess.hpp"

using namespace NRENDERING;

int actTex = 0;

Image::Image () {
    _actTex = new int;
    _tex = new GLuint;

    *_actTex = actTex;
    actTex++;
}

Image::Image (const std::string &path) {
    _actTex = new int;
    _tex = new GLuint;

    *_actTex = actTex;
    actTex++;

    Load(path);
}

Image::~Image () {
    if (NRESOURCES::ResMng::Instance().Unregister(_path) == success) {
        delete _actTex;
        delete _tex;

        glDeleteTextures (1, _tex);
    }
}

int Image::Load (const std::string &path) {
    gl_texture_t *compressed_texture = NULL;
    GLsizei mipWidth, mipHeight, mipSize;
    int blockSize, offset;
    GLint mip;

    /* read texture from file */
    compressed_texture = ReadFile(NTOOLS::DecodePath(path));

    if (compressed_texture == NULL)
        return error;

    if (compressed_texture && compressed_texture->texels) {
        /* generate new texture */
        glGenTextures (1, &compressed_texture->id);
        glBindTexture (GL_TEXTURE_2D, compressed_texture->id);

        /* setup some parameters for texture filters and mipmapping */
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        mipWidth = compressed_texture->width;
        mipHeight = compressed_texture->height;
        blockSize = (compressed_texture->format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
        offset = 0;

        /* upload mipmaps to video memory */
        for (mip = 0; mip < compressed_texture->numMipmaps; ++mip) {
            mipSize = ((mipWidth + 3) / 4) * ((mipHeight + 3) / 4) * blockSize;

            GLeeFuncPtr_glCompressedTexImage2DARB (GL_TEXTURE_2D, mip, compressed_texture->format,
                                    mipWidth, mipHeight, 0, mipSize,
                                    compressed_texture->texels + offset);

            mipWidth = glm::max (mipWidth >> 1, 1);
            mipHeight = glm::max (mipHeight >> 1, 1);

            offset += mipSize;
        }

        *_tex = compressed_texture->id;

        /* opengl has its own copy of pixels */
        free (compressed_texture->texels);
        free (compressed_texture);
    }

    return success;
}

int Image::Bind () {
    if (*_tex != 0) {
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, *_tex);
        return success;
    }

    Error("Image id is incorrect")
    return error;
}

int Image::Active (const int texNb) {
    if (*_tex != 0) {
        glActiveTexture(GL_TEXTURE0 + texNb);
        glBindTexture(GL_TEXTURE_2D, *_tex);
        return success;
    }

    Error("Image id is incorrect")
    return error;
}

GLuint Image::GetId () {
    return *_tex;
}

gl_texture_t* Image::ReadFile (const std::string &path) {
    DDSurfaceDesc ddsd;
    gl_texture_t *texinfo;
    FILE *fp;
    char magic[4];
    int mipmapFactor;
    long bufferSize, curr, end;

    /* open the file */
    fp = fopen (path.c_str(), "rb");
    if (!fp) {
        fprintf (stderr, "error: couldn't open \"%s\"!\n", path.c_str());
        return NULL;
    }

    /* read magic number and check if valid .dds file */
    fread (&magic, sizeof (char), 4, fp);

    if (strncmp (magic, "DDS ", 4) != 0) {
        fprintf (stderr, "the file \"%s\" doesn't appear to be"
                 "a valid .dds file!\n", path.c_str());
        fclose (fp);
        return NULL;
    }

    /* get the surface descriptor */
    fread (&ddsd, sizeof (ddsd), 1, fp);

    texinfo = (gl_texture_t *)malloc (sizeof (gl_texture_t));
    memset (texinfo, 0, sizeof (gl_texture_t));
    texinfo->width = ddsd.width;
    texinfo->height = ddsd.height;
    texinfo->numMipmaps = ddsd.mipMapLevels;

    switch (ddsd.format.fourCC) {
    case FOURCC_DXT1:
        /* DXT1's compression ratio is 8:1 */
        texinfo->format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        texinfo->internalFormat = 3;
        mipmapFactor = 2;
        break;

    case FOURCC_DXT3:
        /* DXT3's compression ratio is 4:1 */
        texinfo->format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        texinfo->internalFormat = 4;
        mipmapFactor = 4;
        break;

    case FOURCC_DXT5:
        /* DXT5's compression ratio is 4:1 */
        texinfo->format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        texinfo->internalFormat = 4;
        mipmapFactor = 4;
        break;

    default:
        /* bad fourCC, unsupported or bad format */
        fprintf (stderr, "the file \"%s\" doesn't appear to be"
                 "compressed using DXT1, DXT3, or DXT5! [%i]\n",
                 path.c_str(), ddsd.format.fourCC);
        free (texinfo);
        fclose (fp);
        return NULL;
    }

    /* calculate pixel data size */
    curr = ftell (fp);
    fseek (fp, 0, SEEK_END);
    end = ftell (fp);
    fseek (fp, curr, SEEK_SET);
    bufferSize = end - curr;

    /* read pixel data with mipmaps */
    texinfo->texels = (GLubyte *)malloc (bufferSize * sizeof (GLubyte));
    fread (texinfo->texels, sizeof (GLubyte), bufferSize, fp);

    /* close the file */
    fclose (fp);
    return texinfo;
}
