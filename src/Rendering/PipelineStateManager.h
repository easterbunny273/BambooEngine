#pragma once

#ifndef __BAMBOOENGINE_PIPELINESTATEMANAGER_H_
#define __BAMBOOENGINE_PIPELINESTATEMANAGER_H_

#include <memory>

#include "GL/glew.h"

class PipelineStateManager
{
public:
	class PipelineStateObject
	{
	public:
		struct DepthTestState
		{
			bool active;
			GLenum depthFunc;

			bool operator==(const DepthTestState &rOther) const { return (active == rOther.active && depthFunc == rOther.depthFunc); }
			bool operator!=(const DepthTestState &rOther) const { return !operator==(rOther); }
			DepthTestState(bool _active, GLenum _depthFunc) : active(_active), depthFunc(_depthFunc) {};
		};

		DepthTestState depthTestState;

		using PSOPtr = std::shared_ptr<PipelineStateObject>;

		static PSOPtr createDefaultStateObject() {
			auto depthTest = DepthTestState(true, GL_LEQUAL);
			return std::shared_ptr<PipelineStateObject>(new PipelineStateObject(depthTest));
		};

		void load(PipelineStateObject &oldState);
	private:
		bool assertDepthTestIsSet(const DepthTestState &state);
		void updateDepthTestState(const DepthTestState &newState);

		PipelineStateObject(DepthTestState depthTest) : depthTestState(depthTest) {};
	};

	void loadPipelineState(PipelineStateObject &object);

private:

	

	PipelineStateObject	m_currentState;
};

#endif