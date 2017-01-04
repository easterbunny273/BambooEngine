#ifdef _WIN32

#pragma once

#ifndef __BAMBOOENGINE_RENDERTARGET_PBUFFER_H_
#define __BAMBOOENGINE_RENDERTARGET_PBUFFER_H_

#include "IRenderTarget_Texture.h"

#include <memory>

class RenderTarget_PBuffer : public IRenderTarget_Texture
{
public:
    virtual ~RenderTarget_PBuffer() { delete[] m_pBufferForReadback; m_pBufferForReadback = nullptr; };

    static std::unique_ptr<RenderTarget_PBuffer> create(unsigned int width,
        unsigned int height,
        unsigned int multisampling);

    virtual unsigned int getWidth() const { return m_width; };
    virtual unsigned int getHeight() const { return m_height; };

    virtual bool pepareForRendering() override;
    virtual void onRenderingFinished() override;

    virtual GLuint getColorTexture() const override { return m_colorTexture; }

private:
    RenderTarget_PBuffer(HDC _hdc, HGLRC _hglrc, HDC _originalHdc, HGLRC _originalHglRc, GLuint _colorTexture, unsigned int _width, unsigned int _height) : m_hdc(_hdc), m_hglrc(_hglrc), m_originalDC(_originalHdc), m_originalGlRC(_originalHglRc), m_colorTexture(_colorTexture), m_width(_width), m_height(_height) { m_pBufferForReadback = new GLubyte[m_width*m_height*4]; };

    HDC m_hdc, m_originalDC;
    HGLRC m_hglrc, m_originalGlRC;
    GLuint m_colorTexture;
    GLubyte * m_pBufferForReadback;

    unsigned int m_width, m_height;

};

#endif

#endif
