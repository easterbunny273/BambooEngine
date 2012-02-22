/*
 * header file for Bamboo::DeferredNodeTranslator class
 * written by: christian moellinger <ch.moellinger@gmail.com>
 * 02/2012, Project "BambooEngine"
 */

#pragma once

#ifndef __BAMBOOENGINE_CAMERARULEOBJECT_DEFERREDTRANSLATOR_HEADER
#define __BAMBOOENGINE_CAMERARULEOBJECT_DEFERREDTRANSLATOR_HEADER

#include "common_gl.h"
#include "Graphic.h"
#include "DeferredNodeTranslator/DeferredNodeTranslator.h"

#include <memory>

class DeferredNodeTranslator::Camera_RuleObject : public DeferredNodeTranslator::IRuleObject
{
public:
  /// update the rendering scene graph pieces which correspond to the given semantic scene node
  virtual void Action(std::shared_ptr<ISemanticSceneNode> spSemNode);

  /// returns the list of accepted semantic scene nodes
  std::vector<ISemanticSceneNode::t_classID> GetAcceptedNodeIDs() const;

private:

};

#endif
