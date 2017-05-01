#include "PipelineStateManager.h"

#include <cassert>

void PipelineStateManager::loadPipelineState(const PipelineState &object)
{
	// DEPTH 
	if (object.depthTest != m_currentState.depthTest)
		setState(GL_DEPTH_TEST, object.depthTest);

	if (object.depthFunction != m_currentState.depthFunction)
		glDepthFunc(translateDepthFunction(object.depthFunction));

	// BLEND
	if (object.blend != m_currentState.blend)
		setState(GL_BLEND, object.blend);

	auto numDrawBuffers = object.blendFunctions.size();
	for (size_t i = 0; i < numDrawBuffers; ++i)
		if (object.blendFunctions[i] != m_currentState.blendFunctions[i])
			glBlendFunci(GLuint(i), translateBlendFactor(object.blendFunctions[i].sFactor), translateBlendFactor(object.blendFunctions[i].dFactor));

	// CULL FACE
	if (object.cullFace != m_currentState.cullFace)
		setState(GL_CULL_FACE, object.cullFace);

	if (object.cullFaceMode != m_currentState.cullFaceMode)
		glCullFace(translateCullFaceMode(object.cullFaceMode));
}

void PipelineStateManager::getRealStateFromGPU(PipelineState &object)
{
	// DEPTH
	object.depthTest = (glIsEnabled(GL_DEPTH_TEST) == GL_TRUE) ? true : false;
	GLint glDepthFunction = -1;
	glGetIntegerv(GL_DEPTH_FUNC, &glDepthFunction);

	object.depthFunction = translateDepthFunction(glDepthFunction);

	// BLEND
	object.blend = (glIsEnabled(GL_BLEND) == GL_TRUE) ? true : false;
	auto numDrawBuffers = object.blendFunctions.size();
	for (size_t i = 0; i < numDrawBuffers; ++i)
	{
		GLint srcFactor=-1, dstFactor=-1;
		glGetIntegeri_v(GL_BLEND_SRC_RGB, (GLuint)i, &srcFactor);
		glGetIntegeri_v(GL_BLEND_DST_RGB, (GLuint)i, &dstFactor);
		object.blendFunctions[i].sFactor = translateBlendFactor(srcFactor);
		object.blendFunctions[i].dFactor = translateBlendFactor(dstFactor);
	}

	// CULL FACE
	object.cullFace = (glIsEnabled(GL_CULL_FACE) == GL_TRUE) ? true : false;
	GLint glCullFaceMode = -1;
	glGetIntegerv(GL_CULL_FACE_MODE, &glCullFaceMode);
	object.cullFaceMode = translateCullFaceMode(glCullFaceMode);

}

void PipelineStateManager::setState(GLenum state, bool active)
{
	if (active)
		glEnable(state);
	else
		glDisable(state);
}

GLenum PipelineStateManager::translateDepthFunction(PipelineState::EDepthFunction depthFunction)
{
	switch (depthFunction)
	{
	case PipelineState::EDepthFunction::ALWAYS:
		return GL_ALWAYS;
	case PipelineState::EDepthFunction::EQUAL:
		return GL_EQUAL;
	case PipelineState::EDepthFunction::GEQUAL:
		return GL_GEQUAL;
	case PipelineState::EDepthFunction::GREATER:
		return GL_GREATER;
	case PipelineState::EDepthFunction::LEQUAL:
		return GL_LEQUAL;
	case PipelineState::EDepthFunction::LESS:
		return GL_LESS;
	case PipelineState::EDepthFunction::NEVER:
		return GL_NEVER;
	case PipelineState::EDepthFunction::NOTEQUAL:
		return GL_NOTEQUAL;
	default:
		assert(!"mode not known");
		return GL_NEVER;
	}
}

