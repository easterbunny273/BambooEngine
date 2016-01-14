#pragma once


#ifndef __BAMBOOENGINE_GEOMETRY_H_
#define __BAMBOOENGINE_GEOMETRY_H_

#include <vector>

#include "ITransformable.h"

class Mesh;

class Geometry : public ITransformable
{
public:
	std::vector<const Mesh *> getMeshes() const;
}

#endif

