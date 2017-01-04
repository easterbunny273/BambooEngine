#ifdef _WIN32

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/gl.h>

#include "RenderTarget_PBuffer.h"
#include <cassert>
#include <string>
#include <memory>

std::unique_ptr<RenderTarget_PBuffer> RenderTarget_PBuffer::create(unsigned int _width, unsigned int _height, unsigned int multisampling)
{
    auto currentDC = wglGetCurrentDC();
    auto currentContext = wglGetCurrentContext();

    int     pixelFormats;
    int     intAttrs[32] = {   /* WGL_RED_BITS_ARB,8,
                                WGL_GREEN_BITS_ARB,8,
                                WGL_BLUE_BITS_ARB,8,
                                WGL_ALPHA_BITS_ARB,8,*/
                                WGL_COLOR_BITS_ARB, 32,
                                WGL_STENCIL_BITS_ARB, 8,
                                WGL_DEPTH_BITS_ARB, 24,
                                WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                                //WGL_DRAW_TO_PBUFFER_ARB, GL_TRUE,
                                WGL_BIND_TO_TEXTURE_RGBA_ARB, GL_TRUE,
                                WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
                                WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
                                WGL_DOUBLE_BUFFER_ARB,GL_FALSE,
                                WGL_SAMPLE_BUFFERS_ARB, (multisampling>0) ? GL_TRUE : GL_FALSE,
                                WGL_SAMPLES_ARB, int(multisampling),
                                0 }; // 0 terminate the list			

    unsigned int numFormats = 0;
    // get an acceptable pixel format to create the PBuffer with
    if (wglChoosePixelFormatARB(currentDC, intAttrs, NULL, 1, &pixelFormats, &numFormats) == FALSE)
    {
		std::string errorMessage;
		errorMessage.append("wglChoosePixelFormatARB returned ");
		errorMessage.append(std::to_string(GetLastError()));
        assert(false); // GetLastError will tell us why it failed			
    }
    if (numFormats == 0) // no supported formats, we need to change the parameters to something acceptable
    {
        assert(false);
    }

    // Set some p - buffer attributes so that we can use this p - buffer as a 2d texture target
    const int attributes[] = { WGL_TEXTURE_FORMAT_ARB,  WGL_TEXTURE_RGBA_ARB, // p-buffer will have RBA texture format
        WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_2D_ARB, 0 }; // Of texture target will be GL_TEXTURE_2D
                                                         // the size of the PBuffer must be the same size as the texture
    auto PBuffer = wglCreatePbufferARB(currentDC, pixelFormats, _width, _height, attributes);
    auto pbufferDC = wglGetPbufferDCARB(PBuffer);
    auto hGlRc = wglCreateContext(pbufferDC);

    //query dimensions of created texture to make sure it was created right
    int width, height;
    wglQueryPbufferARB(PBuffer, WGL_PBUFFER_WIDTH_ARB, &width);
    wglQueryPbufferARB(PBuffer, WGL_PBUFFER_HEIGHT_ARB, &height);
    assert(width == _width && height == _height);

    auto result = wglShareLists(currentContext, hGlRc);

    // switch from the screen context to the texture context to set up the openGL stuff for that context
    wglMakeCurrent(pbufferDC, hGlRc);

    // create color texture
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    GLuint colorTexture;
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    wglMakeCurrent(currentDC, currentContext);

    return std::unique_ptr<RenderTarget_PBuffer>(new RenderTarget_PBuffer(pbufferDC, hGlRc, currentDC, currentContext, colorTexture, width, height));
}

bool RenderTarget_PBuffer::pepareForRendering()
{
    auto ok = wglMakeCurrent(m_hdc, m_hglrc);

    if (ok)
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    return ok ? true : false;
}

void RenderTarget_PBuffer::onRenderingFinished()
{
    
    glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_pBufferForReadback);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pBufferForReadback);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    wglMakeCurrent(m_originalDC, m_originalGlRC);
}

#endif
