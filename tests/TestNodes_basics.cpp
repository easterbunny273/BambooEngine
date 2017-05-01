#include <iostream>

#include "gtest/gtest.h"

#include "Nodes/nodes.h"



TEST(Nodes_SharedVariableMap, createIntVariable)
{
    bamboo::nodes::SharedVariableMap test;

    test.create<int>("int-value");

    EXPECT_TRUE(test.get("int-value") != nullptr);
}

TEST(Nodes_SharedVariableMap, createAndReadIntVariable)
{
    bamboo::nodes::SharedVariableMap test;

    test.create<int>("int-value");

    auto typedVariable = test.get<int>("int-value");
    
    EXPECT_TRUE(typedVariable != nullptr);

    typedVariable->set(42);


    auto typedVariable2 = test.get<int>("int-value");

    EXPECT_EQ(typedVariable2->get(), 42);
}

TEST(Nodes_SharedVariableMap, createAndRemoveVariable)
{
    bamboo::nodes::SharedVariableMap test;

    test.create<int>("int-value");

    auto typedVariable1 = test.get<int>("int-value");
    EXPECT_TRUE(typedVariable1 != nullptr);

    test.remove("int-value");

    auto typedVariable2 = test.get<int>("int-value");
    EXPECT_FALSE(typedVariable2 != nullptr);
}


TEST(Nodes_SharedVariableMap, sharedVariables)
{
    bamboo::nodes::SharedVariableMap storage1;
    bamboo::nodes::SharedVariableMap storage2;

    storage1.create<std::string>("variable2");

    storage2.replace("refedVariable2", storage1.get("variable2"));

    auto originalVariable = storage1.get<std::string>("variable2");
    originalVariable->set("hallo");

    auto refedVariable = storage2.get<std::string>("refedVariable2");
    EXPECT_EQ(refedVariable->get(), "hallo");
}

TEST(Nodes_Inputs, sharedVariables_shared)
{
    auto storage1 = std::make_shared<bamboo::nodes::SharedVariableMap>();
    storage1->create<std::string>("variable1")->set("hallo");

    const bamboo::nodes::Inputs inputs(storage1);
    auto test_variable = inputs.get<std::string>("variable1");
    EXPECT_EQ(test_variable, "hallo");
}

TEST(Nodes_Inputs, sharedVariables_unique)
{
    auto storage1 = std::make_unique<bamboo::nodes::SharedVariableMap>();
    storage1->create<std::string>("variable1")->set("hallo");

    const bamboo::nodes::Inputs inputs(std::move(storage1));
    EXPECT_EQ(storage1, nullptr);

    auto test_variable = inputs.get<std::string>("variable1");
    EXPECT_EQ(test_variable, "hallo");
}

TEST(Nodes_EvaluationContext, EvaluationVector_Simple1)
{
    auto nodeGraph1 = std::make_shared<bamboo::nodes::NodeGraph>();
    auto evaluationContext = std::make_shared<bamboo::nodes::NodeGraphEvaluationContext>(nodeGraph1);

    EXPECT_NO_THROW(evaluationContext->getEvaluationOrder());

    auto evaluationOrder = evaluationContext->getEvaluationOrder();
    EXPECT_EQ(evaluationOrder.size(), 0);
}

TEST(Nodes_EvaluationContext, EvaluationVector_Simple2)
{
    class TestSumNode : public bamboo::nodes::Node
    {
    public:
        TestSumNode(std::string name) : bamboo::nodes::Node(name, "TestNode", { bamboo::nodes::TypedInputDescriptor<int>::create("input1"),
            bamboo::nodes::TypedInputDescriptor<int>::create("input2") }, { bamboo::nodes::TypedOutputDescriptor<int>::create("sum") }) {};

        std::shared_ptr<bamboo::nodes::Node> clone() const override { return std::make_shared<TestSumNode>(m_name); }
    };

    auto nodeGraph1 = std::make_shared<bamboo::nodes::NodeGraph>();
    auto evaluationContext = std::make_shared<bamboo::nodes::NodeGraphEvaluationContext>(nodeGraph1);

    nodeGraph1->createNode<TestSumNode>("node1");

    EXPECT_THROW(evaluationContext->getEvaluationOrder(), std::runtime_error);
}

