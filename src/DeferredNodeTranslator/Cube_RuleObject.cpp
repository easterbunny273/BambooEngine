#include "DeferredNodeTranslator/Cube_RuleObject.h"
#include "SemanticSceneNodes/Cube_SemSceneNode.h"

std::vector<ISemanticSceneNode::t_classID> DeferredNodeTranslator::Cube_RuleObject::GetAcceptedNodeIDs() const
{
  std::vector<ISemanticSceneNode::t_classID> vAcceptedIDs;

  // this rule object only accepts Cube_SemSceneNode
  vAcceptedIDs.push_back(Cube_SemSceneNode::ClassID());

  return vAcceptedIDs;
}

void DeferredNodeTranslator::Cube_RuleObject::Action(std::shared_ptr<ISemanticSceneNode> spSemNode)
{

}
