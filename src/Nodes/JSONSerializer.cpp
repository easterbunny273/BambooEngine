#include "Nodes/nodes.h"

#include "utils/ModernJSON.h"

#include <string>
#include <iostream>
#include <cassert>

std::string bamboo::nodes::NodeGraph::writeToJSON()
{
    nlohmann::json rootObject;
    rootObject["nodes"] = nlohmann::json::array();

    for (auto &node : m_nodes)
    {
        nlohmann::json nodeRep;

        auto nodeId = node.first;
        auto node = getNode(nodeId);

        nodeRep["id"] = nodeId;
        nodeRep["type"] = node->getNodeType();
        nodeRep["name"] = node->getName();

        nodeRep["inputs"] = nlohmann::json::array();
        for (auto &input : node->getInputs())
        {
            nlohmann::json ioRep;
            ioRep["name"] = input->getName();
            ioRep["typeHash"] = input->getTypeHash();

            nodeRep["inputs"].push_back(ioRep);
        }

        nodeRep["outputs"] = nlohmann::json::array();
        for (auto &output : node->getOutputs())
        {
            nlohmann::json ioRep;
            ioRep["name"] = output->getName();
            ioRep["typeHash"] = output->getTypeHash();

            nodeRep["outputs"].push_back(ioRep);
        }

        rootObject["nodes"].push_back(nodeRep);
    }

    rootObject["connections"] = nlohmann::json::array();

    for (auto &connection : m_connections)
    {
        nlohmann::json connectionRep;
        connectionRep["sourceNode"] = connection.m_srcNode;
        connectionRep["sourceOutput"] = connection.m_srcOutput;
        connectionRep["destNode"] = connection.m_dstNode;
        connectionRep["destInput"] = connection.m_dstInput;

        rootObject["connections"].push_back(connectionRep);
    }

    std::stringstream outputStream;
    outputStream << std::setw(2) << rootObject << std::endl;

    return outputStream.str();
}

std::shared_ptr<bamboo::nodes::NodeGraph> bamboo::nodes::NodeGraph::readFromJSON(const std::string &json, NodeFactory &factory)
{
    auto result = std::make_shared<bamboo::nodes::NodeGraph>();
    nlohmann::json rootObject;

    try
    {
        std::stringstream inputStream(json);
        inputStream >> rootObject;
    }
    catch (nlohmann::json::parse_error &e)
    {
        std::cerr << "Could not parse node graph: " << e.what() << std::endl;
        return nullptr;
    }

    try
    {
        auto nodesArray = rootObject["nodes"];
        for (auto &nodeRep : nodesArray)
        {
            auto id = nodeRep["id"];
            auto name = nodeRep["name"];
            auto type = nodeRep["type"];

            int test = type;

            auto newNode = factory.createNodeFromPrototype(type);
            if (!newNode)
            {
                std::cerr << "Could not restore node with type \"" << type << "\"" << std::endl;
                return nullptr;
            }
            else
            {
                newNode->setName(name);
                result->m_nodes[id] = newNode;
            }
        }

        for (auto &connectionRep : rootObject["connections"])
        {
            auto sourceNode = connectionRep["sourceNode"];
            auto sourceOutput = connectionRep["sourceOutput"];

            auto destNode = connectionRep["destNode"];
            auto destInput = connectionRep["destInput"];

            bool validConnection = result->setConnection(sourceNode, sourceOutput, destNode, destInput);

            if (!validConnection)
            {
                assert(!"could not create valid connection");
                return nullptr;
            }
        }
    }
    catch (nlohmann::json::type_error &e)
    {
        std::cerr << "Could not restore node graph: " << e.what() << std::endl;
        return nullptr;
    }

    return result;
}