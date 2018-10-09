#include "Interpreter.h"

int main()
{
	Interpreter curInterpreter;
	std::cout << ">READY FOR INPUT\n";

	while (true)
	{
		std::cout << ">";
		std::string input;
		std::getline(std::cin, input);

		curInterpreter.TokenizeInput(curInterpreter.toLower(input));
		curInterpreter.HandleCommand(curInterpreter.ParseCommand());
	}
}