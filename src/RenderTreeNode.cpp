#include "RenderTreeNode.h"

#include <cassert>

bool RenderTreeNode::render(RenderContext &context)
{
	bool shouldRender = _shouldRender(context);

	if (shouldRender)
	{
		_preRenderSubNodes(context);

		for (auto &subnode : m_subNodes)
			subnode->render(context);

		_postRenderSubNodes(context);
	}
	
	return shouldRender;
}

void RenderTreeNode::addSubNode(RenderTreeNodePtr node)
{
	assert(hasSubNode(node) == false);
}

bool RenderTreeNode::hasSubNode(RenderTreeNodePtr node) const
{
	return (std::find(m_subNodes.cbegin(), m_subNodes.cend(), node) != m_subNodes.cend());
}

void RenderTreeNode::removeSubNode(RenderTreeNodePtr node)
{
	auto iter = std::find(m_subNodes.cbegin(), m_subNodes.cend(), node);
	assert(iter != m_subNodes.cend());

	if (iter != m_subNodes.cend())
		m_subNodes.erase(iter);
}
