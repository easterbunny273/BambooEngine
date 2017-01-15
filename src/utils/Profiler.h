#ifndef __BAMBOO_PROFILER_H_
#define __BAMBOO_PROFILER_H_

#include <string>
#include <stack>
#include <map>
#include <chrono>
#include <memory>

namespace bamboo
{
	using namespace std::literals;

	class Profiler
	{
	private:
		class ProfileFunctionGuard;

	public:
		using ProfileID = size_t;


		void debugHierarchical();

		std::unique_ptr<ProfileFunctionGuard> functionProfiling(const char * funcName);

	private:
		class ProfileFunctionGuard
		{
		public:
			ProfileFunctionGuard(Profiler &profiler, const char * funcName);
			~ProfileFunctionGuard();

		private:
			Profiler			* m_profiler;
			Profiler::ProfileID m_profileID;
		};


		using TimePoint = std::chrono::high_resolution_clock::time_point;
		using Duration = std::chrono::duration<double>;

		struct TProfilingInformation
		{
			size_t					runs = 0;
			Duration::rep			total_time = 0;
		};

		struct THierarchicalProfilingInformation
		{
			size_t					runs = 0;
			Duration::rep			total_time = 0;

			std::map<ProfileID, THierarchicalProfilingInformation> childInfo;
		};

		using FlatMap = std::map<ProfileID, TProfilingInformation>;
		using HierarchicalMap = std::map<ProfileID, THierarchicalProfilingInformation>;

		ProfileID getID(const char * currentFunc);

		void start_nested(ProfileID id);
		void stop_nested(ProfileID id);
		void ItlDebugHierarchical(const HierarchicalMap::iterator& iter, int level, const Duration::rep &parent_duration);

		std::map<ProfileID, std::string> m_generatedIDs;

		std::stack<std::pair<ProfileID, TimePoint>>		m_currentStack;
		std::stack<THierarchicalProfilingInformation*>	m_parentForHierarchical;

		FlatMap	m_infos;
		HierarchicalMap	m_hierarchicalInfos;

	};


}
#endif