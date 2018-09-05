#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <regex>
#include <cctype>


enum Commands
{
	LOAD_COMMAND,
	MEM_COMMAND,
	PRINT_COMMAND,
	STOP_COMMAND,
	STMT_COMMAND
};

Commands ParseCommand(std::vector<std::string> * vec, std::map<std::string, Commands> cmdMap)
{
	int command = -1;
	auto itr = cmdMap.find(vec->at(0));
	if (itr != cmdMap.end())
		command = itr->second;
	
	return static_cast<Commands>(command);
	
}

void HandleCommand(std::vector<std::string> * vec, int command, std::map<std::string, float> * vars)
{
	switch (command)
	{
	case LOAD_COMMAND:
		HandleLoad();
		break;
	case MEM_COMMAND:
		HandleMem();
		break;
	case PRINT_COMMAND:
		HandlePrint();
		break;
	case STOP_COMMAND:
		HandleStop();
		break;
	default:
		break;
	}
}
//these 3 functions get moved into a header later
static inline std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c); }));
	return s;
}

static inline std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {return !std::isspace(ch);}).base(), s.end());
	return s;
}

std::string toUpper(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
	return s;
}

std::vector<std::string> TokenizeInput(std::string input)
{
	std::string cleanInput = input;

	//substr before comment
	std::size_t comPos = cleanInput.find_first_of("//");
	cleanInput = cleanInput.substr(0, comPos);

	//trim whitespace
	cleanInput = ltrim(cleanInput);
	cleanInput = rtrim(cleanInput);
	
	std::cout << cleanInput;
	std::regex reg("\\s+");

	std::sregex_token_iterator iter(input.begin(), input.end(), reg, -1);
	std::sregex_token_iterator end;

	return std::vector<std::string>(iter, end);
}


int main()
{
	std::map<std::string, float> varMap;

	//move into a header somewhere else maybe
	const std::map<std::string, Commands> CommandMap{
		{"load", LOAD_COMMAND},
		{"mem", MEM_COMMAND},
		{"print", PRINT_COMMAND},
		{"stop", STOP_COMMAND}
	};

	std::cout << ">READY FOR INPUT\n";
	while (true)
	{
		std::cout << ">";
		std::string input;
		std::getline(std::cin, input);
		
		std::vector<std::string> tokenVec = TokenizeInput(input);
		
		HandleCommand(&tokenVec, CommandMap);
	}

}

void HandleLoad(std::map<std::string, float> *vars, std::string newVar)
{
	auto itr = vars->find(newVar);

	if (itr == vars->end()) 
	{
		std::cout << ">Error: That variable already exists.\n";
		return;
	}

	std::cout << ">ENTER VALUE FOR " + toUpper(newVar) + " ";
	float val = 0.0;

	std::cin >> val;
	vars->insert(std::pair<std::string, float>(newVar, val));
}

void HandleMem(std::map<std::string, float> *vars, std::string newVar)
{
	auto itr = vars->find(newVar);

	if (itr == vars->end())
	{
		std::cout << ">Error: That variable already exists.\n";
		return;
	}

	//vars->insert(std::pair<std::string, float>(newVar, nullptr));
}

void HandlePrint()
{

}

void HandleStop()
{

}

void HandleStatement()
{

}