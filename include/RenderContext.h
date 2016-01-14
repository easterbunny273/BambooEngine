#pragma once

#ifndef __BAMBOOENGINE_RENDERCONTEXT_H_
#define __BAMBOOENGINE_RENDERCONTEXT_H_

#include <chrono>
#include <ctime>

// the render context provides contextual information and is *not* a rendering/opengl context!
class RenderContext
{
public:
	RenderContext() { m_startingTime = getTime(); }
	// just dummy for now
	unsigned long long getTime() 
	{
		return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
	}

	unsigned long long getTimeFromStart() { return getTime() - m_startingTime; }

private:
	unsigned long long m_startingTime;
};

#endif