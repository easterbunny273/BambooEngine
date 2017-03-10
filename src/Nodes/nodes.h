#include <vector>
#include <typeinfo>
#include <memory>
#include <unordered_map>

#include <boost/optional.hpp>

namespace bamboo
{
    namespace nodes
    {
        using nodeID = size_t;

        class DescriptorBase {
        public:
            DescriptorBase(const std::string &name, size_t typeHash) : m_name(name), m_typeHash(typeHash) {};
            virtual ~DescriptorBase() {};

            const std::string & getName() const { return m_name; };
            size_t              getTypeHash() const { return m_typeHash; };
        protected:
            const std::string m_name;
            const size_t      m_typeHash;
        };

        class InputDescriptor : public DescriptorBase {
        public:
            InputDescriptor(const std::string &name, size_t typeHash) : DescriptorBase(name, typeHash) {};
        };

        template <class T> class TypedInputDescriptor : public InputDescriptor
        {
        public:
            TypedInputDescriptor(const std::string &name) : InputDescriptor(name, typeid(T).hash_code()) {}
        };

        class OutputDescriptor : public DescriptorBase {
        public:
            OutputDescriptor(const std::string &name, size_t typeHash) : DescriptorBase(name, typeHash) {};
        };

        template <class T> class TypedOutputDescriptor : public OutputDescriptor
        {
        public:
            TypedOutputDescriptor(const std::string &name) : OutputDescriptor(name, typeid(T).hash_code()) {}
        };

        class Node {
        public:
            const auto& getInputs() const { return m_inputs; };
            const auto& getOutputs() const { return m_outputs; }

            const InputDescriptor* getInput(const std::string &name) const;
            const OutputDescriptor* getOutput(const std::string &name) const;

            void setName(std::string name) { m_name = name; };
            std::string getName() const { return m_name; }
            std::string getNodeType() const { return m_nodeType; }

            virtual std::shared_ptr<Node> clone() const = 0;

        protected:
            Node(const std::string &name, const std::string &nodeType, std::vector<InputDescriptor> inputs, std::vector<OutputDescriptor> outputs) : m_name(name), m_nodeType(nodeType), m_inputs(inputs), m_outputs(outputs) {};

            std::string       m_name;
            const std::string m_nodeType;
            const std::vector<InputDescriptor>    m_inputs;
            const std::vector<OutputDescriptor>   m_outputs;
        };

        class Connection 
        {
        public:
            Connection(nodeID srcNode, const std::string &srcOutput, nodeID dstNode, const std::string &dstInput) : m_srcNode(srcNode), m_srcOutput(srcOutput), m_dstNode(dstNode), m_dstInput(dstInput) {};

            nodeID         m_srcNode, m_dstNode;
            std::string    m_srcOutput, m_dstInput;
        };

        class NodeFactory {
        public:
            bool registerPrototype(std::shared_ptr<Node> nodePrototype);
            const std::vector<std::shared_ptr<Node>> getPrototypes();
            std::shared_ptr<Node> createNodeFromPrototype(std::string type);

        private:
            std::vector<std::shared_ptr<Node>>      m_registeredPrototypes;
        };


        class NodeGraph {
        public:
            template <class concrete_node> auto createNode(std::string name) { 
                auto newNode = std::make_shared<concrete_node>(name); 
                auto newID = getNextFreeNodeID();
                m_nodes[newID] = newNode;
                return newID;
            };

            boost::optional<nodeID> createNodeFromPrototype(std::string name, NodeFactory &factory, std::string typeName) {
                auto newID = getNextFreeNodeID();
                auto newNode = factory.createNodeFromPrototype(typeName);

                if (!newNode)
                    return boost::optional<nodeID>();

                m_nodes[newID] = newNode;
            }

            bool removeNode(nodeID node);

            bool setConnection(nodeID srcNode, const std::string &srcOutput,
                nodeID dstNode, const std::string &dstInput);

            
            boost::optional<Connection> getConnection(nodeID dstNode, const std::string &dstInput);
            std::vector<Connection> getInputConnections(nodeID dstNode);
            std::vector<Connection> getOutputConnections(nodeID srcNode);
            std::vector<Connection> getConnections(nodeID node);

            bool removeConnection(nodeID dstNode, const std::string &dstInput);
            bool removeConnections(nodeID node);

            std::shared_ptr<Node> getNode(nodeID _nodeID) const;

            std::string writeToJSON();
            static std::shared_ptr<NodeGraph> readFromJSON(const std::string &json, NodeFactory &factory);

        private:
            nodeID getNextFreeNodeID() { static nodeID TEST = 0;  return TEST++; }


            std::unordered_map<nodeID, std::shared_ptr<Node>> m_nodes;

            std::vector<Connection> m_connections;
        };
    }
}