#include "Nodes/nodes.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <string>
#include <iostream>
#include <cassert>

namespace pt = boost::property_tree;

std::string bamboo::nodes::NodeGraph::writeToJSON()
{
    auto exportInput = [&](InputDescriptor &descriptor) -> std::string {
        std::string inputDescr;

        inputDescr += "{";
        inputDescr += "\"type\": " + std::to_string(descriptor.getTypeHash()) + ",";
        inputDescr += "\"name\": \"" + descriptor.getName() + "\"";
        inputDescr += "}";

        return inputDescr;
    };

    auto exportOutput = [&](OutputDescriptor &descriptor) -> std::string {
        std::string inputDescr;

        inputDescr += "{";
        inputDescr += "\"type\": " + std::to_string(descriptor.getTypeHash()) + ",";
        inputDescr += "\"name\": \"" + descriptor.getName() + "\"";
        inputDescr += "}";

        return inputDescr;
    };

    auto exportNode = [&](nodeID _node) -> std::string {
        std::string nodeRep;

        auto node = getNode(_node);
        nodeRep += "{";
        nodeRep += "\"id\": " + std::to_string(_node) + ",";
        nodeRep += "\"type\": \"" + node->getNodeType() + "\",";
        nodeRep += "\"name\": \"" + node->getName() + "\",";

        std::string inputsAsJson;
        auto inputs = node->getInputs();
        bool first = true;
        for (auto &input : inputs)
        {
            if (!first)
                inputsAsJson += ",";

            inputsAsJson += exportInput(input);
            first = false;
        }

        nodeRep += "\"inputs\": [" + inputsAsJson + "],";

        std::string outputsAsJson;
        auto outputs = node->getOutputs();
        first = true;
        for (auto &output : outputs)
        {
            if (!first)
                outputsAsJson += ",";

            outputsAsJson += exportOutput(output);
            first = false;
        }
        nodeRep += "\"outputs\": [" + outputsAsJson + "]";
        nodeRep += "}";

        return nodeRep;
    };

    std::string result;
    result += "{";

    {
        result += "\"nodes\": [";

        bool first = true;
        for (auto &node : m_nodes)
        {
            if (!first)
                result += ",";
            result += exportNode(node.first);
            first = false;
        }
        result += "],";
    }

    {
        result += "\"connections\": [";

        bool first = true;
        for (auto &connection : m_connections)
        {
            if (!first)
                result += ",";

            result += "{";
            result += "\"sourceNode\": " + std::to_string(connection.m_srcNode) + ",";
            result += "\"sourceOutput\": \"" + connection.m_srcOutput + "\",";
            result += "\"destNode\": " + std::to_string(connection.m_dstNode) + ",";
            result += "\"destInput\": \"" + connection.m_dstInput + "\"";
            result += "}";
            
            
            first = false;
        }
        result += "]";
    }

    result += "}";

    return result;
}


std::shared_ptr<bamboo::nodes::NodeGraph> bamboo::nodes::NodeGraph::readFromJSON(const std::string &json, NodeFactory &factory)
{
    std::shared_ptr<bamboo::nodes::NodeGraph> result = std::make_shared<bamboo::nodes::NodeGraph>();


    // Create empty property tree object
    pt::ptree tree;

    std::istringstream inputStream(json);

    // Parse the XML into the property tree.
    pt::read_json(inputStream, tree);

    for (auto &node : tree.get_child("nodes"))
    {
        auto id = node.second.get<int>("id");
        auto name = node.second.get<std::string>("name");
        auto type = node.second.get<std::string>("type");

        auto newNode = factory.createNodeFromPrototype(type);
        if (!newNode)
        {
            assert(!"could not create node");
            return nullptr;
        }
        else
        {
            newNode->setName(name);
            result->m_nodes[id] = newNode;
        }

        // add node
    }

    for (auto &node : tree.get_child("connections"))
    {
        auto sourceNode = node.second.get<int>("sourceNode");
        auto sourceOutput = node.second.get<std::string>("sourceOutput");

        auto destNode = node.second.get<int>("destNode");
        auto destInput = node.second.get<std::string>("destInput");

        bool validConnection = result->setConnection(sourceNode, sourceOutput, destNode, destInput);

        if (!validConnection)
        {
            assert(!"could not create valid connection");
            return nullptr;
        }
    }

    return result;
}
