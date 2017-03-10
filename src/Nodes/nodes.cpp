#include <algorithm>
#include <cassert>

#include "Nodes/nodes.h"

bool bamboo::nodes::NodeGraph::removeNode(nodeID node)
{
    auto nodeObject = getNode(node);
    auto oldSize = m_nodes.size();

    removeConnections(node);

    m_nodes.erase(node);

    assert(getConnections(node).empty());
    assert(!getNode(node));

    return (m_nodes.size() != oldSize);
}

bool bamboo::nodes::NodeGraph::setConnection(nodeID srcNode, const std::string &srcOutput, nodeID dstNode, const std::string &dstInput)
{
    auto srcNodeObject = getNode(srcNode);
    auto dstNodeObject = getNode(dstNode);

    if (!srcNodeObject || !dstNodeObject)
        return false;

    auto inputDescriptor = dstNodeObject->getInput(dstInput);
    auto outputDescriptor = srcNodeObject->getOutput(srcOutput);

    if (!inputDescriptor || !outputDescriptor)
        return false;

    bool correctType = (inputDescriptor->getTypeHash() == outputDescriptor->getTypeHash());

    if (correctType)
    {
        auto oldConnection = getConnection(dstNode, dstInput);

        if (oldConnection)
            removeConnection(dstNode, dstInput);

        m_connections.emplace_back(srcNode, srcOutput, dstNode, dstInput);
    }

    return correctType;
}

boost::optional<bamboo::nodes::Connection> bamboo::nodes::NodeGraph::getConnection(nodeID dstNode, const std::string &dstInput)
{
    for (auto connection : m_connections)
    {
        if (connection.m_dstNode == dstNode && dstInput == connection.m_dstInput)
            return boost::optional<bamboo::nodes::Connection>(connection);
    }

    return {};
}

std::vector<bamboo::nodes::Connection> bamboo::nodes::NodeGraph::getInputConnections(nodeID dstNode)
{
    std::vector<bamboo::nodes::Connection> result;

    for (auto connection : m_connections)
    {
        if (connection.m_dstNode == dstNode)
            result.push_back(connection);
    }

    return result;
}

std::vector<bamboo::nodes::Connection> bamboo::nodes::NodeGraph::getOutputConnections(nodeID srcNode)
{
    std::vector<bamboo::nodes::Connection> result;

    for (auto connection : m_connections)
    {
        if (connection.m_srcNode == srcNode)
            result.push_back(connection);
    }

    return result;
}

std::vector<bamboo::nodes::Connection> bamboo::nodes::NodeGraph::getConnections(nodeID node)
{
    std::vector<bamboo::nodes::Connection> result;

    for (auto connection : m_connections)
    {
        if (connection.m_srcNode == node || 
            connection.m_dstNode == node)
            result.push_back(connection);
    }

    return result;
}

bool bamboo::nodes::NodeGraph::removeConnection(nodeID dstNode, const std::string &dstInput)
{
    for (auto iter = std::begin(m_connections); iter != std::end(m_connections); ++iter)
    {
        if (iter->m_dstNode != dstNode ||
            iter->m_dstInput != dstInput)
            continue;

        m_connections.erase(iter);
        return true;
    }

    return false;
}

bool bamboo::nodes::NodeGraph::removeConnections(nodeID node)
{
    std::vector<Connection> remainingConnections;
    remainingConnections.reserve(m_connections.size());

    auto oldSize = m_connections.size();

    for (auto &connection : m_connections)
    {
        if (connection.m_dstNode == node ||
            connection.m_srcNode == node)
            continue;

        remainingConnections.push_back(connection);
    }

    m_connections = remainingConnections;

    return (m_connections.size() != oldSize);
}

std::shared_ptr<bamboo::nodes::Node> bamboo::nodes::NodeGraph::getNode(nodeID _nodeID) const
{
    auto iterator = m_nodes.find(_nodeID);

    if (iterator == std::end(m_nodes))
        return nullptr;

    return iterator->second;
}

const bamboo::nodes::InputDescriptor* bamboo::nodes::Node::getInput(const std::string &name) const
{
    for (auto &input : m_inputs)
    {
        if (input.getName() == name)
            return &input;
    }

    return nullptr;
}

const bamboo::nodes::OutputDescriptor* bamboo::nodes::Node::getOutput(const std::string &name) const
{
    for (auto &output : m_outputs)
    {
        if (output.getName() == name)
            return &output;
    }

    return nullptr;
}

bool bamboo::nodes::NodeFactory::registerPrototype(std::shared_ptr<Node> nodePrototype)
{
    for (auto &v : m_registeredPrototypes)
    {
        if (v->getNodeType() == nodePrototype->getNodeType())
        {
            assert(!"node type already registered");
            return false;
        }
    }

    m_registeredPrototypes.push_back(nodePrototype);
    return true;
}

const std::vector<std::shared_ptr<bamboo::nodes::Node>> bamboo::nodes::NodeFactory::getPrototypes()
{
    return m_registeredPrototypes;
}

std::shared_ptr<bamboo::nodes::Node> bamboo::nodes::NodeFactory::createNodeFromPrototype(std::string type)
{
    for (auto &v : m_registeredPrototypes)
    {
        if (v->getNodeType() != type)
            continue;

        return v->clone();
    }

    return nullptr;
}
