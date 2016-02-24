#pragma once

#ifndef __BAMBOOENGINE_FBOMANAGER_H_
#define __BAMBOOENGINE_FBOMANAGER_H_

#include "FBO.h"

#include <stack>

class FboManager
{
public:
	void pushFBO(FBOPtr fbo);
	void popFBO();

	FBOPtr getActiveFBO() const;

private:
	std::stack<FBOPtr> m_stack;
};

#endif