TEST(Nodes_EvaluationContext, EvaluationVector_Simple3_CheckOrder)
{
    class TestSumNode : public bamboo::nodes::Node
    {
    public:
        TestSumNode(std::string name) : bamboo::nodes::Node(name, "TestNode", { bamboo::nodes::TypedInputDescriptor<int>::create("input1"),
            bamboo::nodes::TypedInputDescriptor<int>::create("input2") }, { bamboo::nodes::TypedOutputDescriptor<int>::create("sum") }) {};

        std::shared_ptr<bamboo::nodes::Node> clone() const override { return std::make_shared<TestSumNode>(m_name); }
    };


    class TestOutIntNode : public bamboo::nodes::Node
    {
    public:
        TestOutIntNode(std::string name) : bamboo::nodes::Node(name, "OutIntNode", {}, { bamboo::nodes::TypedOutputDescriptor<int>::create("out") }) {};

        std::shared_ptr<bamboo::nodes::Node> clone() const override { return std::make_shared<TestOutIntNode>(m_name); }
    };

    auto nodeGraph1 = std::make_shared<bamboo::nodes::NodeGraph>();
    auto evaluationContext = std::make_shared<bamboo::nodes::NodeGraphEvaluationContext>(nodeGraph1);

    auto sumNode = nodeGraph1->createNode<TestSumNode>("sum_node");
    auto outNode1 = nodeGraph1->createNode<TestOutIntNode>("node_Int1");
    auto outNode2 = nodeGraph1->createNode<TestOutIntNode>("node_Int2");

    nodeGraph1->setConnection(outNode1, "out", sumNode, "input1");
    nodeGraph1->setConnection(outNode2, "out", sumNode, "input2");

    EXPECT_NO_THROW(evaluationContext->getEvaluationOrder());

    auto evaluationOrder = evaluationContext->getEvaluationOrder();
    EXPECT_EQ(evaluationOrder.size(), 3);

    auto checkEvaluateXbeforeY = [&](bamboo::nodes::nodeID x, bamboo::nodes::nodeID y) -> bool {
        auto v = evaluationOrder;
        auto iterX = std::find(std::begin(v), std::end(v), x);
        auto iterY = std::find(std::begin(v), std::end(v), y);

        return (iterX < iterY);
    };

    EXPECT_TRUE(checkEvaluateXbeforeY(outNode1, sumNode));
    EXPECT_TRUE(checkEvaluateXbeforeY(outNode2, sumNode));
}

TEST(Nodes_EvaluationContext, EvaluationVector_Simple4_CheckOrder)
{
    class TestSumNode : public bamboo::nodes::Node
    {
    public:
        TestSumNode(std::string name) : bamboo::nodes::Node(name, "TestNode", { bamboo::nodes::TypedInputDescriptor<int>::create("input1"),
            bamboo::nodes::TypedInputDescriptor<int>::create("input2") }, { bamboo::nodes::TypedOutputDescriptor<int>::create("sum") }) {};

        std::shared_ptr<bamboo::nodes::Node> clone() const override { return std::make_shared<TestSumNode>(m_name); }
    };
    
    class TestPrintNode : public bamboo::nodes::Node {
    public:
        TestPrintNode(std::string name) : bamboo::nodes::Node(name, "PrintIntNode", { bamboo::nodes::TypedInputDescriptor<int>::create("in") }, {  }) {};

        std::shared_ptr<bamboo::nodes::Node> clone() const override { return std::make_shared<TestPrintNode>(m_name); }
    };

    class TestOutIntNode : public bamboo::nodes::Node
    {
    public:
        TestOutIntNode(std::string name) : bamboo::nodes::Node(name, "OutIntNode", {}, { bamboo::nodes::TypedOutputDescriptor<int>::create("out") }) {};

        std::shared_ptr<bamboo::nodes::Node> clone() const override { return std::make_shared<TestOutIntNode>(m_name); }
    };

    auto nodeGraph1 = std::make_shared<bamboo::nodes::NodeGraph>();
    auto evaluationContext = std::make_shared<bamboo::nodes::NodeGraphEvaluationContext>(nodeGraph1);

    auto sumNode = nodeGraph1->createNode<TestSumNode>("sum_node");
    auto outNode1 = nodeGraph1->createNode<TestOutIntNode>("node_Int1");
    auto outNode2 = nodeGraph1->createNode<TestOutIntNode>("node_Int2");
    auto printNode = nodeGraph1->createNode<TestPrintNode>("print_node");

    nodeGraph1->setConnection(outNode1, "out", sumNode, "input1");
    nodeGraph1->setConnection(outNode2, "out", sumNode, "input2");
    nodeGraph1->setConnection(sumNode, "sum", printNode, "in");

    EXPECT_NO_THROW(evaluationContext->getEvaluationOrder());

    auto evaluationOrder = evaluationContext->getEvaluationOrder();
    EXPECT_EQ(evaluationOrder.size(), 4);

    auto checkEvaluateXbeforeY = [&](bamboo::nodes::nodeID x, bamboo::nodes::nodeID y) -> bool {
        auto v = evaluationOrder;
        auto iterX = std::find(std::begin(v), std::end(v), x);
        auto iterY = std::find(std::begin(v), std::end(v), y);

        return (iterX < iterY);
    };

    EXPECT_TRUE(checkEvaluateXbeforeY(outNode1, printNode));
    EXPECT_TRUE(checkEvaluateXbeforeY(outNode1, sumNode));
    EXPECT_TRUE(checkEvaluateXbeforeY(outNode2, sumNode));
    EXPECT_TRUE(checkEvaluateXbeforeY(sumNode, printNode));
}

