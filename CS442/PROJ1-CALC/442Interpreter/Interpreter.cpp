#include "Interpreter.h"

Interpreter::Interpreter() {}

Interpreter::~Interpreter()
{

}

Interpreter::Keywords Interpreter::ParseCommand()
{
	int command = -1;
	auto itr = KeywordMap.find(tokenVec.at(0));
	if (itr != KeywordMap.end())
		command = itr->second;
	
	return static_cast<Keywords>(command);
}

void Interpreter::HandleCommand(int command)
{
	switch (command)
	{
		case LOAD:
			HandleLoad();
			break;
		case MEM:
			HandleMem();
			break;
		case PRINT:
			HandlePrint();
			break;
		case STOP:
			HandleStop();
			break;
		default:
			break;
	}
}

bool Interpreter::OperatorExists(char c)
{
	auto itr = OperatorMap.find(c);
	
	if (itr == OperatorMap.end())
		return false;
	
	return true;
}

void Interpreter::TokenizeInput(std::string input)
{
	tokenVec.clear();
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

	std::vector<std::string> cleanVec(iter, end);

	for (auto token : cleanVec)
	{
		std::string curExpression = "";
		for (auto c : token)
		{
			if (OperatorExists(c))
			{
				if (curExpression.length() > 0)
				{
					tokenVec.push_back(curExpression);
					curExpression = "";
				}
				tokenVec.push_back(std::string(1, c));
			}
			else
				curExpression += c;
		}
		if (curExpression.length() > 0)
			tokenVec.push_back(curExpression);	
	}
}


void Interpreter::HandleLoad()
{
	if (tokenVec.size() <= 1)
		return;

	std::string newVar = toLower(tokenVec.at(1));
	auto itr = varMap.find(newVar);

	if (itr != varMap.end())
	{
		std::cout << ">Error: That variable already exists.\n";
		return;
	}

	std::cout << ">ENTER VALUE FOR " + toUpper(newVar) + " ";
	float val = 0.0f;

	std::cin >> val;

	varMap.insert(std::pair<std::string, std::optional<float>>(newVar, CreateVariable(&val)));
	std::cin.ignore();
}

std::optional<float> Interpreter::CreateVariable(float* val)
{
	if (val == nullptr)
		return std::nullopt;
	else
		return std::optional<float>{*val};
}

void Interpreter::HandleMem()
{
	if (tokenVec.size() <= 1)
		return;

	std::string newVar = toLower(tokenVec.at(1));
	auto itr = varMap.find(newVar);

	if (itr != varMap.end())
	{
		std::cout << ">Error: That variable already exists.\n";
		return;
	}

	varMap.insert(std::pair<std::string, std::optional<float>>(newVar, CreateVariable()));
}

void Interpreter::HandlePrint()
{
	std::string printVar = toLower(tokenVec.at(1));
	auto itr = varMap.find(printVar);

	if (itr == varMap.end()) 
	{
		std::cout << ">ERROR: That variable doesn't exist.\n";
		return;
	}

	std::ostringstream ss;

	ss.precision(2);

	ss << ">VALUE OF ";
	ss << toUpper(printVar);
	ss << " IS ";

	if (varMap[printVar].has_value())
		ss << std::fixed << *varMap[printVar] << "\n";
	else
		ss << "NULL\n";

	std::cout << ss.str();
}

void Interpreter::HandleStop()
{

}

void Interpreter::HandleStatement()
{
	//first token should always be a variable
	//next token should be =
	//after the = sign, check for +,-,/,*,^,sqrt or variables
}