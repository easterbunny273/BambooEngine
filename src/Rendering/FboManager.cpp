#include "FboManager.h"

void FboManager::pushFBO(FBOPtr fbo)
{
	m_stack.push(fbo);
	
	fbo->bindFBO();
}

void FboManager::popFBO()
{
	m_stack.pop();

	auto pActiveFBO = getActiveFBO();
	if (pActiveFBO)
		pActiveFBO->bindFBO();
	else
		FBO::unbindFBO();

}

FBOPtr FboManager::getActiveFBO() const
{
	return (m_stack.empty() == false) ? m_stack.top() : nullptr;
}

