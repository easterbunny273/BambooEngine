#include "CommandStreamHandler.h"

// ++++++++++ TEST STUFF START
#include "Nodes/nodes.h"

class TestSumNode : public bamboo::nodes::Node
{
public:
    TestSumNode(std::string name) : bamboo::nodes::Node(name, "TestNode", { bamboo::nodes::TypedInputDescriptor<int>::create("input1"),
        bamboo::nodes::TypedInputDescriptor<int>::create("input2") }, { bamboo::nodes::TypedOutputDescriptor<int>::create("sum") }) {};

    std::shared_ptr<bamboo::nodes::Node> clone() const override { return std::make_shared<TestSumNode>(m_name); }
};


class TestPrintNode : public bamboo::nodes::Node
{
public:
    TestPrintNode(std::string name) : bamboo::nodes::Node(name, "PrintNode", { bamboo::nodes::TypedInputDescriptor<int>::create("input") }, {}) {};

    std::shared_ptr<bamboo::nodes::Node> clone() const override { return std::make_shared<TestPrintNode>(m_name); }
};
// ++++++++++ TEST STUFF  END

std::unique_ptr<bamboo::INetworkConnectionHandler> bamboo::CommandStreamHandlerFactory::create()
{
	return std::make_unique<CommandStreamHandler>();
}

void bamboo::CommandStreamHandler::handle(boost::asio::ip::tcp::socket &socket)
{
    assert(!"not implemented yet");
}

std::unique_ptr<bamboo::INetworkConnectionHandler> bamboo::NodeTestStreamHandlerFactory::create()
{
    return std::make_unique<NodeTestStreamHandler>();
}

void bamboo::NodeTestStreamHandler::handle(boost::asio::ip::tcp::socket &socket)
{
    socket.write_some(boost::asio::buffer("WELCOME\n"));

    while (socket.is_open())
    {
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\n");
        
        std::string responseAsString(boost::asio::buffer_cast<const char*>(response.data()), response.size());

        if (responseAsString == "GET\n")
        {
            bamboo::nodes::NodeFactory nodeFactory;
            nodeFactory.registerPrototype(std::make_shared<TestSumNode>("prototype2"));
            nodeFactory.registerPrototype(std::make_shared<TestPrintNode>("prototype1"));

            bamboo::nodes::NodeGraph graph;
            auto node1 = graph.createNode<TestSumNode>("node1");
            auto node2 = graph.createNode<TestPrintNode>("node2");

            bool ok = graph.setConnection(node1, "sum", node2, "input");

            auto testJson = graph.writeToJSON();
            testJson.append("\n");

            boost::asio::write(socket, boost::asio::buffer(testJson));
        }
    }
    
}