PipelineState::EDepthFunction PipelineStateManager::translateDepthFunction(GLenum depthFunction)
{
	switch (depthFunction)
	{
	case GL_ALWAYS:
		return PipelineState::EDepthFunction::ALWAYS;
	case GL_EQUAL:
		return PipelineState::EDepthFunction::EQUAL;
	case GL_GEQUAL:
		return PipelineState::EDepthFunction::GEQUAL;
	case GL_GREATER:
		return PipelineState::EDepthFunction::GREATER;
	case GL_LEQUAL:
		return PipelineState::EDepthFunction::LEQUAL;
	case GL_LESS:
		return PipelineState::EDepthFunction::LESS;
	case GL_NEVER:
		return PipelineState::EDepthFunction::NEVER;
	case GL_NOTEQUAL:
		return PipelineState::EDepthFunction::NOTEQUAL;
	default:
		assert(!"mode not known");
		return PipelineState::EDepthFunction::NEVER;;
	}
}

GLenum PipelineStateManager::translateBlendFactor(PipelineState::_BlendFunction::EFunc blendFactor)
{
	switch (blendFactor)
	{
	case PipelineState::_BlendFunction::EFunc::CONSTANT_ALPHA:
		return GL_CONSTANT_ALPHA;
	case PipelineState::_BlendFunction::EFunc::CONSTANT_COLOR:
		return GL_CONSTANT_COLOR;
	case PipelineState::_BlendFunction::EFunc::DST_ALPHA:
		return GL_DST_ALPHA;
	case PipelineState::_BlendFunction::EFunc::DST_COLOR:
		return GL_DST_COLOR;
	case PipelineState::_BlendFunction::EFunc::ONE:
		return GL_ONE;
	case PipelineState::_BlendFunction::EFunc::ONE_MINUS_CONSTANT_ALPHA:
		return GL_ONE_MINUS_CONSTANT_ALPHA;
	case PipelineState::_BlendFunction::EFunc::ONE_MINUS_CONSTANT_COLOR:
		return GL_ONE_MINUS_CONSTANT_COLOR;
	case PipelineState::_BlendFunction::EFunc::ONE_MINUS_DST_ALPHA:
		return GL_ONE_MINUS_DST_ALPHA;
	case PipelineState::_BlendFunction::EFunc::ONE_MINUS_DST_COLOR:
		return GL_ONE_MINUS_DST_COLOR;
	case PipelineState::_BlendFunction::EFunc::ONE_MINUS_SRC1_ALPHA:
		return GL_ONE_MINUS_SRC1_ALPHA;
	case PipelineState::_BlendFunction::EFunc::ONE_MINUS_SRC1_COLOR:
		return GL_ONE_MINUS_SRC1_COLOR;
	case PipelineState::_BlendFunction::EFunc::ONE_MINUS_SRC_ALPHA:
		return GL_ONE_MINUS_SRC_ALPHA;
	case PipelineState::_BlendFunction::EFunc::ONE_MINUS_SRC_COLOR:
		return GL_ONE_MINUS_SRC_COLOR;
	case PipelineState::_BlendFunction::EFunc::SRC1_ALPHA:
		return GL_SRC1_ALPHA;
	case PipelineState::_BlendFunction::EFunc::SRC1_COLOR:
		return GL_SRC1_COLOR;
	case PipelineState::_BlendFunction::EFunc::SRC_ALPHA:
		return GL_SRC_ALPHA;
	case PipelineState::_BlendFunction::EFunc::SRC_ALPHA_SATURATE:
		return GL_SRC_ALPHA_SATURATE;
	case PipelineState::_BlendFunction::EFunc::SRC_COLOR:
		return GL_SRC_COLOR;
	case PipelineState::_BlendFunction::EFunc::ZERO:
		return GL_ZERO;
	default:
		assert(!"no case fired");
		return GL_ZERO;
	}
}

