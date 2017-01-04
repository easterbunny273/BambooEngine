#ifndef __BAMBOOENGINE_IGLCONTEXT_H_
#define __BAMBOOENGINE_IGLCONTEXT_H_

#include "FboManager.h"
#include "PipelineStateManager.h"

class IGLContext
{
public:
	virtual bool makeCurrent() = 0;

	FboManager & getFboManager() { return m_fboManager; };
private:
	FboManager m_fboManager;
	//PipelineStateManager m_stateManager;
};

#endif