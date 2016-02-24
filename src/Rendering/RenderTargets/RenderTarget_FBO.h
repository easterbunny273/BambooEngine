#pragma once

#ifndef __BAMBOOENGINE_RENDERTARGET_FBO_H_
#define __BAMBOOENGINE_RENDERTARGET_FBO_H_

#include "IRenderTarget_Texture.h"
#include "../FBO.h"

#include <memory>

class RenderTarget_FBO : public IRenderTarget_Texture
{
public:
    virtual ~RenderTarget_FBO();

    static std::unique_ptr<RenderTarget_FBO> create(unsigned int width,
        unsigned int height,
        unsigned int multisampling);

    virtual unsigned int getWidth() const { return m_fbo->getWidth(); };
    virtual unsigned int getHeight() const { return m_fbo->getHeight(); };

    virtual bool pepareForRendering() override;
    virtual void onRenderingFinished() override;

    virtual GLuint getColorTexture() const override { return m_fbo->getColorTexture(0); }
private:
    RenderTarget_FBO(std::unique_ptr<FBO> &fbo);

    std::unique_ptr<FBO> m_fbo;
};

#endif