#include "Interpreter.h"

Interpreter::Interpreter() {}

Interpreter::Commands Interpreter::ParseCommand()
{
	int command = -1;
	auto itr = CommandMap.find(tokenVec.at(0));
	if (itr != CommandMap.end())
		command = itr->second;
	
	return static_cast<Commands>(command);
	
}

void Interpreter::HandleCommand(int command)
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

tokenContainer Interpreter::TokenizeInput(std::string input)
{
	std::string cleanInput = input;

	//substr before comment
	std::size_t comPos = cleanInput.find_first_of("//");
	cleanInput = cleanInput.substr(0, comPos);

	//trim whitespace
	cleanInput = ltrim(cleanInput);
	cleanInput = rtrim(cleanInput);
	
	std::regex reg("\\s+");

	std::sregex_token_iterator iter(input.begin(), input.end(), reg, -1);
	std::sregex_token_iterator end;

	return std::vector<std::string>(iter, end);
}

void Interpreter::HandleLoad()
{
	/*auto itr = vars->find(newVar);

	if (itr == vars->end()) 
	{
		std::cout << ">Error: That variable already exists.\n";
		return;
	}

	std::cout << ">ENTER VALUE FOR " + toUpper(newVar) + " ";
	float val = 0.0;

	std::cin >> val;
	vars->insert(std::pair<std::string, float>(newVar, val));*/
}

void Interpreter::HandleMem()
{
	//auto itr = vars->find(newVar);

	//if (itr == vars->end())
	//{
	//	std::cout << ">Error: That variable already exists.\n";
	//	return;
	//}

	//vars->insert(std::pair<std::string, float>(newVar, nullptr));
}

void Interpreter::HandlePrint()
{

}

void Interpreter::HandleStop()
{

}

void Interpreter::HandleStatement()
{

}