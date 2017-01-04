#pragma once

#ifndef __BAMBOOENGINE_RENDERTREENODE_DUMMYCUBE_H_
#define __BAMBOOENGINE_RENDERTREENODE_DUMMYCUBE_H_

#include "RenderTreeNode.h"

class RenderTreeNode_DummyCube : public RenderTreeNode
{
public:

protected:
	virtual bool _shouldRender(RenderContext &context) { return true; };
	virtual void _preRenderSubNodes(RenderContext &context);
	virtual void _postRenderSubNodes(RenderContext &context);
};

#endif