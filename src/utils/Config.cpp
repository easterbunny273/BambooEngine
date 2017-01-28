#include "Config.h"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace bamboo;

void bamboo::IConfiguration::Section::writeToPropertyTree(boost::property_tree::ptree &tree) const
{
	for (auto &item : m_settings)
		item->writeToPropertyTree(tree);
}

void bamboo::IConfiguration::Section::readFromPropertyTree(const boost::property_tree::ptree &tree)
{
	for (auto &item : m_settings)
		item->readFromPropertyTree(tree);
}

void bamboo::CoreConfig::loadFromFile()
{
	boost::property_tree::ptree tree;

	try
	{
		boost::property_tree::read_ini(m_filename, tree);
	}
	catch (...)
	{
		// no config file found
	}

	network._section.readFromPropertyTree(tree);
}

void bamboo::CoreConfig::writeToFile() const
{
	boost::property_tree::ptree tree;

	network._section.writeToPropertyTree(tree);

	boost::property_tree::write_ini(m_filename, tree);
}
