#include "Interpreter.h"

int main()
{
	//will change 90% of this when everything is working
	Interpreter curInterpreter;
	std::cout << ">READY FOR INPUT\n";
	while (true)
	{
		std::cout << ">";

		std::string input;
		std::getline(std::cin, input);
		curInterpreter.tokenVec = curInterpreter.TokenizeInput(input);
		curInterpreter.HandleCommand(curInterpreter.ParseCommand());
	}
	
}