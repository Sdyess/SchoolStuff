#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <regex>
#include <cctype>

typedef std::vector<std::string> tokenContainer;
typedef std::map<std::string, float> variableContainer;

class Interpreter
{
public:
	enum Commands
	{
		LOAD_COMMAND,
		MEM_COMMAND,
		PRINT_COMMAND,
		STOP_COMMAND,
		STMT_COMMAND
	};

	variableContainer varMap;
	tokenContainer tokenVec;
	
	const std::map<std::string, Commands> CommandMap{
		{"load", LOAD_COMMAND},
		{"mem", MEM_COMMAND},
		{"print", PRINT_COMMAND},
		{"stop", STOP_COMMAND}
	};

	Interpreter();

	tokenContainer TokenizeInput(std::string);
	void HandlePrint();
	void HandleStop();
	void HandleMem();
	void HandleStatement();
	void HandleLoad();
	void HandleCommand(int);
	Commands ParseCommand();

	static inline std::string &ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c); }));
		return s;
	}

	static inline std::string &rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {return !std::isspace(ch); }).base(), s.end());
		return s;
	}

	std::string toUpper(std::string s)
	{
		std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
		return s;
	}
	
};