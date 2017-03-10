#include <iostream>
#include <algorithm>
#include <cassert>

#include "nodes.h"

class TestSumNode : public bamboo::nodes::Node
{
public:
    TestSumNode(std::string name) : bamboo::nodes::Node(name, "TestNode", { bamboo::nodes::TypedInputDescriptor<int>("input1"),
        bamboo::nodes::TypedInputDescriptor<int>("input2") }, { bamboo::nodes::TypedOutputDescriptor<int>("sum") }) {};

    std::shared_ptr<bamboo::nodes::Node> clone() const override { return std::make_shared<TestSumNode>(m_name); }
};


class TestPrintNode : public bamboo::nodes::Node
{
public:
    TestPrintNode(std::string name) : bamboo::nodes::Node(name, "PrintNode", { bamboo::nodes::TypedInputDescriptor<int>("input") }, {}) {};

    std::shared_ptr<bamboo::nodes::Node> clone() const override { return std::make_shared<TestPrintNode>(m_name); }
};

int main3(int argc, char ** argv)
{
    bamboo::nodes::NodeFactory nodeFactory;
    nodeFactory.registerPrototype(std::make_shared<TestSumNode>("prototype2"));
    nodeFactory.registerPrototype(std::make_shared<TestPrintNode>("prototype1"));

    bamboo::nodes::TypedInputDescriptor<int> test("hello");

    test;



    bamboo::nodes::NodeGraph graph;
    auto result1 = graph.createNode<TestSumNode>("node1");
    auto result2 = graph.createNode<TestPrintNode>("node2");

    auto testNode1 = graph.getNode(result1);

    bool ok = graph.setConnection(result1, "sum", result2, "input");

    auto connections1 = graph.getConnections(result1);
    auto connections2 = graph.getConnections(result2);

    auto testJson = graph.writeToJSON();
    auto result = graph.readFromJSON(testJson, nodeFactory);


    bool removed = graph.removeNode(result1);

    auto _connections1 = graph.getConnections(result1);
    auto _connections2 = graph.getConnections(result2);

    char buffer;
    std::cin >> buffer;

    return 0;
}