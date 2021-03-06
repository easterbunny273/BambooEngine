/*
 * header file for DeferredNodeTranslator::Light_RuleObject class
 * written by: christian moellinger <ch.moellinger@gmail.com>
 * 02/2012, Project "BambooEngine"
 */

#pragma once

#ifndef __BAMBOOENGINE_LIGHTRULEOBJECT_DEFERREDTRANSLATOR_HEADER
#define __BAMBOOENGINE_LIGHTRULEOBJECT_DEFERREDTRANSLATOR_HEADER

#include "GLUtils.h"
#include "Graphic.h"
#include "DeferredNodeTranslator/DeferredNodeTranslator.h"
#include "SemanticSceneNodes/Light_SemSceneNode.h"
#include <memory>
#include "BambooLib/include/GeneralDefinitions.h"

namespace BambooGraphics
{
    class DeferredNodeTranslator::Light_RuleObject : public DeferredNodeTranslator::IRuleObject
    {
    public:
      virtual IRuleObject * CloneFor(ISemanticSceneNode *pSemNode, DeferredNodeTranslator *pTranslator);

      /// update the rendering scene graph pieces which correspond to the given semantic scene node
      virtual void Action();

      /// returns the list of accepted semantic scene nodes
      std::vector<BambooLib::t_classID> GetAcceptedNodeIDs() const;

    private:
      std::shared_ptr<RN_SpotLight>      m_spCorrespondingRenderingNode;
      std::shared_ptr<GraphicsCore::IRenderNode>      m_spCorrespondingRenderingNode_Model;

      Light_SemSceneNode * m_pSemNode;
    };

}

#endif
