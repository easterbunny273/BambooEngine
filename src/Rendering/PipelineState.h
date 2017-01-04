#ifndef __BAMBOOENGINE_PIPELINESTATE_H_
#define __BAMBOOENGINE_PIPELINESTATE_H_

#include <array>

struct PipelineState
{
	enum class EDrawBuffer
	{
		NONE,
		FRONT_LEFT,
		FRONT_RIGHT,
		BACK_LEFT,
		BACK_RIGHT,
		FRONT,
		BACK,
		LEFT,
		RIGHT,
		FRONT_AND_BACK,

		COLOR_ATTACHMENT0,
		COLOR_ATTACHMENT1,
		COLOR_ATTACHMENT2,
		COLOR_ATTACHMENT3,
		COLOR_ATTACHMENT4,
		COLOR_ATTACHMENT5,
		COLOR_ATTACHMENT6,
		COLOR_ATTACHMENT7,
		COLOR_ATTACHMENT8,

		NUM_DRAW_BUFFERS
	};

	static const size_t MAX_DRAW_BUFFERS = (size_t)EDrawBuffer::NUM_DRAW_BUFFERS;

	enum class EDepthFunction
	{
		NEVER,
		LESS,
		EQUAL,
		LEQUAL,
		GREATER,
		NOTEQUAL,
		GEQUAL,
		ALWAYS
	};

	struct _BlendFunction {
		enum class EFunc {
			ZERO,
			ONE,
			SRC_COLOR,
			ONE_MINUS_SRC_COLOR,
			DST_COLOR,
			ONE_MINUS_DST_COLOR,
			SRC_ALPHA,
			ONE_MINUS_SRC_ALPHA,
			DST_ALPHA,
			ONE_MINUS_DST_ALPHA,
			CONSTANT_COLOR,
			ONE_MINUS_CONSTANT_COLOR,
			CONSTANT_ALPHA,
			ONE_MINUS_CONSTANT_ALPHA,
			SRC_ALPHA_SATURATE,
			SRC1_COLOR,
			ONE_MINUS_SRC1_COLOR,
			SRC1_ALPHA,
			ONE_MINUS_SRC1_ALPHA
		};

		EFunc sFactor{ EFunc::ONE };
		EFunc dFactor{ EFunc::ZERO };

		bool operator==(const _BlendFunction &other) const { return sFactor == other.sFactor && dFactor == other.dFactor; };
		bool operator!=(const _BlendFunction &other) const { return sFactor != other.sFactor || dFactor != other.dFactor; };
	};

	bool blend { false };
	std::array<_BlendFunction, MAX_DRAW_BUFFERS> blendFunctions;
	
	enum class ECullFaceMode {
		FRONT,
		BACK,
		FRONT_AND_BACK
	};

	bool cullFace { false };
	ECullFaceMode cullFaceMode { ECullFaceMode::BACK };


	bool depthTest { false };
	EDepthFunction depthFunction { EDepthFunction::LESS };

	struct _ScissorWindow
	{
		GLint x{ 0 };
		GLint y{ 0 };
		GLsizei width{ 0 };		//TODO: initialize with window width
		GLsizei height{ 0 };	//TODO: initialize with window height
	};

	bool scissorTest{ false };
	_ScissorWindow scissorWindow;
};

#endif