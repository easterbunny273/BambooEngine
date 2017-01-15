#include "Profiler.h"
#include <cassert>
#include <iostream>

constexpr bool USE_PROFILING = true;

bamboo::Profiler::ProfileID bamboo::Profiler::getID(const char * currentFunc)
{
	std::hash<std::string> hash_fn;
	auto hashresult = hash_fn(currentFunc);

	m_generatedIDs[hashresult] = currentFunc;

	return hashresult;
}

void bamboo::Profiler::start_nested(ProfileID id)
{
	auto new_item = std::make_pair(id, std::chrono::high_resolution_clock::now());

	m_currentStack.push(new_item);

	auto &hierarchicalInfoStruct = m_parentForHierarchical.empty() ? m_hierarchicalInfos[id] : m_parentForHierarchical.top()->childInfo[id];

	m_parentForHierarchical.push(&hierarchicalInfoStruct);
}


void bamboo::Profiler::stop_nested(ProfileID id)
{
	assert(m_currentStack.top().first == id);

	auto now = std::chrono::high_resolution_clock::now();
	auto started = m_currentStack.top().second;

	m_currentStack.pop();

	auto duration = std::chrono::duration_cast<Duration>(now - started);

	m_infos[id].runs		+= 1;
	m_infos[id].total_time  += duration.count();

	auto &hierarchicalInfoStruct = m_parentForHierarchical.top();
	hierarchicalInfoStruct->runs += 1;
	hierarchicalInfoStruct->total_time += duration.count();
	m_parentForHierarchical.pop();
}

void bamboo::Profiler::debugHierarchical()
{
	if (!USE_PROFILING)
		return;

	Duration::rep parent_duration(0);
	for (auto iter = m_hierarchicalInfos.begin(); iter != m_hierarchicalInfos.end(); ++iter)
		parent_duration += iter->second.total_time;

	for (auto iter = m_hierarchicalInfos.begin(); iter != m_hierarchicalInfos.end(); ++iter)
	{
		ItlDebugHierarchical(iter, 0, parent_duration);
	}
}

std::unique_ptr<bamboo::Profiler::ProfileFunctionGuard> bamboo::Profiler::functionProfiling(const char * funcName)
{
	if (!USE_PROFILING)
		return nullptr;

	return std::unique_ptr<ProfileFunctionGuard>(new ProfileFunctionGuard(*this, funcName));
}

void bamboo::Profiler::ItlDebugHierarchical(const HierarchicalMap::iterator& iter, int level, const Duration::rep &parent_duration)
{
	std::string _padding_string("  ");

	std::string padding;
	for (int i = 0; i < level; ++i)
		padding += _padding_string;

	auto relative_time = 100.0f * (iter->second.total_time / parent_duration);
	std::cout << padding << m_generatedIDs[iter->first] << " (runs=" << iter->second.runs << ", total=" << iter->second.total_time << "sec, relative=" << relative_time << "% )\n";

	auto total_time_unprofiled = iter->second.total_time;
	for (auto iter2 = iter->second.childInfo.begin(); iter2 != iter->second.childInfo.end(); ++iter2)
	{
		total_time_unprofiled -= iter2->second.total_time;
		ItlDebugHierarchical(iter2, level + 1, iter->second.total_time);
	}

	if (iter->second.childInfo.empty() == false)
	{
		auto relative_time_unprofiled = 100.0f * (total_time_unprofiled / iter->second.total_time);
		std::cout << padding << _padding_string << "_other_ (total=" << total_time_unprofiled << "sec, relative=" << relative_time_unprofiled << "% )\n";
	}
}

bamboo::Profiler::ProfileFunctionGuard::ProfileFunctionGuard(Profiler &profiler, const char * funcName)
	: m_profiler(&profiler), m_profileID(profiler.getID(funcName))
{
	m_profiler->start_nested(m_profileID);
}

bamboo::Profiler::ProfileFunctionGuard::~ProfileFunctionGuard()
{
	m_profiler->stop_nested(m_profileID);
}