#pragma once

#ifndef __BAMBOOENGINE_ITRANSFORMABLE_H_
#define __BAMBOOENGINE_ITRANSFORMABLE_H_

#include "glm/matrix.hpp"

class ITransformable
{
public:
	const glm::mat4& getTransformMatrix() const { return m_transformMatrix; };
	void setTransformMatrix(const glm::mat4& newMatrix) { m_transformMatrix = newMatrix; };

private:
	glm::mat4 m_transformMatrix;
};

#endif