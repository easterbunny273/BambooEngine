#ifndef __BAMBOOLIB_RESSOURCE_FBO_H_
#define __BAMBOOLIB_RESSOURCE_FBO_H_

#include <memory>
#include <vector>

#include <GL/glew.h>

class FBO;

using FBOPtr = std::shared_ptr<FBO>;

class FBO
{
public:
    static std::unique_ptr<FBO> create(unsigned int width, unsigned int height, unsigned int multisampling, unsigned int numColorTextures, bool hasDepthBuffer);
    virtual ~FBO();

    bool bindFBO();
	static void unbindFBO();

    bool blitFBO();

    auto getWidth() const {
        return m_width;
    };

    auto getHeight() const{
        return m_height;
    }

    GLuint getColorTexture(unsigned int index);

protected:
    FBO(unsigned width, unsigned height, GLuint fbo, std::vector<GLuint> colorTextures, GLint depthRenderBuffer);
    FBO(unsigned width, unsigned height, unsigned multisampling, GLuint fbo, GLuint multisampleFbo, std::vector<GLuint> colorTextures, std::vector<GLuint> multisampledColorTextures, GLint depthRenderBuffer);

    unsigned int m_width, m_height, m_multisampling;
    GLuint               m_fboId, m_multisampleFboId;
    std::vector<GLuint>  m_colorTextures, m_multisampledColorTextures;
    GLint               m_optionalDepthRenderBuffer;

};

#endif