PipelineState::_BlendFunction::EFunc PipelineStateManager::translateBlendFactor(GLenum blendFactor)
{
	switch (blendFactor)
	{
	case GL_CONSTANT_ALPHA:
		return PipelineState::_BlendFunction::EFunc::CONSTANT_ALPHA;
	case GL_CONSTANT_COLOR:
		return PipelineState::_BlendFunction::EFunc::CONSTANT_COLOR;
	case GL_DST_ALPHA:
		return PipelineState::_BlendFunction::EFunc::DST_ALPHA;
	case GL_DST_COLOR:
		return PipelineState::_BlendFunction::EFunc::DST_COLOR;
	case GL_ONE:
		return PipelineState::_BlendFunction::EFunc::ONE;
	case GL_ONE_MINUS_CONSTANT_ALPHA:
		return PipelineState::_BlendFunction::EFunc::ONE_MINUS_CONSTANT_ALPHA;
	case GL_ONE_MINUS_CONSTANT_COLOR:
		return PipelineState::_BlendFunction::EFunc::ONE_MINUS_CONSTANT_COLOR;
	case GL_ONE_MINUS_DST_ALPHA:
		return PipelineState::_BlendFunction::EFunc::ONE_MINUS_DST_ALPHA;
	case GL_ONE_MINUS_DST_COLOR:
		return PipelineState::_BlendFunction::EFunc::ONE_MINUS_DST_COLOR;
	case GL_ONE_MINUS_SRC1_ALPHA:
		return PipelineState::_BlendFunction::EFunc::ONE_MINUS_SRC1_ALPHA;
	case GL_ONE_MINUS_SRC1_COLOR:
		return PipelineState::_BlendFunction::EFunc::ONE_MINUS_SRC1_COLOR;
	case GL_ONE_MINUS_SRC_ALPHA:
		return PipelineState::_BlendFunction::EFunc::ONE_MINUS_SRC_ALPHA;
	case GL_ONE_MINUS_SRC_COLOR:
		return PipelineState::_BlendFunction::EFunc::ONE_MINUS_SRC_COLOR;
	case GL_SRC1_ALPHA:
		return PipelineState::_BlendFunction::EFunc::SRC1_ALPHA;
	case GL_SRC1_COLOR:
		return PipelineState::_BlendFunction::EFunc::SRC1_COLOR;
	case GL_SRC_ALPHA:
		return PipelineState::_BlendFunction::EFunc::SRC_ALPHA;
	case GL_SRC_ALPHA_SATURATE:
		return PipelineState::_BlendFunction::EFunc::SRC_ALPHA_SATURATE;
	case GL_SRC_COLOR:
		return PipelineState::_BlendFunction::EFunc::SRC_COLOR;
	case GL_ZERO:
		return PipelineState::_BlendFunction::EFunc::ZERO;
	default:
		assert(!"no case fired");
		return PipelineState::_BlendFunction::EFunc::ZERO;
	}
}

GLenum PipelineStateManager::translateCullFaceMode(PipelineState::ECullFaceMode cullFaceMode)
{
	switch (cullFaceMode)
	{
	case PipelineState::ECullFaceMode::FRONT:
		return GL_FRONT;
	case PipelineState::ECullFaceMode::BACK:
		return GL_BACK;
	case PipelineState::ECullFaceMode::FRONT_AND_BACK:
		return GL_FRONT_AND_BACK;
	default:
		assert(!"no case fired");
		return GL_FRONT_AND_BACK;
	}
}

PipelineState::ECullFaceMode PipelineStateManager::translateCullFaceMode(GLenum cullFaceMode)
{
	switch (cullFaceMode)
	{
	case GL_FRONT:
		return PipelineState::ECullFaceMode::FRONT;
	case GL_BACK:
		return PipelineState::ECullFaceMode::BACK;
	case GL_FRONT_AND_BACK:
		return PipelineState::ECullFaceMode::FRONT_AND_BACK;
	default:
		assert(!"no case fired");
		return PipelineState::ECullFaceMode::FRONT_AND_BACK;
	}
}

