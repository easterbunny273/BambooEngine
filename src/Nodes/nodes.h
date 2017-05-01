#ifndef __BAMBOOENGINE_NODES_H_
#define __BAMBOOENGINE_NODES_H_

#include <vector>
#include <typeinfo>
#include <memory>
#include <map>
#include <unordered_map>
#include <cassert>

#include <string>
#include <string_view>

#include "variables.h"

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
            static std::shared_ptr<TypedInputDescriptor<T>> create(const std::string &name) { return std::shared_ptr<TypedInputDescriptor<T>>(new TypedInputDescriptor<T>(name)); }

        protected:
            TypedInputDescriptor(const std::string &name) : InputDescriptor(name, typeid(T).hash_code()) {}
        };

        class OutputDescriptor : public DescriptorBase {
        public:
            std::shared_ptr<IVariable> createVariable() const { return createTypedVariable(); }

        protected:
            OutputDescriptor(const std::string &name, size_t typeHash) : DescriptorBase(name, typeHash) {};

            virtual std::shared_ptr<IVariable> createTypedVariable() const { assert(!"do not call base type"); return nullptr; };
        };

        template <class T> class TypedOutputDescriptor : public OutputDescriptor
        {
        public:
            static std::shared_ptr<TypedOutputDescriptor<T>> create(const std::string &name) { return std::shared_ptr<TypedOutputDescriptor<T>>(new TypedOutputDescriptor<T>(name)); }

        protected:
            TypedOutputDescriptor(const std::string &name) : OutputDescriptor(name, typeid(T).hash_code()) {}
            std::shared_ptr<IVariable> createTypedVariable() const override final { return std::make_shared<TypedVariable<T>>(); };
        };

        class Inputs
        {
        public:
            Inputs(std::shared_ptr<SharedVariableMap> wrappedMap) : m_variables(wrappedMap) {};
            Inputs(std::unique_ptr<SharedVariableMap> &&wrappedMap) : m_variables(std::move(wrappedMap)) {};

            template <typename T> T get(std::string_view name) const
            {
                auto typed_variable_object = m_variables->get<T>(name);
                return typed_variable_object->get();
            }

        private:
            const std::shared_ptr<const SharedVariableMap> m_variables;
        };

        class Outputs
        {
        public:
            Outputs(std::shared_ptr<SharedVariableMap> wrappedMap) : m_variables(wrappedMap) {};

            template <typename T> void set(std::string_view name, const T &value) const
            {
                auto typed_variable_object = m_variables->get<T>(name);
                typed_variable_object->set(value);
            }

        private:
            std::shared_ptr<SharedVariableMap> m_variables;
        };



        class Node {
        public:
            using InputDescriptorsVec = std::vector<std::shared_ptr<InputDescriptor>>;
            using OutputDescriptorsVec = std::vector<std::shared_ptr<OutputDescriptor>>;

            const auto& getInputs() const { return m_inputs; };
            const auto& getOutputs() const { return m_outputs; }

            const std::shared_ptr<InputDescriptor> getInput(std::string_view name) const;
            const std::shared_ptr<OutputDescriptor> getOutput(std::string_view name) const;

            void setName(std::string name) { m_name = name; };
            std::string getName() const { return m_name; }
            std::string getNodeType() const { return m_nodeType; }

            virtual std::shared_ptr<Node> clone() const = 0;
            
            virtual bool execute(const Inputs &in, Outputs &out) const { assert(!"execute method not overwritten"); return false; };

        protected:
            Node(const std::string &name, const std::string &nodeType, InputDescriptorsVec inputs, OutputDescriptorsVec outputs) : m_name(name), m_nodeType(nodeType), m_inputs(inputs), m_outputs(outputs) {};

            std::string       m_name;
            const std::string m_nodeType;

            const InputDescriptorsVec   m_inputs;
            const OutputDescriptorsVec  m_outputs;
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

        class NodeGraph;

        class NodeGraphEvaluationContext
        {
        public:
            NodeGraphEvaluationContext(std::shared_ptr<NodeGraph> graph) : m_nodeGraph(graph) {};

            template <typename T> void setDirectInput(nodeID node, std::string inputName, const T& value) {
                if (m_directInputs[node] == nullptr)
                    m_directInputs[node] = std::make_shared<SharedVariableMap>();

                auto variable = m_directInputs[node]->get<T>(inputName);
                if (!variable)
                    variable = m_directInputs[node]->create<T>(inputName);

                variable->set(value);
            };

            void clearDirectInput(nodeID node, std::string inputName) {
                if (m_directInputs[node] == nullptr)
                    return;

                m_directInputs[node]->remove(inputName);
            }

            bool hasDirectInput(nodeID node, std::string inputName);
           
            std::vector<bamboo::nodes::nodeID> getEvaluationOrder();

            bool evaluate();

        private:

            std::shared_ptr<NodeGraph>                           m_nodeGraph;
            std::map<nodeID, std::shared_ptr<SharedVariableMap>> m_directInputs;
            std::map<nodeID, std::shared_ptr<SharedVariableMap>> m_outputVariablesPerNode;

        };

        class NodeGraph {
        public:
            template <class concrete_node> auto createNode(std::string name) { 
                auto newNode = std::make_shared<concrete_node>(name); 
                auto newID = getNextFreeNodeID();
                m_nodes[newID] = newNode;
                return newID;
            };

            nodeID createNodeFromPrototype(std::string name, NodeFactory &factory, std::string typeName) {
                auto newID = getNextFreeNodeID();
                auto newNode = factory.createNodeFromPrototype(typeName);

                if (!newNode)
                    return 0;

                m_nodes[newID] = newNode;
            }

            bool removeNode(nodeID node);

            bool setConnection(nodeID srcNode, const std::string &srcOutput,
                nodeID dstNode, const std::string &dstInput);

            
            std::shared_ptr<Connection> getConnection(nodeID dstNode, const std::string &dstInput);
            std::vector<Connection> getInputConnections(nodeID dstNode);
            std::vector<Connection> getOutputConnections(nodeID srcNode);
            std::vector<Connection> getConnections(nodeID node);

            bool hasConnection(nodeID dstNode, const std::string &dstInput);
            bool removeConnection(nodeID dstNode, const std::string &dstInput);
            bool removeConnections(nodeID node);

            std::shared_ptr<Node> getNode(nodeID _nodeID) const;
            auto getNodes() const { return m_nodes; }

            std::string writeToJSON();
            static std::shared_ptr<NodeGraph> readFromJSON(const std::string &json, NodeFactory &factory);

        private:
            nodeID getNextFreeNodeID() { static nodeID TEST = 1;  return TEST++; }


            std::unordered_map<nodeID, std::shared_ptr<Node>> m_nodes;

            std::vector<Connection> m_connections;
        };


    }
}

#endif