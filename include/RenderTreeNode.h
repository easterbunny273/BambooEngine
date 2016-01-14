#ifndef __BAMBOOENGINE_RENDERTREENODE_H
#define __BAMBOOENGINE_RENDERTREENODE_H

// Includes
#include <vector>
#include <memory>

// Forward declarations
class RenderContext;
class RenderTreeNode;

// Global type declarations
using RenderTreeNodePtr = std::shared_ptr<RenderTreeNode>;

// Class definition
class RenderTreeNode
{
public:
	virtual ~RenderTreeNode() {};

	bool render(RenderContext &context);

	auto getSubNodes() const { return m_subNodes; };
	void addSubNode(RenderTreeNodePtr node);
	bool hasSubNode(RenderTreeNodePtr node) const;
	void removeSubNode(RenderTreeNodePtr node);

protected:
	virtual bool _shouldRender(RenderContext &context) = 0;
	virtual void _preRenderSubNodes(RenderContext &context) = 0;
	virtual void _postRenderSubNodes(RenderContext &context) = 0;

private:
	std::vector<RenderTreeNodePtr>	m_subNodes;
};


#endif