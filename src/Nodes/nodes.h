#include <vector>
#include <typeinfo>
#include <memory>
#include <map>
#include <unordered_map>
#include <cassert>

namespace bamboo
{
    namespace nodes
    {
        using nodeID = size_t;

        class IVariable
        {
        public:
            IVariable(size_t typeHash) : m_typeHash(typeHash) {};
            size_t typeHash() const { return m_typeHash; }
            virtual ~IVariable() {};

        protected:
            const size_t      m_typeHash;
        };

        template <class T> class TypedVariable : public IVariable
        {
        public:
            TypedVariable() : IVariable(typeid(T).hash_code()) {};

            T get() const { return m_value; };
            void set(T value) { m_value = value; };

        private:
            T m_value;
        };

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

        class SharedVariableMap
        {
        public:
            template <typename T> std::shared_ptr<TypedVariable<T>> create(const std::string &name) { m_variables[name] = std::make_shared<TypedVariable<T>>(); return get<T>(name); }

            void set(const std::string &name, std::shared_ptr<IVariable> &v) { m_variables[name] = v; }

            std::shared_ptr<IVariable> get(const std::string& name) { 
                auto iter = m_variables.find(name);
                if (iter != m_variables.end())
                    return iter->second;
                else
                    return nullptr;
            };

            template <typename T> std::shared_ptr<TypedVariable<T>> get(const std::string &name) {
                auto untyped_variable = get(name);
                return std::dynamic_pointer_cast<TypedVariable<T>>(untyped_variable);
            }

        private:
            std::map<std::string, std::shared_ptr<IVariable>> m_variables;
        };

        class Inputs
        {
        public:
            Inputs(std::shared_ptr<SharedVariableMap> wrappedMap) : m_variables(wrappedMap) {};
            Inputs(std::unique_ptr<SharedVariableMap> &&wrappedMap) : m_variables(std::move(wrappedMap)) {};

            template <typename T> T get(const std::string &name) const
            {
                auto typed_variable_object = m_variables->get<T>(name);

                return typed_variable_object->get();
            }

        private:
            std::shared_ptr<SharedVariableMap> m_variables;
        };

        class Outputs
        {
        public:
            Outputs(std::shared_ptr<SharedVariableMap> wrappedMap) : m_variables(wrappedMap) {};

            template <typename T> void set(const std::string &name, const T &value) const
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

            const std::shared_ptr<InputDescriptor> getInput(const std::string &name) const;
            const std::shared_ptr<OutputDescriptor> getOutput(const std::string &name) const;

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

            //template <typename T> bool setDirectInput(nodeID node, std::string inputName, T value);
            //template <typename T> T getOutput(nodeID node, std::string outputName);

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