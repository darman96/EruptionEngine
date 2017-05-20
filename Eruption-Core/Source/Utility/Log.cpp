#include "StdAfx.h"
#include "Log.h"

#ifdef _DEBUG
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <ios>
#endif

namespace Eruption { namespace Utility {

	void Log::Initialize() {
		
#ifdef _DEBUG
		CreateDebugConsole();
#endif

		// TODO: Log to File

	}

	void Log::CreateDebugConsole() {
		AllocConsole();
		SetConsoleTitle("Debug Console");

		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);

		cout.clear();
		cerr.clear();
		std::cin.clear();
	}

	void Log::LogMessage(const string& engineSystem, const string& message, LogType type)
	{
		string logType = "";
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(consoleHandle, type);

		switch (type)
		{
		case Info:
			logType = "Info";
			break;
		case Success:
			logType = "Success";
			break;
		case Warning:
			logType = "Warning";
			break;
		case Error:
			logType = "Error";
			break;
		default:
			break;
		}

		cout << "[" << logType << "][" << engineSystem << "]" << message << endl;
	}

	void Log::LogInfo(const string& engineSystem, const string& message) {
		LogMessage(engineSystem, message, Info);
	}

	void Log::LogError(const string& engineSystem, const string& message) {
		LogMessage(engineSystem, message, Error);
	}

	void Log::LogWarning(const string& engineSystem, const string& message) {
		LogMessage(engineSystem, message, Warning);
	}

	void Log::LogSuccess(const string& engineSystem, const string& message) {
		LogMessage(engineSystem, message, Success);
	}

} }