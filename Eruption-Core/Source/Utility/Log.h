#pragma once
#include "Singleton.h"

namespace Eruption { namespace Utility {

	class Log : Singleton
	{

		friend class Singleton;

	private:
		enum LogType {
			Info	= 15,
			Success = 10,
			Warning = 14,
			Error	= 12
		};

	public:

		void Initialize();

		void LogInfo(const string& engineSystem, const string& message);
		void LogError(const string& engineSystem, const string& message);
		void LogWarning(const string& engineSystem, const string& message);
		void LogSuccess(const string& engineSystem, const string& message);

		void NewLine() { cout << endl; }

	private: 

		// private singleton constructor
		Log()
		{
		}

		void LogMessage(const string& engineSystem, const string& message, LogType type);

		void CreateDebugConsole();

	};

} }