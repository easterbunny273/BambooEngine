#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "FBO.h"
#include <cassert>

std::unique_ptr<FBO> FBO::create(unsigned int width, unsigned int height, unsigned int multisampling, unsigned int numColorTextures, bool hasDepthBuffer)
{
    auto createDepthRenderbuffer = [](unsigned int _width, unsigned int _height, unsigned int _multisampling) -> GLuint {
        GLuint depthRenderBuffer;
        glGenRenderbuffers(1, &depthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, _multisampling, GL_DEPTH_COMPONENT24, _width, _height);
        return depthRenderBuffer;
    };

    std::vector<GLuint> colorTextures(numColorTextures);

    glGenTextures(numColorTextures, colorTextures.data());

    for (unsigned int n = 0; n < numColorTextures; ++n)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorTextures[n]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
    }

    GLuint fboId;
    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    // Attach 2D texture to this FBO
    for (unsigned int n = 0; n < numColorTextures; ++n)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextures[0], 0);
    
    GLint optionalDepthRenderBuffer = hasDepthBuffer ? (GLint)createDepthRenderbuffer(width, height, multisampling) : -1;
    
    if (optionalDepthRenderBuffer > 0)
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, optionalDepthRenderBuffer);

    if (multisampling > 0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        std::vector<GLuint> multisampledColorTextures(numColorTextures);
        
        glGenTextures(numColorTextures, multisampledColorTextures.data());
        
        for (unsigned int n = 0; n < numColorTextures; ++n)
        {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampledColorTextures[n]);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisampling, GL_RGBA8, width, height, true);
        }

        GLuint multisampledFboId;
        glGenFramebuffers(1, &multisampledFboId);
        glBindFramebuffer(GL_FRAMEBUFFER, multisampledFboId);

        // Attach 2D texture to this FBO
        for (unsigned int n = 0; n < numColorTextures; ++n)
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampledColorTextures[0], 0);

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

        GLint optionalMSDepthRenderBuffer = hasDepthBuffer ? (GLint)createDepthRenderbuffer(width, height, multisampling) : -1;

        if (optionalMSDepthRenderBuffer > 0)
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, optionalMSDepthRenderBuffer);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        assert(status == GL_FRAMEBUFFER_COMPLETE);

        return std::unique_ptr<FBO>(new FBO(width, height, multisampling, fboId, multisampledFboId, colorTextures, multisampledColorTextures, optionalDepthRenderBuffer));
    }
    else
    {
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        
        assert(status == GL_FRAMEBUFFER_COMPLETE);

        switch (status)
        {
        case GL_FRAMEBUFFER_COMPLETE:
            return std::unique_ptr<FBO>(new FBO(width, height, fboId, colorTextures, optionalDepthRenderBuffer));
            break;
        default:
            return nullptr;
        }
    }


    
    

    return nullptr;
}

bool FBO::bindFBO()
{
    if (m_multisampling > 0)
        glBindFramebuffer(GL_FRAMEBUFFER, m_multisampleFboId);
    else
        glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);

    return true;
}

void FBO::unbindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool FBO::blitFBO()
{
    if (m_multisampling > 0)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fboId);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_multisampleFboId); // Make sure your multisampled FBO is the read framebuffer
        glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    return false;
}

GLuint FBO::getColorTexture(unsigned int index)
{
    assert(index < m_colorTextures.size());

    return m_colorTextures[index];
}

FBO::FBO(unsigned width, unsigned height, GLuint fbo, std::vector<GLuint> colorTextures, GLint depthRenderBuffer)
    : m_width(width), m_height(height), m_multisampling(0), m_fboId(fbo), m_colorTextures(colorTextures), m_optionalDepthRenderBuffer(depthRenderBuffer)
{
    // only initializer list
}

FBO::FBO(unsigned width, unsigned height, unsigned multisampling, GLuint fbo, GLuint multisampleFbo, std::vector<GLuint> colorTextures, std::vector<GLuint> multisampledColorTextures, GLint depthRenderBuffer)
    : m_width(width), m_height(height), m_multisampling(multisampling), m_fboId(fbo), m_multisampleFboId(multisampleFbo), m_colorTextures(colorTextures), m_multisampledColorTextures(multisampledColorTextures), m_optionalDepthRenderBuffer(depthRenderBuffer)
{

}

FBO::~FBO()
{
    glDeleteFramebuffers(1, &m_fboId);
    glDeleteTextures(GLsizei(m_colorTextures.size()), m_colorTextures.data());

    if (m_optionalDepthRenderBuffer >= 0)
    {
        GLuint depthRenderBuffer(m_optionalDepthRenderBuffer);
        glDeleteRenderbuffers(1, &depthRenderBuffer);
    }
}
