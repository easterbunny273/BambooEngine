#pragma once

#ifndef __BAMBOOENGINE_CONFIG_H_
#define __BAMBOOENGINE_CONFIG_H_

#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <string>

namespace bamboo
{

	class IConfiguration
	{
	public:
		class ISetting
		{
		public:
			virtual void readFromPropertyTree(const boost::property_tree::ptree &tree) = 0;
			virtual void writeToPropertyTree(boost::property_tree::ptree &tree) const = 0;
		};

		class Section
		{
		public:
			Section(const std::string &name) : m_name(name) {};
			Section(const char * szName) : m_name(szName) {};

			void addSetting(ISetting *setting) { m_settings.push_back(setting); };

			void writeToPropertyTree(boost::property_tree::ptree &tree) const;
			void readFromPropertyTree(const boost::property_tree::ptree &tree);

			const std::string & getName() const { return m_name; }

		private:
			std::string				m_name;
			std::vector<ISetting*>	m_settings;
		};

		template <class T> class Setting : public ISetting
		{
			friend class Section;

		public:
			Setting(Section &section, const std::string &name, const T &defaultValue) : m_name(section.getName() + std::string(".") + name), m_defaultValue(defaultValue) { section.addSetting(this); };

			void set(T value) { m_value = value; };
			T value() const { return m_value; }

		private:
			void readFromPropertyTree(const boost::property_tree::ptree &tree) override
			{
				m_value = tree.get<T>(m_name, m_defaultValue);
			};

			void writeToPropertyTree(boost::property_tree::ptree &tree) const override
			{
				tree.put(m_name, m_value);
			};

		private:
			const std::string	m_name;
			const T				m_defaultValue;
			T					m_value;
		};
	};

	class CoreConfig : public IConfiguration
	{
	public:
		CoreConfig(const std::string &filename) : m_filename(filename) { loadFromFile(); };
		~CoreConfig() { writeToFile(); };

		void flush() const { writeToFile(); }

	private:
		void loadFromFile();
		void writeToFile() const;

		const std::string		m_filename;

	public:
		struct TSettings_Network {
			Section					_section{ "network" };
			Setting<int>			serverPort{ _section, "server_port", 6000 };
		} network;
	};
};
#endif