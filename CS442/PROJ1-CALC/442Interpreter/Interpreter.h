#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <stack>
#include <regex>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <array>
#include <variant>
#include <functional>
#include <optional>
#include <vector>
#include <deque>


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
		EQL,
		L_PAREN,
		R_PAREN
	};

	enum Errors
	{
		INVALID_SYNTAX,
		UNDECL_VAR,
		UNINIT_VAR,
		EXIST_VAR
	};

	enum EvalActions
	{
		BEGIN,
		VARIABLE,
		KEYWORD,
		OPERATOR,
		VALUE
	};

	variableContainer varMap;
	std::deque<std::string> tokenQueue;
	std::stack<std::string> tokenStack;
	
	struct OpPrec {
		OpPrec(Operators pOp, int pPrec) {
			op = pOp; prec = pPrec;
		}
		Operators op;
		int prec;
	};

	const std::unordered_map<std::string, Keywords> KeywordMap{
		{"load", LOAD},
		{"mem", MEM},
		{"print", PRINT},
		{"stop", STOP},
		{"sqrt", SQRT}
	};

	const std::unordered_map<char, OpPrec> OperatorMap{
		{'+', OpPrec(ADD, 1)},
		{'-', OpPrec(SUB, 1)},
		{'*', OpPrec(MULT, 2)},
		{'/', OpPrec(DIV, 2)},
		{'^', OpPrec(PWR, 3)},
		{'=', OpPrec(EQL, 0)},
		{'(', OpPrec(L_PAREN, 4)},
		{')', OpPrec(R_PAREN, 4)}
	};

	Interpreter();
	~Interpreter();

	void TokenizeInput(std::string);
	void HandlePrint();
	void HandleStop();
	float applyOp(float curVal, float appVal, char op);
	void HandleMem();
	void HandleStatement();
	void HandleLoad();
	void HandleCommand(int);
	
	Keywords ParseCommand();
	
	std::optional<float> CreateVariable(float* = nullptr);
	
	
	bool OperatorExists(char);
	int GetOperatorPrecedence(char);
	bool KeywordExists(std::string);
	bool VariableExists(std::string);
	void PrintErrorStatement(Errors, std::string = "UNKNOWN");
	bool IsValidVariableName(std::string);

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