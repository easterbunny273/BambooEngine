#pragma once

#ifndef __BAMBOOENGINE_PIPELINESTATEMANAGER_H_
#define __BAMBOOENGINE_PIPELINESTATEMANAGER_H_

#include <memory>

#include "GL/glew.h"

#include "PipelineState.h"

class PipelineStateManager
{
public:
	void loadPipelineState(const PipelineState &object);
	static void getRealStateFromGPU(PipelineState &object);
private:
	void setState(GLenum state, bool active);
	
	static GLenum translateDepthFunction(PipelineState::EDepthFunction depthFunction);
	static PipelineState::EDepthFunction translateDepthFunction(GLenum depthFunction);

	static GLenum translateBlendFactor(PipelineState::_BlendFunction::EFunc blendFactor);
	static PipelineState::_BlendFunction::EFunc translateBlendFactor(GLenum blendFactor);

	static GLenum translateCullFaceMode(PipelineState::ECullFaceMode cullFaceMode);
	static PipelineState::ECullFaceMode translateCullFaceMode(GLenum cullFaceMode);

	PipelineState	m_currentState;
};

#endif