#ifndef __BAMBOOENGINE_NODES_VARIABLES_H_
#define __BAMBOOENGINE_NODES_VARIABLES_H_

#include <memory>
#include <string_view>

namespace bamboo
{
    namespace nodes
    {
        class IVariable
        {
        public:
            size_t typeHash() const { return m_typeHash; }
            virtual ~IVariable() {};

        protected:
            IVariable(size_t typeHash) : m_typeHash(typeHash) {};
            const size_t      m_typeHash;
        };

        template <class T> class TypedVariable : public IVariable
        {
        public:
            TypedVariable() : IVariable(typeid(T).hash_code()) {};

            T get() const { return m_value; };
            void set(T value) { m_value = value; };

            static constexpr size_t StaticTypeHash() { return typeid(T).hash_code(); }

        private:
            T m_value;
        };


        class SharedVariableMap
        {
        public:
            template <typename T> std::shared_ptr<TypedVariable<T>> create(const std::string &name) { m_variables[name] = std::make_shared<TypedVariable<T>>(); return get<T>(name); }

            void replace(const std::string &name, std::shared_ptr<IVariable> &v) { m_variables[name] = v;}

            void remove(std::string_view name) { 
                auto iter = m_variables.find(name);
                if (iter != m_variables.end())
                    iter->second = nullptr;
                /*if (auto iter = m_variables.find(name); iter != m_variables.end())
                    iter->second = nullptr;*/
            }

            std::shared_ptr<IVariable> get(std::string_view name) const {
                auto iter = m_variables.find(name);
                return (iter != m_variables.end() ? iter->second : nullptr);
            };

            template <typename T> std::shared_ptr<TypedVariable<T>> get(std::string_view name) const {
                auto untyped_variable = get(name);
                return std::dynamic_pointer_cast<TypedVariable<T>>(untyped_variable);
            }

        private:
            std::map<std::string, std::shared_ptr<IVariable>, std::less<>> m_variables;
        };
    }
}

#endif