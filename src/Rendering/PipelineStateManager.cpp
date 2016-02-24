#include "PipelineStateManager.h"

void PipelineStateManager::PipelineStateObject::load(PipelineStateObject &oldState)
{
	assertDepthTestIsSet(depthTestState);

	if (oldState.depthTestState != depthTestState)
		updateDepthTestState(depthTestState);
}

bool PipelineStateManager::PipelineStateObject::assertDepthTestIsSet(const DepthTestState &state)
{
	GLint currentDepthFunc;
	glGetIntegerv(GL_DEPTH_FUNC, &currentDepthFunc);

	bool isCorrect = (glIsEnabled(GL_DEPTH_TEST) == (state.active ? GL_TRUE : GL_FALSE)) && currentDepthFunc == state.depthFunc;

	return isCorrect;
}

void PipelineStateManager::PipelineStateObject::updateDepthTestState(const DepthTestState &newState)
{
	newState.active ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	glDepthFunc(newState.depthFunc);
}

void PipelineStateManager::loadPipelineState(PipelineStateObject &newState)
{
	newState.load(m_currentState);
}

