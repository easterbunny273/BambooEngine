#pragma once

#ifndef __BAMBOOENGINE_GLCONTEXT_H_
#define __BAMBOOENGINE_GLCONTEXT_H_

#include "FboManager.h"
#include "PipelineStateManager.h"

class GLContext
{
public:
	FboManager & getFboManager() { return m_fboManager; };
	PipelineStateManager & getStateWrapper() { return m_stateManager; }

private:
	FboManager m_fboManager;
	PipelineStateManager m_stateManager;
};

#endif