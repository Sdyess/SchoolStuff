#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <regex>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <array>
#include <optional>
#include <functional>


typedef std::vector<std::string> tokenContainer;
typedef std::unordered_map<std::string, std::optional<float>> variableContainer;

class Interpreter
{
public:
	enum Keywords
	{
		LOAD,
		MEM,
		PRINT,
		STOP,
		STMT,
		SQRT
	};

	enum Operators
	{
		ADD,
		SUB,
		MULT,
		DIV,
		PWR,
		EQL
	};

	variableContainer varMap;
	tokenContainer tokenVec;
	
	const std::unordered_map<std::string, Keywords> KeywordMap{
		{"load", LOAD},
		{"mem", MEM},
		{"print", PRINT},
		{"stop", STOP},
		{"sqrt", SQRT}
	};

	const std::unordered_map<char, Operators> OperatorMap{
		{'+', ADD},
		{'-', SUB},
		{'*', MULT},
		{'/', DIV},
		{'^', PWR},
		{'=', EQL}
	};

	Interpreter();
	~Interpreter();

	void TokenizeInput(std::string);
	void HandlePrint();
	void HandleStop();
	void HandleMem();
	void HandleStatement();
	void HandleLoad();
	void HandleCommand(int);
	Keywords ParseCommand();
	std::optional<float> CreateVariable(float* = nullptr);
	bool OperatorExists(char);

	std::string &ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c); }));
		return s;
	}

	std::string &rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {return !std::isspace(ch); }).base(), s.end());
		return s;
	}

	std::string toUpper(std::string s)
	{
		std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
		return s;
	}

	std::string toLower(std::string s)
	{
		std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c);  });
		return s;
	}
	
};