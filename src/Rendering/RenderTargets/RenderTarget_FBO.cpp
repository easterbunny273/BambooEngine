#include "RenderTarget_FBO.h"

RenderTarget_FBO::~RenderTarget_FBO()
{

}

std::unique_ptr<RenderTarget_FBO> RenderTarget_FBO::create(unsigned int width, unsigned int height, unsigned int multisampling)
{
    auto createdRessource = FBO::create(width, height, multisampling, 1, true);

    if (createdRessource)
        return std::unique_ptr<RenderTarget_FBO>(new RenderTarget_FBO(createdRessource));
    else
        return nullptr;
}

bool RenderTarget_FBO::pepareForRendering()
{
    bool ok = m_fbo->bindFBO();

    if (ok)
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    return ok;
}

void RenderTarget_FBO::onRenderingFinished()
{
    m_fbo->blitFBO();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget_FBO::RenderTarget_FBO(std::unique_ptr<FBO> &fbo)
    : m_fbo(std::move(fbo))
{
    // only initializer list
}
