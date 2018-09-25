#include "Interpreter.h"

Interpreter::Interpreter() {}

Interpreter::~Interpreter()
{

}

Interpreter::Keywords Interpreter::ParseCommand()
{
	int command = -1;
	auto itr = KeywordMap.find(tokenQueue.at(tokenQueue.size() - 1));
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
		case SQRT:
			HandleStatement();
		default:
			HandleStatement();
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

bool Interpreter::KeywordExists(std::string keyword)
{
	auto itr = KeywordMap.find(keyword);

	if (itr == KeywordMap.end())
		return false;

	return true;
}

bool Interpreter::VariableExists(std::string var)
{
	std::string newVar = toLower(tokenQueue.at(1));
	auto itr = varMap.find(toLower(var));

	if (itr == varMap.end())
		return false;

	return true;
}

void Interpreter::PrintErrorStatement(Errors error, std::string token)
{
	switch (error)
	{
		case INVALID_SYNTAX:
			std::cout << ">ERROR: INVALID SYNTAX AT " << token << "\n";
			break;
		case UNDECL_VAR:
			std::cout << ">ERROR: VARIABLE IS UNDECLARED AT " << token << "\n";
			break;
		case UNINIT_VAR:
			std::cout << ">ERROR: VARIABLE IS UNITIALIZED AT " << token << "\n";
			break;
		case EXIST_VAR:
			std::cout << ">ERROR: VARIABLE ALREADY EXISTS AT " << token << "\n";
			break;
	}
}

void Interpreter::TokenizeInput(std::string input)
{
	tokenQueue.clear();
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
					tokenQueue.push_front(curExpression);
					curExpression = "";
				}
				tokenQueue.push_front(std::string(1, c));
			}
			else
				curExpression += c;
		}
		if (curExpression.length() > 0)
			tokenQueue.push_front(curExpression);	
	}

	tokenStack = std::stack<std::string, std::deque<std::string>>(tokenQueue);

}


void Interpreter::HandleLoad()
{
	if (tokenQueue.size() <= 1)
		return;

	std::string newVar = toLower(tokenQueue.at(tokenQueue.size() - 2));
	auto itr = varMap.find(newVar);

	if (itr != varMap.end())
	{
		PrintErrorStatement(EXIST_VAR, newVar);
		return;
	}

	std::cout << ">ENTER VALUE FOR " + toUpper(newVar) + " ";
	
	float val = 0.0f;

	std::cin >> val;

	varMap.insert(std::pair(newVar, CreateVariable(&val)));
	std::cin.ignore();
}

std::optional<float> Interpreter::CreateVariable(float* val)
{
	if (!val)
		return std::nullopt;
	else
		return std::optional<float>{*val};
}

void Interpreter::HandleMem()
{
	if (tokenQueue.size() <= 1)
		return;

	std::string newVar = toLower(tokenQueue.at(tokenQueue.size() - 2));
	auto itr = varMap.find(newVar);

	if (itr != varMap.end())
	{
		PrintErrorStatement(EXIST_VAR, newVar);
		return;
	}

	varMap.insert(std::pair(newVar, CreateVariable()));
}

void Interpreter::HandlePrint()
{
	std::string printVar = toLower(tokenQueue.at(tokenQueue.size() - 2));
	auto itr = varMap.find(printVar);

	if (itr == varMap.end()) 
	{
		PrintErrorStatement(UNDECL_VAR, printVar);
		return;
	}

	std::ostringstream ss;

	if (varMap[printVar].has_value())
	{
		ss.precision(2);

		ss << ">VALUE OF ";
		ss << toUpper(printVar);
		ss << " IS ";
		ss << std::fixed << varMap[printVar].value() << "\n";
		std::cout << ss.str();
	}
	else
		PrintErrorStatement(UNINIT_VAR, printVar);

	
}

void Interpreter::HandleStop()
{

}

float Interpreter::applyOp(float curVal, float appVal, char op)
{
	switch (op)
	{
	case '+':
		return curVal + appVal;
	case '-':
		return curVal - appVal;
	case '/':
		return curVal / appVal;
	case '*':
		return curVal * appVal;
	case '^':
		return std::powf(curVal, appVal);
	case '=':
		return appVal;
	}
}

void Interpreter::HandleStatement()
{
	try
	{
		if (tokenQueue.size() < 1)
			return;

		//first token should always be a variable
		auto itr = varMap.find(tokenQueue.at(tokenQueue.size() - 1));

		//doesnt exist
		if (itr == varMap.end()) {
			PrintErrorStatement(UNDECL_VAR, toUpper(tokenQueue.at(tokenQueue.size() - 1)));
			return;
		}
		std::string varName = tokenStack.top();
		tokenStack.pop();
		//next token should be =
		if (tokenStack.top().compare("=") != 0)
		{
			PrintErrorStatement(INVALID_SYNTAX);
			return;
		}
		tokenStack.pop();
		float newVal = nanf("");
		char prevOp = '=';
		//add enum for previous token type to prevent errors

		const int stack_size = tokenStack.size();
		for (int i = 0; i < stack_size; i++)
		{
			if (tokenStack.size() == 0)
			{
				itr->second = newVal;
				return;
			}
				
			//check if it is a variable, keyword, or operator
			std::string curPiece = tokenStack.top();
			bool isKeyword = KeywordExists(curPiece);
			bool isOp = OperatorExists(curPiece[0]);
			bool isVar = VariableExists(curPiece);

			if (isOp)
			{
				prevOp = curPiece[0];
				tokenStack.pop();
				continue;
			}
			
			if (isVar)
			{
				//take var value and apply to newVal with prevOp
				float varVal;
				if (varMap[toLower(curPiece)].has_value())
					varVal = varMap[toLower(curPiece)].value();
				else
				{
					PrintErrorStatement(INVALID_SYNTAX, curPiece);
					return;
				}
				newVal = applyOp(newVal, varVal, prevOp);
				tokenStack.pop();
			}
			
			if (isKeyword)
			{
				if (toLower(curPiece).compare("sqrt") != 0)
				{
					PrintErrorStatement(INVALID_SYNTAX, curPiece);
					return;
				}
				tokenStack.pop();
				std::string next = tokenStack.top();
				if (OperatorExists(next[0]))
				{
					PrintErrorStatement(INVALID_SYNTAX, next);
					return;
				}
				if (VariableExists(toLower(next)))
				{
					float varVal;
					if (varMap[toLower(next)].has_value())
					{
						varVal = varMap[toLower(next)].value();
						newVal = sqrtf(varVal);
						tokenStack.pop();
						continue;
					}			
					else
					{
						PrintErrorStatement(INVALID_SYNTAX, next);;
						return;
					}		
				}
				else
				{
					newVal = sqrt(std::stof(next));
				}
			}
			//if all 3 false, we check if it is a number instead
			if (!isKeyword && !isOp && !isVar)
			{
				//bad if item prior was a keyword or variable
				try
				{
					if(isnan(newVal))
						newVal = applyOp(0, std::stof(curPiece), prevOp);
					else
						newVal = applyOp(newVal, std::stof(curPiece), prevOp);
				}
				catch (const std::out_of_range& oor)
				{
					PrintErrorStatement(INVALID_SYNTAX);
				}
				tokenStack.pop();
			}
		}
		itr->second = newVal;
	}
	catch (const std::out_of_range& oor)
	{
		PrintErrorStatement(INVALID_SYNTAX);
	}
}

