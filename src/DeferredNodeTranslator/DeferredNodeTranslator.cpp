#include "DeferredNodeTranslator/DeferredNodeTranslator.h"

#include "DeferredNodeTranslator/Camera_RuleObject.h"
#include "DeferredNodeTranslator/Cube_RuleObject.h"
#include "DeferredNodeTranslator/Light_RuleObject.h"
#include "DeferredNodeTranslator/LoadedModel_RuleObject.h"

#include "PC_Logger.h"

DeferredNodeTranslator::DeferredNodeTranslator()
{
  std::shared_ptr<IRuleObject> spCameraRule(new Camera_RuleObject());
  std::shared_ptr<IRuleObject> spCubeRule(new Cube_RuleObject());
  std::shared_ptr<IRuleObject> spLightRule(new Light_RuleObject());
  std::shared_ptr<IRuleObject> spLoadedModelRule(new LoadedModel_RuleObject());

  ItlRegisterRuleObjectPrototype(spCameraRule);
  ItlRegisterRuleObjectPrototype(spCubeRule);
  ItlRegisterRuleObjectPrototype(spLightRule);
  ItlRegisterRuleObjectPrototype(spLoadedModelRule);
}

DeferredNodeTranslator::~DeferredNodeTranslator()
{

}

std::shared_ptr<Bamboo::IRenderNode> DeferredNodeTranslator::Translate(std::shared_ptr<ISemanticSceneNode> spSemRoot)
{

}

void DeferredNodeTranslator::ItlRegisterRuleObjectPrototype(std::shared_ptr<IRuleObject> pObject)
{
  std::vector<ISemanticSceneNode::t_classID> vAcceptedIDs = pObject->GetAcceptedNodeIDs();

  for (unsigned int i=0; i < vAcceptedIDs.size(); i++)
  {
      bool bAlreadyRegistered = (m_mRegisteredRuleObjects.find(vAcceptedIDs[i]) != m_mRegisteredRuleObjects.end());

      assert (bAlreadyRegistered == false);

      if (bAlreadyRegistered)
        Logger::error() << "A rule object prototype for handling semantic scene graph nodes with class id " << vAcceptedIDs[i] << " is already registered!" << Logger::endl;
      else
        m_mRegisteredRuleObjects[vAcceptedIDs[i]] = pObject;
  }
}
