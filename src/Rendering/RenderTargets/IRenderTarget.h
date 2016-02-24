#pragma once

#ifndef __BAMBOOENGINE_IRENDERTARGET_H_

class IRenderTarget
{
public:
	virtual ~IRenderTarget() {};

	virtual unsigned int getWidth() const = 0;
	virtual unsigned int getHeight() const = 0;

	virtual bool pepareForRendering() = 0;
	virtual void onRenderingFinished() = 0;
};

#endif