TEST(Nodes_EvaluationContext, EvaluationVector_Simple5_CyclicReference)
{
    class TestNode1 : public bamboo::nodes::Node {
    public:
        TestNode1(std::string name) : bamboo::nodes::Node(name, "TestNode1", { bamboo::nodes::TypedInputDescriptor<int>::create("in") }, { bamboo::nodes::TypedOutputDescriptor<int>::create("out")  }) {};

        std::shared_ptr<bamboo::nodes::Node> clone() const override { return std::make_shared<TestNode1>(m_name); }
    };

    class TestNode2 : public bamboo::nodes::Node
    {
    public:
        TestNode2(std::string name) : bamboo::nodes::Node(name, "TestNode2", { bamboo::nodes::TypedInputDescriptor<int>::create("in") }, { bamboo::nodes::TypedOutputDescriptor<int>::create("out") }) {};

        std::shared_ptr<bamboo::nodes::Node> clone() const override { return std::make_shared<TestNode2>(m_name); }
    };

    auto nodeGraph1 = std::make_shared<bamboo::nodes::NodeGraph>();
    auto evaluationContext = std::make_shared<bamboo::nodes::NodeGraphEvaluationContext>(nodeGraph1);

    auto node1 = nodeGraph1->createNode<TestNode1>("node1");
    auto node2 = nodeGraph1->createNode<TestNode2>("node2");

    nodeGraph1->setConnection(node1, "out", node2, "in");
    nodeGraph1->setConnection(node2, "out", node1, "in");

    EXPECT_THROW(evaluationContext->getEvaluationOrder(), std::runtime_error);
}

TEST(Nodes_EvaluationContext, SimpleEvaluation)
{
    class TestNode1 : public bamboo::nodes::Node {
    public:
        TestNode1(std::string name) : bamboo::nodes::Node(name, "TestNode1", { }, { }), m_testVariable(nullptr) {};
        
        bool execute(const bamboo::nodes::Inputs &in, bamboo::nodes::Outputs &out) const override { if (m_testVariable) m_testVariable->assign("executed"); return (m_testVariable != nullptr); };

        void setTestVariablePtr(std::string * ptr) { m_testVariable = ptr; }
        std::shared_ptr<bamboo::nodes::Node> clone() const override { return std::make_shared<TestNode1>(m_name); }
    private:
        std::string * m_testVariable;
    };

    std::string testVariable("not executed");

    auto nodeGraph1 = std::make_shared<bamboo::nodes::NodeGraph>();
    auto evaluationContext = std::make_shared<bamboo::nodes::NodeGraphEvaluationContext>(nodeGraph1);

    auto node1 = nodeGraph1->createNode<TestNode1>("node1");
    
    auto nodePtr = nodeGraph1->getNode(node1);
    auto typedNodePtr = std::dynamic_pointer_cast<TestNode1>(nodePtr);
    typedNodePtr->setTestVariablePtr(&testVariable);

    bool result = evaluationContext->evaluate();

    EXPECT_TRUE(testVariable.compare("executed") == 0);

    EXPECT_TRUE(result);
    (void)node1;
}

TEST(Nodes_EvaluationContext, SimpleEvaluation_2Nodes)
{
    class TestNode1 : public bamboo::nodes::Node {
    public:
        TestNode1(std::string name) : bamboo::nodes::Node(name, "TestNode1", {}, { bamboo::nodes::TypedOutputDescriptor<int>::create("out")  }) {};

        bool execute(const bamboo::nodes::Inputs &in, bamboo::nodes::Outputs &out) const override { out.set<int>("out", 42); return true; };

        std::shared_ptr<bamboo::nodes::Node> clone() const override { return std::make_shared<TestNode1>(m_name); }
    };

    class TestNode2 : public bamboo::nodes::Node {
    public:
        TestNode2(std::string name) : bamboo::nodes::Node(name, "TestNode2", { bamboo::nodes::TypedInputDescriptor<int>::create("in")  }, {}) {};

        bool execute(const bamboo::nodes::Inputs &in, bamboo::nodes::Outputs &out) const override { std::cout << in.get<int>("in") << std::endl; return true; };

        std::shared_ptr<bamboo::nodes::Node> clone() const override { return std::make_shared<TestNode1>(m_name); }
    };
     
    std::string testVariable("not executed");

    auto nodeGraph1 = std::make_shared<bamboo::nodes::NodeGraph>();
    auto evaluationContext = std::make_shared<bamboo::nodes::NodeGraphEvaluationContext>(nodeGraph1);

    auto node1 = nodeGraph1->createNode<TestNode1>("node1");
    auto node2 = nodeGraph1->createNode<TestNode2>("node2");
    auto node3 = nodeGraph1->createNode<TestNode2>("node3");

    nodeGraph1->setConnection(node1, "out", node2, "in");
    nodeGraph1->setConnection(node1, "out", node3, "in");

    bool result = evaluationContext->evaluate();

    EXPECT_TRUE(result);
    (void)node1; 
}