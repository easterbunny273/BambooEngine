#pragma once

#ifndef __BAMBOOENGINE_IRENDERTARGET_H_

#include "IRenderTarget.h"

#include <GL/glew.h>

class IRenderTarget_Texture : public IRenderTarget
{
public:
    virtual ~IRenderTarget_Texture() {};

    virtual GLuint getColorTexture() const = 0;
};

#endif